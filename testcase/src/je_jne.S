#include "trap.h"

.globl start
start:
    mov $0, %ebp
    mov $0x08000000, %esp
    sub $0x10, %esp
    
    mov $10000, %eax
    test %eax, %eax
    jnz long_jmp;
    HIT_BAD_TRAP
    HIT_BAD_TRAP
    HIT_BAD_TRAP
    
long_jmp_back:

    mov $10000, %eax
1:
    sub $1, %eax
    test %eax, %eax
    jnz 1b

    
    mov $0, %eax
    mov $0, %ebx
    mov $0, %ecx
    mov $0, %edx
    mov $0, %esi
    mov $0, %edi
    
    test %eax, %eax
    jz 1f
    HIT_BAD_TRAP
1:
    test %ebx, %ebx
    jz 1f
    HIT_BAD_TRAP
1:
    test %ecx, %ecx
    jz 1f
    HIT_BAD_TRAP
1:
    test %edx, %edx
    jz 1f
    HIT_BAD_TRAP
1:
    test %esi, %edi
    jz 1f
    HIT_BAD_TRAP
1:

    mov $12345, %eax
1:
    sub $-1, %eax
    or %eax, %ebx
    and %eax, %ecx
    xor %eax, %edx
    or %eax, %esi
    xor %eax, %edi
    cmp $23456, %eax
    jne 1b
    
    cmp %ebx, %esi
    jne bad
    cmp %edx, %edi
    je 1f
    
1:
    cmp $0xdeaddead, %ebx
    jne 1f
    HIT_BAD_TRAP
1:
    cmp $0x00007fff, %ebx
    jne bad
    cmp $0x00005ba1, %edx
    jne bad
    
    
    
    
    
    mov $0x08000000, %esp
    sub $0x10, %esp
    

    /* prime number */
    mov $200, %esi
    push $2
    mov $3, %eax
    
big_loop:
    mov $2, %edx
tf_loop:
    mov %eax, %ecx
    // %ecx / %edx
div_loop:
    sub %edx, %ecx
    jz not_prime // not prime
    test $0x80000000, %ecx
    jz div_loop
    
    sub $-1, %edx
    cmp %edx, %eax
    jne tf_loop

    // prime
    push %eax

not_prime:
    sub $-1, %eax
    cmp %eax, %esi
    jne big_loop




    /* check prime number */
    mov $0x08000000, %edi
    sub $0x10, %edi
    mov $prime_data, %ebx
    mov $0, %ecx
    mov $0, %edx
1:
    sub $4, %edi
    mov (%edi), %eax
    movb (%ebx), %cl
    sub $-1, %ebx
    cmp %eax, %ecx
    jne bad
    sub $-1, %edx
    cmp $25, %edx
    jne 1b
    je good




prime_data:
    .byte 0x2
    .byte 0x3
    .byte 0x5
    .byte 0x7
    .byte 0xb
    .byte 0xd
    .byte 0x11
    .byte 0x13
    .byte 0x17
    .byte 0x1d
    .byte 0x1f
    .byte 0x25
    .byte 0x29
    .byte 0x2b
    .byte 0x2f
    .byte 0x35
    .byte 0x3b
    .byte 0x3d
    .byte 0x43
    .byte 0x47
    .byte 0x49
    .byte 0x4f
    .byte 0x53
    .byte 0x59
    .byte 0x61

bad:
    HIT_BAD_TRAP
    HIT_BAD_TRAP
    HIT_BAD_TRAP
    HIT_BAD_TRAP
    HIT_BAD_TRAP
    HIT_BAD_TRAP
    HIT_BAD_TRAP
good:
    HIT_GOOD_TRAP

long_jmp:
    mov $0, %eax
    test %eax, %eax
    jz long_jmp_back
    HIT_BAD_TRAP
    HIT_BAD_TRAP
    HIT_BAD_TRAP

