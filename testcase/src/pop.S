#include "trap.h"

.globl start
start:
    mov $0, %ebp
    mov $0x08000000, %esp
    sub $0x10, %esp
    
    pushl $0xaabbccdd
    pushl $0xdeadbeef
    
    pop %eax
    pop %cx
    pop %dx
    
    cmp $0xdeadbeef, %eax
    jne bad
    cmp $0xccdd, %cx
    jne bad
    cmp $0xaabb, %dx
    jne bad
    
    
    
    pushl $0x11223344
    pushl $0x55667788
    mov $buf, %eax
    
    popl (%eax)
    cmpl $0x55667788, (%eax)
    jne bad
    
    popw (%eax)
    cmpw $0x3344, (%eax)
    jne bad
    
    popw (%eax)
    cmpw $0x1122, (%eax)
    jne bad
    
    HIT_GOOD_TRAP
bad:
    HIT_BAD_TRAP
buf:
.byte 0xcc
.byte 0xcc
.byte 0xcc
.byte 0xcc
.byte 0xcc
.byte 0xcc
.byte 0xcc
.byte 0xcc


