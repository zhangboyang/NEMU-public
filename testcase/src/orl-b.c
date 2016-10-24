#include "trap.h"
int main()
{
    volatile int x;
    __asm__ __volatile__ (
        "mov $0x11223344, %%eax\n\t"
        ".byte 0x83\n\t"
        ".byte 0xc8\n\t"
        ".byte 0xff\n\t" // orl %eax, $0xffffffff
        :"=a"(x));
        
    nemu_assert(x == 0xffffffff);
    
    HIT_GOOD_TRAP;
    return 0;
}

