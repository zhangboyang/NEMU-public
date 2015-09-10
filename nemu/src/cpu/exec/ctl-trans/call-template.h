#include "cpu/exec/template-start.h"

#define instr call
#if DATA_BYTE == 2 || DATA_BYTE == 4

static inline void do_execute_with_type(i)() {

    // call rel16 is not implemented
    DB2_PANIC("call rel16");
    // if you want write here, don't forget:
    //   the statment below should be changed, too (16-bit need to sign extend)

	cpu.protect_eip = 2;
	cpu.orig_eip = cpu.EIP;
	
	cpu.EIP += op_src->val; // no need to sign extent, since this MUST BE 32bit
#ifdef DEBUG
	print_asm(str(instr) " offset:0x%x", op_src->val);
#endif
}

static inline void do_execute_with_type(rm)() {

    DB2_PANIC("call r/m16");
	
	cpu.protect_eip = 3;
	cpu.orig_eip = cpu.EIP;
	
	cpu.EIP = op_src->val;
	print_asm_template1();
}


make_instr_helper_with_type(rm)
make_instr_helper_with_type(i)
#endif

#include "cpu/exec/template-end.h"
