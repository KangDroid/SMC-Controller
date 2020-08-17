#include <iostream>
#include "smc.h"
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
    return (int)(min_rpm + (rpm_per_percentage*percentage));
}

// This main function is for testing purpose
int main(void) {
    SMC smc_tmp;
    float core_temp;
    float minimum_core = 50.0;
    float maximum_core = 80.0;
    int min_fan = 1836;
    int max_fan = 5616;
    int i = 0;
    while (i < 10) {
        core_temp = smc_tmp.SMCGetTemp("TC4C");
        if (core_temp == -1.0) {
            cout << "Error occured" << endl;
            return -1;
        }
        cout << "Core Temp: " << core_temp << endl;
        float percentage_tmp = get_percentage(minimum_core, maximum_core, core_temp);
        cout << "Percentage: " << percentage_tmp << endl;
        cout << "Target RPM: " << get_rpm(min_fan, max_fan, percentage_tmp) << endl;
        i++;
        sleep(1);
    }
    return 0;
}