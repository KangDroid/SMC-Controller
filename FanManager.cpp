#include "FanManager.h"

FanManager::FanManager() {
    init(0, "", 0, 0);
}
FanManager::FanManager(int f_no) {
    init(f_no, "", 0, 0);
}

int FanManager::get_temp() {
    return smc_object.SMCGetTemp();
}

int FanManager::get_rpm_container() {
    return this->rpm_container;
}

int FanManager::get_temp_container() {
    return this->temp_container;
}

void FanManager::set_rpm_container(int a) {
    this->rpm_container = a;
}

void FanManager::set_temp_container(int a) {
    this->temp_container = a;
}

void FanManager::init(int f_no, string what, int min_f, int max_f) {
    this->fan_number = f_no;
    //F0Md, F0Tg
    this->key_auto = "F"+to_string(this->fan_number)+"Md";
    this->key_fan = "F"+to_string(this->fan_number)+"Tg";
    this->to_watch = what;
    this->min_rpm = min_f;
    this->max_rpm = max_f;
    smc_object.set_temp_watch(this->to_watch);
    smc_object.find_index_temp();
}
// Get rpm from percentage
int FanManager::get_rpm(float percentage) {
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

// Get percentage from temp
float FanManager::get_percentage(float min, float max, float cur) {
    float ret_val = ((cur - min) * 100) / (max - min);
    if (ret_val < 0) {
        ret_val = 0;
    }
    return ret_val;
}

void FanManager::set_bytes(SMCVal_t* smc_value, string bits) {
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
void FanManager::set_key(SMCVal_t* smc_value, string force_key) {
    sprintf(smc_value->key, force_key.c_str());
}

void FanManager::set_auto() {
    SMCVal_t smc_value;
    set_key(&smc_value, this->key_auto);
    set_bytes(&smc_value, "00");
    smc_object.SMCWriteKey(smc_value);
}

void FanManager::set_force() {
    SMCVal_t smc_value;
    set_key(&smc_value, this->key_auto);
    set_bytes(&smc_value, "01");
    smc_object.SMCWriteKey(smc_value);
}

void FanManager::set_rpm(int target_rpm) {
    SMCVal_t smc_value;
    Tools tmp_converter(target_rpm); tmp_converter.to_bits();
    int gradual_increase = 10;

    // Set key on SMCValue
    set_key(&smc_value, this->key_fan);

    // For Gradual Increase
    if (rpm_container != 0) {
        int increase_check = abs(target_rpm - rpm_container);
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
        int determin_sign = target_rpm - rpm_container;
        int to_iterate = increase_check / gradual_increase;
        if (increase_check % gradual_increase != 0) {
            to_iterate++;
        }
        // TMP
        int tmp_rpm = rpm_container;
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