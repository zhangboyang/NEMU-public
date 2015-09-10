#include "cpu/exec/helper.h"


static inline void do_execute_rm2r() { // move gpr to cr
	print_asm("lidt fword %s", op_src->str);
	cpu.IDTR = lnaddr_read(op_src->addr + 2, 4);
	cpu.IDT_LIMIT = lnaddr_read(op_src->addr, 2);
}

make_helper(lidt_rm2r_l) {
    return idex(eip, decode_rm2r_l, do_execute_rm2r);
}

