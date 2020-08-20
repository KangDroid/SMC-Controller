#include <iostream>
#include "smc.h"
#include "Tools.h"

using namespace std;

class FanManager {
private:
    int min_rpm;
    int max_rpm;
    int fan_number;
    int rpm_container;
    int temp_container;
    string key_auto; // For setting 0/1 for auto one
    string key_fan; // For setting write-value for each fan.
    string to_watch; // Temperature Watcher
    SMC smc_object;
public:
    FanManager();
    FanManager(int f_no);
    int get_temp();
    int get_rpm_container();
    int get_temp_container();
    void set_rpm_container(int a);
    void set_temp_container(int a);
    void init(int f_no, string what, int min_f, int max_f);
    // Get rpm from percentage
    int get_rpm(float percentage);
    // Get percentage from temp
    float get_percentage(float min, float max, float cur);
    void set_bytes(SMCVal_t* smc_value, string bits);
    // For MBP 2019 16" --> F0Md
    void set_key(SMCVal_t* smc_value, string force_key);
    void set_auto();
    void set_force();
    void set_rpm(int target_rpm);
};