#include "trap.h"
#include <string.h>

const char str[] = "Hello, world!\n";

int main() {
    REALMACHINE_NOTSUITABLE;
    
    int len;
	asm volatile (  "movl $4, %%eax;"	// system call ID, 4 = SYS_write
					"movl $1, %%ebx;"	// file descriptor, 1 = stdout
					"movl $str, %%ecx;"	// buffer address
					"movl $14, %%edx;"	// length
					"int $0x80":"=eax"(len));
    
    nemu_assert(len == strlen(str));
    
	HIT_GOOD_TRAP;

	return 0;
}
