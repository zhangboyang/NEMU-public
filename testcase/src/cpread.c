/* cross page read test */
#include "trap.h"

int test_val[] = {
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
};

int val_cnt = sizeof(test_val) / sizeof(test_val[0]);

#define PAGE_SIZE 4096

void naive_memset(void *p, char c, int n)
{
    unsigned char *s = p;
    int i;
    for (i = 0; i < n; i++)
        s[i] = c;
}

void run_test(volatile unsigned data)
{
    static volatile int val[PAGE_SIZE + 1]; // alloc for 4 pages and additional one int
    volatile int n = PAGE_SIZE;
    volatile int i, offset;
    naive_memset((void *) val, 0xab, sizeof(val));
    for (i = 0; i <= n; i++) val[i] = data; // set n + 1 items
    for (offset = 0; offset < 4; offset++) {
        volatile int * volatile ptr = (void *) ((char *) val + offset);
        for (i = 0; i < n; i++) {
            nemu_assert(ptr[i] == data);
        }
        volatile unsigned char lsb = data & 0xff;
        data = (data >> 8) | (lsb << 24);
    }
}

int main()
{
    int i;
    for (i = 0; i < val_cnt; i++)
        run_test(test_val[i]);
    HIT_GOOD_TRAP;
    return 0;
}
