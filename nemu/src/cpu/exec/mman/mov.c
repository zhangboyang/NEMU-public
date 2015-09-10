#include "cpu/exec/helper.h"

// GPR to CR0, CR1, CR2, CR3 ...

static void mov_cr_do_execute_rm2r() { // move gpr to cr
	uint32_t modrm = instr_fetch(cpu.eip + 2, 1);
	int cr_index = (modrm >> 3) & 0x7;
	print_asm("mov %s, %%cr%d", op_src->str, cr_index);
	cpu.CR[cr_index] = op_src->val;
	if (cr_index == 3) { // mov data to CR3, should flush TLB
	    extern void flush_tlb();
	    //printf("TLB flushed\n");
	    flush_tlb();
	}
	if (cr_index == 0 || cr_index == 3) {
	    #ifdef USE_VERY_FAST_MEMORY
	    extern void make_translate_flushed();
	    make_translate_flushed();
	    #endif
	}
}

make_helper(mov_cr_rm2r_l) {
    return idex(eip, decode_rm2r_l, mov_cr_do_execute_rm2r);
}


static void mov_cr_do_execute_r2rm() { // mov cr to gpr
	uint32_t modrm = instr_fetch(cpu.eip + 2, 1);
	int cr_index = (modrm >> 3) & 0x7;
	print_asm("mov %%cr%d, %s", cr_index, op_dest->str);
	write_operand_l(op_dest, cpu.CR[cr_index]);
}

make_helper(mov_cr_r2rm_l) {
    return idex(eip, decode_r2rm_l, mov_cr_do_execute_r2rm);
}




// GPR to segment register
extern void load_segment(int sreg_index, int selector);

static void mov_sreg_do_execute_rm2r() { // move gpr to segment register
	uint32_t modrm = instr_fetch(cpu.eip + 1, 1);
	int sreg_index = (modrm >> 3) & 0x7;
	print_asm("mov %s, %%%s", op_src->str, seg_regs[sreg_index]);
	load_segment(sreg_index, op_src->val);
}

make_helper(mov_sreg_rm2r_w) {
    return idex(eip, decode_rm2r_w, mov_sreg_do_execute_rm2r);
} 
