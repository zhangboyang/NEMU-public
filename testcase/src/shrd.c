#include "trap.h"
//#include <stdio.h>

typedef unsigned long long TYPE;

TYPE data[] = {
    0, 1, 2, 3, 5, 7, 11, 13, 17, 19, 23, 29
    -1, -2, -3, -5, -7, -11, -13, -17, -19, -23, -29,

    20, 30, 50,
    -20, -30, -50,

    0x7ffffffffffffffa, 0x7ffffffffffffffb,
    0x7ffffffffffffffc, 0x7ffffffffffffffd,
    0x7ffffffffffffffe, 0x7fffffffffffffff,
    0x8000000000000000, 0x8000000000000001,
    0x8000000000000002, 0x8000000000000003,
    0x8000000000000004, 0x8000000000000005,

    0x6631a3c5992d428b, 0x3a6ac5ed24cb0563,
    0x2fa099392715ddf4, 0x434c1488ec806d82,
    0xf8a9e478877a4107, 0x17f95918e3c2dad4,
    0x9dc1dadd05fe03c5, 0x4660525725384aa2,
    0x91a4716950c8f9b7, 0x7d85a9cbf5371f84,
    0x0f49b3ddf4f1e168, 0xbae686f31cfb42e0,
    0xe3665b91bc16d67e, 0xd808a3ba85fdacbd,
    0x2865e0d75a01f456, 0x292d75ea6fb0e5e8,
    0x36fd4f379934ed15, 0xff8f2d6ea26d7db4,
    0xf9035c37627d8250, 0x87d18d6b2d3cf3ca,  
};

int data_size = sizeof(data) / sizeof(data[0]);

TYPE naive_shift_right(TYPE a, int b)
{
    b &= 0x3f;
    union {
        volatile TYPE int64;
        volatile unsigned int int32[2];
    } data;
    
    data.int64 = a;
    
    while (b--) {
        data.int32[0] >>= 1;
        data.int32[0] += data.int32[1] << 31;
        data.int32[1] >>= 1;
    }
    
    return data.int64;
}

TYPE naive_naive_shift_right(volatile TYPE a, int b)
{
    switch (b % 64) {
        case 0 : return a >> 0 ;
        case 1 : return a >> 1 ;
        case 2 : return a >> 2 ;
        case 3 : return a >> 3 ;
        case 4 : return a >> 4 ;
        case 5 : return a >> 5 ;
        case 6 : return a >> 6 ;
        case 7 : return a >> 7 ;
        case 8 : return a >> 8 ;
        case 9 : return a >> 9 ;
        case 10 : return a >> 10 ;
        case 11 : return a >> 11 ;
        case 12 : return a >> 12 ;
        case 13 : return a >> 13 ;
        case 14 : return a >> 14 ;
        case 15 : return a >> 15 ;
        case 16 : return a >> 16 ;
        case 17 : return a >> 17 ;
        case 18 : return a >> 18 ;
        case 19 : return a >> 19 ;
        case 20 : return a >> 20 ;
        case 21 : return a >> 21 ;
        case 22 : return a >> 22 ;
        case 23 : return a >> 23 ;
        case 24 : return a >> 24 ;
        case 25 : return a >> 25 ;
        case 26 : return a >> 26 ;
        case 27 : return a >> 27 ;
        case 28 : return a >> 28 ;
        case 29 : return a >> 29 ;
        case 30 : return a >> 30 ;
        case 31 : return a >> 31 ;
        case 32 : return a >> 32 ;
        case 33 : return a >> 33 ;
        case 34 : return a >> 34 ;
        case 35 : return a >> 35 ;
        case 36 : return a >> 36 ;
        case 37 : return a >> 37 ;
        case 38 : return a >> 38 ;
        case 39 : return a >> 39 ;
        case 40 : return a >> 40 ;
        case 41 : return a >> 41 ;
        case 42 : return a >> 42 ;
        case 43 : return a >> 43 ;
        case 44 : return a >> 44 ;
        case 45 : return a >> 45 ;
        case 46 : return a >> 46 ;
        case 47 : return a >> 47 ;
        case 48 : return a >> 48 ;
        case 49 : return a >> 49 ;
        case 50 : return a >> 50 ;
        case 51 : return a >> 51 ;
        case 52 : return a >> 52 ;
        case 53 : return a >> 53 ;
        case 54 : return a >> 54 ;
        case 55 : return a >> 55 ;
        case 56 : return a >> 56 ;
        case 57 : return a >> 57 ;
        case 58 : return a >> 58 ;
        case 59 : return a >> 59 ;
        case 60 : return a >> 60 ;
        case 61 : return a >> 61 ;
        case 62 : return a >> 62 ;
        case 63 : return a >> 63 ;
    }
    return a;
}
void test()
{
    TYPE x, y;
    int i, j;
    for (i = 0; i < data_size; i++)
        for (j = 0; j <= 64; j++) {
            x = data[i];
            y = x >> j;
            nemu_assert(y == naive_shift_right(x, j));
            nemu_assert(y == naive_naive_shift_right(x, j));
        }
}

int main()
{
    test();
    HIT_GOOD_TRAP;
    return 0;
}
