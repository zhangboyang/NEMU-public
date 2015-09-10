#include "cpu/exec/helper.h"

extern void load_segment(int sreg_index, int selector);

make_helper(iretd) {

	print_asm("iretd");
	
	uint32_t new_eip = POP_DWORD();
	uint32_t new_cs = POP_DWORD();
	uint32_t new_eflags = POP_DWORD();
	
	load_segment(R_CS, new_cs);
	cpu.EIP = new_eip; cpu.protect_eip = 1;
	cpu.EFLAGS.EFLAGS = new_eflags;
	
/*	printf("iret: return to cs:eip = %04X:%08X\n", new_cs, new_eip);
	void print_registers(); print_registers();*/
    
	return 1;
}
