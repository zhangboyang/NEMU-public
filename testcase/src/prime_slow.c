#include "trap.h"

int p[] = {
    0x2, 0x3, 0x5, 0x7, 0xb, 0xd, 0x11, 0x13, 0x17, 0x1d, 0x1f, 0x25, 0x29,
    0x2b, 0x2f, 0x35, 0x3b, 0x3d, 0x43, 0x47, 0x49, 0x4f, 0x53, 0x59, 0x61
};

int mod_with_sub(int a, int b)
{
    while (a > 0) a -= b;
    return a ? a + b : 0;
}

int is_prime(int x)
{
    int i;
    for (i = 2; i < x; i++)
        if (mod_with_sub(x, i) == 0)
            return 0;
    return 1;
}

int main()
{
    int i;
    int n = 200;
    int cnt = 0;
    for (i = 2; i <= n; i++)
        if (is_prime(i)) {
            //__asm__ __volatile__ ("int3" :: "a"(i));
            if (cnt < sizeof(p) / sizeof(int)) {
                if (i == p[cnt])
                    cnt++;
                else
                    goto fail;
            }
        }
    
    
    HIT_GOOD_TRAP;
fail:
    HIT_BAD_TRAP;
    return 0;
}
