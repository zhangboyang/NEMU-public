#include "trap.h"

.globl start
start:
    mov $0, %ebp
    mov $0x08000000, %esp
    sub $0x10, %esp
    mov %esp, %esi
    
    call func1
    push %eax
    push %ecx
    push %edx
    push %ebx
    call func2
    call func3
    
    cmp %esp, %esi
    je test_ok
    HIT_BAD_TRAP
    
func1:
    push %eax
    push %ecx
    push %edx
    push %ebx
    pop %eax
    pop %eax
    pop %eax
    pop %eax
    xor %eax, %eax
    ret
    HIT_BAD_TRAP

func2:
    push %eax
    pop %eax
    push %eax
    pop %eax
    ret $16
    HIT_BAD_TRAP

func3:
    push %eax
    push %ecx
    push %edx
    push %ebx
    call func2
    xor %eax, %eax
    ret
    HIT_BAD_TRAP

test_ok:
    HIT_GOOD_TRAP
test_fail:
    HIT_BAD_TRAP
    HIT_BAD_TRAP
    HIT_BAD_TRAP
    
