#include "trap.h"

.globl start
start:
    REALMACHINE_NOTSUITABLE
    
    mov $0, %ebp
    mov $0x08000000, %esp
    sub $0x10, %esp
    
    xor %eax, %eax
    mov $buf, %ecx
1:
    movb (%eax), %dl
    movb %dl, (%ecx)
    movb $0xcc, (%eax)
    sub $-1, %eax
    sub $-1, %ecx
    cmp $0x100, %eax
    jne 1b
    
    xor %eax, %eax
    jz move_program
    HIT_BAD_TRAP
    HIT_BAD_TRAP
    HIT_BAD_TRAP
    HIT_BAD_TRAP
    HIT_BAD_TRAP
    HIT_BAD_TRAP
    HIT_BAD_TRAP
    HIT_BAD_TRAP
    HIT_BAD_TRAP
    HIT_BAD_TRAP
    HIT_BAD_TRAP
    HIT_BAD_TRAP
    HIT_BAD_TRAP
    HIT_BAD_TRAP
    HIT_BAD_TRAP
    HIT_BAD_TRAP
    
    
program_to_move:
    xor %eax, %eax
    xor %ebx, %ebx
    mov $1, %eax
    test %eax, %eax
.byte 0x66 /* 16-bit jnz */
.byte 0x0f
.byte 0x85
.byte 0x06
.byte 0x00

.byte 0xcc
.byte 0xcc
.byte 0xcc
.byte 0xcc
.byte 0xcc
.byte 0xcc
    mov $test_ok, %eax
    call *%eax
    HIT_BAD_TRAP
    HIT_BAD_TRAP
    HIT_BAD_TRAP
    


move_program: /* copy program to 0x10 */
    mov $program_to_move, %eax
    mov $0x10, %edx
1:
    movb (%eax), %cl
    movb %cl, (%edx)
    sub $-1, %eax
    sub $-1, %edx
    cmp $move_program, %eax
    jne 1b
    
    // data_to_write: (0x10 - (rel + 2)) & 0xffff
    mov $0xE, %eax
    mov $rel, %ecx
    sub %cx, %ax
    movw %ax, (%ecx)
    
    xor %eax, %eax
.byte 0x66 /* 16-bit je */
.byte 0x0f
.byte 0x84
rel:
.byte 0xcc
.byte 0xcc

    HIT_BAD_TRAP
    HIT_BAD_TRAP
    HIT_BAD_TRAP
test_ok:
    mov $buf, %ecx
    xor %eax, %eax
1:
    mov (%ecx), %dl
    mov %dl, (%eax)
    sub $-1, %eax
    sub $-1, %ecx
    cmp $0x100, %eax
    jne 1b
    
    HIT_GOOD_TRAP
    
    HIT_BAD_TRAP
    HIT_BAD_TRAP
    HIT_BAD_TRAP
buf:
