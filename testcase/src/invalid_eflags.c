#include "trap.h"
int main()
{
    REALMACHINE_NOTSUITABLE;
    __asm__ __volatile("shl %cl, %eax");
    __asm__ __volatile("seto %al"); // should cause NEMU crash
    HIT_BAD_TRAP;
    return 0;
}
