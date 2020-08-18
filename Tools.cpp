#include "Tools.h"

Tools::Tools(float to_eval) {
    this->bits = "";
    this->evaluate = to_eval;
    mf.target_value = this->evaluate;
}
void Tools::append_bits(int n, int i) {

    // Prints the binary representation 
    // of a number n up to i-bits. 
    int k; 
    for (k = i - 1; k >= 0; k--) { 

        if ((n >> k) & 1) 
            bits += '1';
        else
            bits += '0';
    } 
}

void Tools::to_bits() {
    // bits[idx++] = var.raw.sign;
    // append_bits(var.raw.exponent, 8);
    append_bits(mf.raw.mantissa, 32);
}

string Tools::bin_to_dec(string bits, int bitidx) {
    char ref[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
    int ret_val = 0;

    for (int i = 0; i < bitidx; i++) {
        ret_val += pow(2, (bitidx - 1) - i) * (bits[i] - '0');
    }
    string target_value = "";
    target_value += ref[ret_val];
    return target_value;
}

string Tools::print_hex() {
    string bits_array[8];
    string final_hex[4];
    int st = 0, ed = 4;
    for (int i = 0; i < 8; i++) {
        bits_array[i] = bits.substr(st, ed);
        st += 4;
        bits_array[i] = bin_to_dec(bits_array[i], bits_array[i].length());
    }

    int bit_idx = 0;
    for (int i = 0; i < 4; i++) {
        final_hex[i] = bits_array[bit_idx] + bits_array[bit_idx + 1];
        bit_idx += 2;
    }
    string final_result = "";
    for (int i = 3; i >= 0; i--) {
        final_result += final_hex[i];
    }

    return final_result;
}