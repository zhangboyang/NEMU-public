#include "trap.h"
int main()
{
    volatile int a = 1;
    a = 0;
    nemu_assert(a);
    
    return 0;
}
