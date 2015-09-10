#include "trap.h"

.globl start
start:
	movl $0x10000, %eax
	movl $0x20000, %ebx
	
	movl $5, %ecx
	
1:
	movl $0x11223344, (%eax)
	add $4, %eax
	movl $0x55667788, (%ebx)
	add $4, %ebx
	sub $1, %ecx
	test %ecx, %ecx
    jnz 1b
	
	HIT_GOOD_TRAP

