#include "trap.h"
int main()
{
    volatile float a, b, c;
    a = 0.5;
    b = 1.5;
    c = 2.0;
    nemu_assert(a + b == c);
    HIT_GOOD_TRAP;
    return 0;
}

