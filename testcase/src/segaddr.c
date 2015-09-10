#include "trap.h"


/* use this file to test if 'rm->sreg' set properly */
/* use 'si' to single step through this file */


#define OP "leal "
//#define OP "movl "

int main()
{
    
    /*__asm__ __volatile__ (
        "int3\n\t"
        // DS
        OP "%%ds:(%%eax), %%eax\n\t"
        OP "%%ds:(%%ecx), %%eax\n\t"
        OP "%%ds:(%%edx), %%eax\n\t"
        OP "%%ds:(%%ebx), %%eax\n\t"
        OP "%%ds:(%%esi), %%eax\n\t"
        OP "%%ds:(%%edi), %%eax\n\t"
        OP "%%ds:(,%%eax,4), %%eax\n\t"
        OP "%%ds:(%%eax,%%ebp), %%eax\n\t"
        OP "%%ds:5(%%ebx,%%eax,8), %%eax\n\t"
        
        "nop\n\t"
        "nop\n\t"
        "nop\n\t"
        
        // SS
        OP "%%ss:(%%esp), %%eax\n\t"
        OP "%%ss:(%%ebp), %%eax\n\t"
        OP "%%ss:20(%%esp), %%eax\n\t"
        OP "%%ss:(%%esp), %%eax\n\t"
        OP "%%ss:(%%ebp), %%eax\n\t"
        OP "%%ss:(%%ebp,%%ebp), %%eax\n\t"
        OP "%%ss:(%%esp,%%ebp), %%eax\n\t"
        
    :::"eax");
    */
    HIT_GOOD_TRAP;
    return 0;
}
