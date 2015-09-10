#include "trap.h"

typedef unsigned int UINT;
typedef int INT;

void UINTdivUINT(UINT a, UINT b, UINT *qp, UINT *rp)
{
    UINT r = 0, q = 0;
    int i;
    for (i = 0; i < 32; i++) {
        r = (r << 1) + (a >> 31);
        a <<= 1;
        q <<= 1;
        if (r >= b) {
            r -= b;
            q += 1;
        }
    }
    if (qp) *qp = q;
    if (rp) *rp = r;
}

void INTdivINT(INT a, INT b, INT *qp, INT *rp)
{
    int qf = 0, rf = 0;
    if (a < 0) { qf = rf = 1; a = -a; }
    if (b < 0) { qf ^= 1; b = -b; }

    UINTdivUINT(a, b, (UINT *) qp, (UINT *) rp);

    if (qp && qf) *qp = -*qp;
    if (rp && rf) *rp = -*rp;
}

INT data[] = {
    0, 1, 2, 3, 5, 7, 11, 13, 17, 19, 23, 29
    -1, -2, -3, -5, -7, -11, -13, -17, -19, -23, -29,

    20, 30, 50,
    -20, -30, -50,

    0x7ffffffa, 0x7ffffffb,
    0x7ffffffc, 0x7ffffffd,
    0x7ffffffe, 0x7fffffff,
    0x80000000, 0x80000001,
    0x80000002, 0x80000003,
    0x80000004, 0x80000005,

    0x6631a3c5, 0x24cb0563,
    0x2fa09939, 0xec806d82,
    0xf8a9e478, 0xe3c2dad4,
    0x9dc1dadd, 0x25384aa2,
    0x91a47169, 0xf5371f84,
    0x0f49b3dd, 0x1cfb42e0,
    0xe3665b91, 0x85fdacbd,
    0x2865e0d7, 0x6fb0e5e8,
    0x36fd4f37, 0xa26d7db4,
    0xf9035c37, 0x2d3cf3ca,
};

int data_size = sizeof(data) / sizeof(data[0]);

#define MAKE_TEST_FUNC(TYPE, FMT) \
void test_ ## TYPE() \
{ \
    TYPE a, b; \
    TYPE q, r; \
    int i, j; \
    for (i = 0; i < data_size; i++) \
        for (j = 0; j < data_size; j++) { \
            a = data[i]; \
            b = data[j]; \
            if (b == 0) continue; \
            TYPE ## div ## TYPE (a, b, &q, &r); \
            nemu_assert(q == a / b); \
            nemu_assert(r == a % b); \
        } \
}

MAKE_TEST_FUNC(INT, "%d")
MAKE_TEST_FUNC(UINT, "%u")

int main()
{
    test_INT();
    test_UINT();
    HIT_GOOD_TRAP;
    return 0;
}

