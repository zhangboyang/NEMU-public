#include "trap.h"

.globl start
start:

    // nemu command for testing:
    //    si; p *$esp, $eip
    call 1f
    HIT_BAD_TRAP
    HIT_BAD_TRAP
    HIT_BAD_TRAP
1:

    /* another test */
    mov $0xaaaaaaaa, %eax
    call 1f
    HIT_BAD_TRAP
    HIT_BAD_TRAP
    HIT_BAD_TRAP
    call 2f
1:
    call 1f
1:
    subl $10, (%esp)
    call *(%esp)
    
    HIT_BAD_TRAP
    HIT_BAD_TRAP
    HIT_BAD_TRAP
    
    
    /* another test */
2:
    mov $0xaaaaaaaa, %eax
    call 1f
1:
    movl (%esp), %ebx
    sub $-8, %ebx
    call *%ebx
    call 1f
1:
    sub (%esp), %ebx
    sub $-5, %ebx
    test %ebx, %ebx
    
    jz 1f
    
    HIT_BAD_TRAP
    HIT_BAD_TRAP
    HIT_BAD_TRAP
1:
    HIT_GOOD_TRAP

