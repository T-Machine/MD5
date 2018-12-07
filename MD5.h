#ifndef _MD5_
#define _MD5_
#include <iostream>
#include <vector>
#include <string.h>
#include <sstream>
using namespace std;

typedef unsigned int bits_32;
typedef unsigned char byte;

struct MD_buffer {
    bits_32 A;
    bits_32 B;
    bits_32 C;
    bits_32 D;
};

//512 bits == 64 bytes
struct bits_512 {
    bits_32 bits[16];
};

class MD5 {
private:
    vector<byte> padding_byte;
    vector<bits_512> groups;
    MD_buffer buffer;
    byte little_endding_bytes[16];

public:
    MD5(string s);
    string get_encode();
    void MD5_padding(const byte* input, size_t length);
    void MD5_process();
    MD_buffer get_final();
    MD_buffer H_MD5(MD_buffer CV, bits_512 Y);
    bits_32 F(bits_32 b, bits_32 c, bits_32 d);
    bits_32 G(bits_32 b, bits_32 c, bits_32 d);
    bits_32 H(bits_32 b, bits_32 c, bits_32 d);
    bits_32 I(bits_32 b, bits_32 c, bits_32 d);
    bits_32 cycle_left_shift(bits_32 n, int s);
};


#endif