#include"MD5.h"
using namespace std;

byte padding_val[64] = {
    0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

int X[4][16] = {
    {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
    {1, 6, 11, 0, 5, 10, 15, 4, 9, 14, 3, 8, 13, 2, 7, 12},
    {5, 8, 11, 14, 1, 4, 7, 10, 13, 0, 3, 6, 9, 12, 15, 2},
    {0, 7, 14, 5, 12, 3, 10, 1, 8, 15, 6, 13, 4, 11, 2, 9}
};

int S[4][16] = {
    {7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22},
    {5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20},
    {4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23},
    {6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21}
};

bits_32 T[4][16] = {
    {0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee, 
     0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
     0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,
     0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821},
    {0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa,
     0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
     0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed,
     0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a},
    {0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c,
     0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
     0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05,
     0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665},
    {0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039,
     0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
     0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,
     0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391}
};

bits_32 MD5::F(bits_32 b, bits_32 c, bits_32 d) {
    return (b & c) | (~b & d);
}

bits_32 MD5::G(bits_32 b, bits_32 c, bits_32 d) {
    return (b & d) | (c & ~d);
}

bits_32 MD5::H(bits_32 b, bits_32 c, bits_32 d) {
    return b ^ c ^ d;
}

bits_32 MD5::I(bits_32 b, bits_32 c, bits_32 d) {
    return c ^ (b | ~d);
}

bits_32 MD5::cycle_left_shift(bits_32 n, int s) {
    return (n << s) | (n >> (32 - s));
}

MD_buffer MD5::H_MD5(MD_buffer CV, bits_512 Y) {
    bits_32 A_ = CV.A;
    bits_32 B_ = CV.B;
    bits_32 C_ = CV.C;
    bits_32 D_ = CV.D;
    for(int i = 0; i < 4; i ++) {
        for(int j = 0; j < 16; j ++) {
            bits_32 temp;
            bits_32 A = A_;
            bits_32 B = B_;
            bits_32 C = C_;
            bits_32 D = D_;
            if(i == 0) {
                temp = F(B, C, D) + A + Y.bits[X[i][j]] + T[i][j];
            } else if(i == 1) {
                temp = G(B, C, D) + A + Y.bits[X[i][j]] + T[i][j];
            } else if(i == 2) {
                temp = H(B, C, D) + A + Y.bits[X[i][j]] + T[i][j];
            } else {
                temp = I(B, C, D) + A + Y.bits[X[i][j]] + T[i][j];
            }
            A = cycle_left_shift(temp, S[i][j]) + B;  //对A迭代
            A_ = D;
            B_ = A;
            C_ = B;
            D_ = C; //循环轮换
        }
    }
    CV.A += A_;
    CV.B += B_;
    CV.C += C_;
    CV.D += D_;
    
    return CV;
}

MD5::MD5(string s) {
    const byte* input = (const byte*)s.c_str();
    MD5_padding(input, s.length());
    MD5_process();
}

void MD5::MD5_padding(const byte* input, size_t length) {
    //填充
    int pad = length % 64;
    //若pad > 56，则 P = 56 - (pad - 56) + (64 - 56) = 120 - pad
    //若pad <= 56，则P = 56 - pad
    int P = pad <= 56 ? 56 - pad : 120 - pad;
    for(int i = 0; i < length; i ++) {
        byte b = input[i];
        padding_byte.push_back(b);
    }
    for(int i = 0; i < P; i ++) {
        byte b = padding_val[i];
        padding_byte.push_back(b);
    }
    //加上K的低64位
    size_t K = length * 8;
    for(int i = 0; i < 8; i ++) {
        byte b = (K >> (i * 8)) & 0xff;
        padding_byte.push_back(b);
    }

    //分块
    int bits_32_num = 0;
    int bytes_num = 0;
    bits_512 block;
    bits_32 words = 0;  //32bits 字
    for(int i = 0; i < padding_byte.size(); i ++) {
        byte b = padding_byte[i];
        words |= (b << (bytes_num * 8));
        bytes_num ++;
        if(bytes_num == 4) {    //4个byte组成一个32bits字
            block.bits[bits_32_num] = words;
            bytes_num = 0;
            words = 0;
            bits_32_num ++;
            if(bits_32_num == 16) {    //16个32bits字组成一个512bits的块
                groups.push_back(block);
                bits_32_num = 0;
            }
        }
    }
}

void MD5::MD5_process() {
    MD_buffer IV;
    IV.A = 0x67452301;
    IV.B = 0xEFCDAB89;
    IV.C = 0x98BADCFE;
    IV.D = 0x10325476;
    MD_buffer CV = IV;

    for(int i = 0; i < groups.size(); i ++) {
        CV = H_MD5(CV, groups[i]);
    }
    buffer = CV;
}

string MD5::get_encode() {
    //小端形式输出
    little_endding_bytes[0] = (byte)(buffer.A & 0xff);
    little_endding_bytes[1] = (byte)((buffer.A >> 8) & 0xff);
    little_endding_bytes[2] = (byte)((buffer.A >> 16) & 0xff);
    little_endding_bytes[3] = (byte)((buffer.A >> 24) & 0xff);
    little_endding_bytes[4] = (byte)(buffer.B & 0xff);
    little_endding_bytes[5] = (byte)((buffer.B >> 8) & 0xff);
    little_endding_bytes[6] = (byte)((buffer.B >> 16) & 0xff);
    little_endding_bytes[7] = (byte)((buffer.B >> 24) & 0xff);
    little_endding_bytes[8] = (byte)(buffer.C & 0xff);
    little_endding_bytes[9] = (byte)((buffer.C >> 8) & 0xff);
    little_endding_bytes[10] = (byte)((buffer.C >> 16) & 0xff);
    little_endding_bytes[11] = (byte)((buffer.C >> 24) & 0xff);
    little_endding_bytes[12] = (byte)(buffer.D & 0xff);
    little_endding_bytes[13] = (byte)((buffer.D >> 8) & 0xff);
    little_endding_bytes[14] = (byte)((buffer.D >> 16) & 0xff);
    little_endding_bytes[15] = (byte)((buffer.D >> 24) & 0xff);

    ostringstream s;
    s << hex;
    for(int i = 0; i < 16; i ++) {
        if(little_endding_bytes[i] < 16) s << 0;
        s << (int)little_endding_bytes[i];
    }
    return s.str();
}

MD_buffer MD5::get_final() {
    return buffer;
}
