#include "trap.h"
int main()
{
    volatile int x, y;
    __asm__ __volatile__ (
        ".byte 0x6a\n\t"
        ".byte 0xff\n\t" // push $0xffffffff
        "pop %0\n\t"
        
        ".byte 0x6a\n\t"
        ".byte 0x00\n\t" // push $0x0
        "pop %1\n\t"
        :"=r"(x), "=r"(y));
        
    nemu_assert(x == 0xffffffff);
    nemu_assert(y == 0);

    __asm__ __volatile__ (
        ".byte 0x6a\n\t"
        ".byte 0x7f\n\t" // push $0x0000007f
        "pop %0\n\t"
        
        ".byte 0x6a\n\t"
        ".byte 0x80\n\t" // push $0xffffff80
        "pop %1\n\t"
        :"=r"(x), "=r"(y));
        
    nemu_assert(x == 0x0000007f);
    nemu_assert(y == 0xffffff80);
    
    HIT_GOOD_TRAP;
    return 0;
}
