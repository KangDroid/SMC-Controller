#ifndef __TOOLS_H__
#define __TOOLS_H__

#include <iostream>
#include <cstdio> 
#include <cmath>

using namespace std;
typedef union { 
    float target_value; 
    struct { 
        unsigned int mantissa : 32;
    } raw; 
} ieee_converter; 

class Tools {
private:
    string bits;
    float evaluate;
    ieee_converter mf;
public:
    Tools(float to_eval);
    void append_bits(int n, int i);
    void to_bits();
    string bin_to_dec(string bits, int bitidx);
    string print_hex();
};
#endif // __TOOLS_H__