#include <iostream>
#include <stack>
#include <csignal>
#include "smc.h"
#include "Tools.h"
#include "FanManager.h"
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

FanManager fan_control[2];

void signal_handler(int signum) {
    cout << "Signal: " << signum << " Detected!" << endl;
    cout << "Cleaning up..." << endl;
    for (int i = 0; i < 2; i++) {
        fan_control[i].set_auto();
    }
    exit(signum);
}

// This main function is for testing purpose
int main(void) {
    signal(SIGINT, signal_handler);

    // Init Fan Manager
    fan_control[0].init(0, "TC4C", 1836, 5616);
    fan_control[1].init(1, "TC4C", 1700, 5200);
    float core_temp;
    float minimum_core = 20.0;
    float maximum_core = 40.0;
    int i = 0;
    for (int fc = 0; fc < 2; fc++) {
        fan_control[fc].set_force();
    }
    while (i < 30) {
        for (int fc = 0; fc < 2; fc++) {
            cout << "Working for Fan : " << fc << endl; 
            core_temp = fan_control[fc].get_temp();
            if (core_temp == -1.0) {
                cout << "Error occured" << endl;
                return -1;
            }
            if (fan_control[fc].get_temp_container() != 0) {
                if (abs(core_temp - fan_control[fc].get_temp_container()) < 2) {
                    cout << "Skipping, Core Temp: " << core_temp << endl;
                    cout << "Previous: " << fan_control[fc].get_temp_container() << endl;
                    i++;
                    sleep(2);
                    continue;
                }
            }
            cout << "Core Temp: " << core_temp << endl;
            float percentage_tmp = fan_control[fc].get_percentage(minimum_core, maximum_core, core_temp);
            cout << "Percentage: " << percentage_tmp << endl;
            int rpm_target = fan_control[fc].get_rpm(percentage_tmp);
            cout << "Target RPM: " << rpm_target << endl;
            fan_control[fc].set_rpm(rpm_target);
            fan_control[fc].set_rpm_container(rpm_target);
            fan_control[fc].set_temp_container(core_temp);
            i++;
            cout << endl;
        }
        sleep(4);
    }
    for (int fc = 0; fc < 2; fc++) {
        fan_control[fc].set_auto();
    }
    return 0;
}