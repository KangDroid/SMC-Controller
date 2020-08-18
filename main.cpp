#include <iostream>
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
    Tools tmp_converter(target_rpm);
    tmp_converter.to_bits();
    set_key(&smc_value, "F0Tg");
    set_bytes(&smc_value, tmp_converter.print_hex());
    //cout << "Target HEX: " << tmp_converter.print_hex() << endl;
    smc_object.SMCWriteKey(smc_value);
}

// This main function is for testing purpose
int main(void) {
    SMC smc_tmp("TC4C");
    float core_temp;
    float minimum_core = 10.0;
    float maximum_core = 80.0;
    int min_fan = 1836;
    int max_fan = 5616;
    int i = 0;
    set_force(smc_tmp);
    while (i < 30) {
        core_temp = smc_tmp.SMCGetTemp();
        if (core_temp == -1.0) {
            cout << "Error occured" << endl;
            return -1;
        }
        cout << "Core Temp: " << core_temp << endl;
        float percentage_tmp = get_percentage(minimum_core, maximum_core, core_temp);
        cout << "Percentage: " << percentage_tmp << endl;
        int rpm_target = get_rpm(min_fan, max_fan, percentage_tmp);
        cout << "Target RPM: " << rpm_target << endl;
        set_rpm(smc_tmp, rpm_target);
        i++;
        cout << endl;
        sleep(2);
    }
    set_auto(smc_tmp);
    return 0;
}