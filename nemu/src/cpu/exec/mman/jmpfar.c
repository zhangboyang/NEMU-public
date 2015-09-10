#include "cpu/exec/helper.h"

extern void load_segment(int sreg_index, int selector);

make_helper(jmpfar) {


    uint32_t seg_selector = instr_fetch(eip + 1 + 4, 2);
    uint32_t new_eip = instr_fetch(eip + 1, 4);
	print_asm("jmp far %04x:%08x", seg_selector, new_eip);

    load_segment(R_CS, seg_selector);
    cpu.EIP = new_eip; cpu.protect_eip = 1;
    
    
	return 1 + 4 + 2;
}
