#include "cpu/exec/helper.h"

extern void raise_intr(uint8_t NO);

make_helper(int_i_b) {


    uint32_t intr_number = instr_fetch(eip + 1, 1);
	print_asm("int 0x%02x", intr_number);

    cpu.EIP += 2; // set to next instr's addr
    raise_intr(intr_number);
    
    panic("shouldn't reach here");
    
	return 2;
}
