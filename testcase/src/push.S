#include "trap.h"

.globl start
start:

    mov $0, %ebp
    mov $0x08000000, %esp
    sub $0x10, %esp
    
    
    // use this nemu command to test:
    //      si; x (0x8000000-$esp) $esp
    
    
    
    push $0x11                      // opcode = 6a
    push $0x22
    push $0x33
    push $0x44
    
    pushw $0xaabb                   // opcode = 68
    pushw $0xccdd
    pushl $0xaabbccdd
    
    mov $0x1234aaaa, %eax           // opcode = 50
    mov $0x5678bbbb, %ebx
    push %eax
    push %ebx
    push %ax
    push %bx
    
    push $0xdeadbeef
    mov %esp, %eax
    pushw (%eax)                    // opcode = FF/6
    pushw 2(%eax)
    pushl (%eax)
    
    push %eax
    push %ecx
    push %edx
    push %ebx
    push %esp
    push %ebp
    push %esi
    push %edi
    
    push %ax
    push %cx
    push %dx
    push %bx
    push %sp
    push %bp
    push %si
    push %di
    
	HIT_GOOD_TRAP

