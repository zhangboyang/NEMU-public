#include "trap.h"

int main()
{
    volatile int a, c;
    
    __asm__ __volatile__ (
        "mov $0xAABBCCDD, %%eax\n\t"
        "mov $0x11223344, %%ecx\n\t"
        "xor %%edx, %%edx\n\t"
        "xor %%ebx, %%ebx\n\t"
        "mov $0xab, %%dh\n\t"
        "mov $0x23, %%bh\n\t"
        "mov $-1, %%esi\n\t"
        "mov $0x22334455, %%edi\n\t"
        
        // you may use esi/edi instead of dh/bh, that's wrong!
        "movsx %%dh, %%eax\n\t"
        "movsx %%bh, %%ecx\n\t"
        
        "mov %%eax, %0\n\t"
        "mov %%ecx, %1\n\t"
        
        :"=m"(a), "=m"(c)
        :
        :"eax", "ecx", "ebx", "edx", "esi", "edi");
    
    nemu_assert(a == 0xffffffab);
    nemu_assert(c == 0x23);
    
    HIT_GOOD_TRAP;
    return 0;
}
