#include <iostream>
#include <stack>
#include "smc.h"
#include "Tools.h"
using namespace std;

/**
    TA0V 29.098 
    TB0T 33.699 
    TB1T 33.398 
    TB2T 33.699 
    TC0E 68.695 
    TC0F 71.141 
    TC0P 52.188 
    TC0T 0.809 
    TC1C 63.000 
    TC2C 69.000 
    TC3C 69.000 
    TC4C 64.000 
    TC5C 58.000 
    TC6C 60.000 
    TC7C 58.000 
    TC8C 58.000 
    TCGC 57.000 
    TCMX 72.016 
    TCSA 60.000 
    TCXC 72.016 
    TF0S 4.281 
    TG0P 48.500 
    TG1P 42.500 
    TGDD 55.000 
    TGDE 51.000 
    TGDF 54.746 
    TGDT 1.531 
    TGVF 39.625 
    TGVP 39.625 
    TH0F 33.289 
    TH0X 33.391 
    TH0a 33.391 
    TH0b 32.793 
    TH1a 32.883 
    TH1b 32.922 
    TM0P 41.938 
    TPCD 44.000 
    TTLD 32.562 
    TTRD 32.688 
    TW0P 43.312 
    TaLC 33.562 
    TaRC 33.625 
    Th1H 42.562 
    Th2H 47.062 
    Tm0P 50.625 
    Ts0P 32.996 
    Ts0S 35.281 
    Ts1P 30.996 
    Ts1S 36.094 
    Ts2S 34.609 
*/

// RPM Container
stack<int> rpm_container;
stack<int> temp_container;

// Get percentage from temp
float get_percentage(float min, float max, float cur) {
    float ret_val = ((cur - min) * 100) / (max - min);
    if (ret_val < 0) {
        ret_val = 0;
    }
    return ret_val;
}

// Get rpm from percentage
int get_rpm(int min_rpm, int max_rpm, float percentage) {
    // min = 1836, max = 5616
    int subs = max_rpm - min_rpm;
    float rpm_per_percentage = subs / 100.0;
    int target = (int)(min_rpm + (rpm_per_percentage*percentage));
    if (target < min_rpm) {
        target = min_rpm;
    }

    if (target > max_rpm) {
        target = max_rpm;
    }
    return target;
}

void set_bytes(SMCVal_t* smc_value, string bits) {
    int i;
    char c[3];
    for (i = 0; i < bits.length()/2; i++)
    {
        sprintf(c, "%c%c", bits.at(i*2), bits.at((i*2)+1));
        smc_value->bytes[i] = (int) strtol(c, NULL, 16);
    }
    smc_value->dataSize = bits.length() / 2;
    if ((smc_value->dataSize * 2) != bits.length())
    {
        printf("Error: value is not valid\n");
        return;
    }
}

// For MBP 2019 16" --> F0Md
void set_key(SMCVal_t* smc_value, string force_key) {
    sprintf(smc_value->key, force_key.c_str());
}

void set_auto(SMC& smc_object) {
    SMCVal_t smc_value;
    set_key(&smc_value, "F0Md");
    set_bytes(&smc_value, "00");
    smc_object.SMCWriteKey(smc_value);
}

void set_force(SMC& smc_object) {
    SMCVal_t smc_value;
    set_key(&smc_value, "F0Md");
    set_bytes(&smc_value, "01");
    smc_object.SMCWriteKey(smc_value);
}

void set_rpm(SMC& smc_object, int target_rpm) {
    SMCVal_t smc_value;
    Tools tmp_converter(target_rpm); tmp_converter.to_bits();
    int gradual_increase = 10;

    // Set key on SMCValue
    set_key(&smc_value, "F0Tg");

    // For Gradual Increase
    if (rpm_container.size() != 0) {
        int prev_rpm = rpm_container.top(); rpm_container.pop();
        int increase_check = abs(target_rpm - prev_rpm);
        if (increase_check == 0) {
            return;
        } else if (increase_check < 20) {
            gradual_increase = 2;
        } else if (increase_check < 150) {
            gradual_increase = 10; 
        } else if (increase_check < 600) {
            gradual_increase = 50;
        } else if (increase_check < 2500) {
            gradual_increase = 200;
        } else {
            gradual_increase = 500;
        }
        int determin_sign = target_rpm - prev_rpm;
        int to_iterate = increase_check / gradual_increase;
        if (increase_check % gradual_increase != 0) {
            to_iterate++;
        }
        // TMP
        int tmp_rpm = prev_rpm;
        for (int i = 0; i < to_iterate; i++) {
            if (determin_sign < 0) {
                tmp_rpm = tmp_rpm - gradual_increase;
            } else if (determin_sign > 0) {
                tmp_rpm = tmp_rpm + gradual_increase;
            }
            Tools bits_converter(tmp_rpm); bits_converter.to_bits();
            if (i == to_iterate - 1) {
                Tools tmp_cv(target_rpm); tmp_cv.to_bits();
                set_bytes(&smc_value, tmp_cv.print_hex());
            } else {
                set_bytes(&smc_value, bits_converter.print_hex());
            }
            smc_object.SMCWriteKey(smc_value);
            // Sleep 0.5s
            usleep(500000);
        }
    } else {
        cout << "EMpty on stack" << endl;
        set_bytes(&smc_value, tmp_converter.print_hex());
        smc_object.SMCWriteKey(smc_value);
    }
}

// This main function is for testing purpose
int main(void) {
    SMC smc_tmp("TC4C");
    float core_temp;
    float minimum_core = 50.0;
    float maximum_core = 80.0;
    int min_fan = 1836;
    int max_fan = 5616;
    int i = 0;
    set_force(smc_tmp);
    while (i < 60) {
        core_temp = smc_tmp.SMCGetTemp();
        if (core_temp == -1.0) {
            cout << "Error occured" << endl;
            return -1;
        }
        if (temp_container.size() != 0) {
            int tmp_ct = temp_container.top();
            if (abs(core_temp - tmp_ct) < 2) {
                cout << "Skipping, Core Temp: " << core_temp << endl;
                cout << "Previous: " << tmp_ct << endl;
                i++;
                sleep(2);
                continue;
            }
        }
        cout << "Core Temp: " << core_temp << endl;
        float percentage_tmp = get_percentage(minimum_core, maximum_core, core_temp);
        cout << "Percentage: " << percentage_tmp << endl;
        int rpm_target = get_rpm(min_fan, max_fan, percentage_tmp);
        cout << "Target RPM: " << rpm_target << endl;
        set_rpm(smc_tmp, rpm_target);
        rpm_container.push(rpm_target);
        temp_container.push(core_temp);
        i++;
        cout << endl;
        sleep(4);
    }
    set_auto(smc_tmp);
    return 0;
}