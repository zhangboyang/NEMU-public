#include "trap.h"

.globl start
start:

    mov $0, %eax
    add $1, %eax            // opcode = 83
    
    mov $0, %eax
    add $1, %ax             // opcode = 83
    
    mov $0, %eax
    add $1, %ah             // opcode = 80
    
    mov $0, %eax
    add $1, %al             // opcode = 2c
    
    mov $0, %eax
    add $0xaabbccdd, %eax   // opcode = 2d
    
    mov $0, %ebx
    add $0xaabbccdd, %ebx   // opcode = 81
    
    mov $0, %ebx
    add $0xaabb, %bx        // opcode = 81
    
    
    
    mov $0xaabb, %eax
    
    mov $0, %ebx
    add %al, %bl            // opcode = 28
    
    mov $0, %ebx
    add %ax, %bx            // opcode = 29
    
    mov $0, %ebx
    add %eax, %ebx          // opcode = 29
    
    
    
    mov $0x08000000, %esp
    sub $0x10, %esp
    movl $0xaabbccdd, (%esp)
    
    mov $0, %ebx
    addb (%esp), %bl            // opcode = 28
    
    mov $0, %ebx
    addw (%esp), %bx            // opcode = 29
    
    mov $0, %ebx
    addl (%esp), %ebx          // opcode = 29
    
	HIT_GOOD_TRAP

