#include "trap.h"
int main()
{
    int x, y;
    __asm__ __volatile__ (
        "mov $0x1a2b3c4d, %%eax\n\t"
        "clc\n\t"
        "mov $0xdeadbeef, %%edx\n\t"
        "setc %%al\n\t"
        "mov %%eax, %0\n\t"
        
        "mov $0x1a2b3c4d, %%eax\n\t"
        "stc\n\t"
        "mov $0xdeadbeef, %%edx\n\t"
        "setc %%al\n\t"
        "mov %%eax, %1\n\t"
        :"=m"(x), "=m"(y)
        :
        :"eax", "edx");
        
    nemu_assert(x == 0x1a2b3c00);
    nemu_assert(y == 0x1a2b3c01);
    
    HIT_GOOD_TRAP;
    return 0;
}
