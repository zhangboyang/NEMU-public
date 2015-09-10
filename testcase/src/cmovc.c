#include "trap.h"
int main()
{
    int x, y;
    __asm__ __volatile__ (
        "mov $0x11223344, %%eax\n\t"
        "mov $0x1a2b3c4d, %%edx\n\t"
        "clc\n\t"
        "mov $0xaabbccdd, %%ecx\n\t"
        "cmovc %%eax, %%edx\n\t"
        "mov %%edx, %0\n\t"
        
        "mov $0x11223344, %%eax\n\t"
        "mov $0x1a2b3c4d, %%edx\n\t"
        "stc\n\t"
        "mov $0xaabbccdd, %%ecx\n\t"
        "cmovc %%eax, %%edx\n\t"
        "mov %%edx, %1\n\t"
        :"=m"(x), "=m"(y)
        :
        :"eax", "ecx", "edx");
        
    nemu_assert(x == 0x1a2b3c4d);
    nemu_assert(y == 0x11223344);
    
    HIT_GOOD_TRAP;
    return 0;
}
