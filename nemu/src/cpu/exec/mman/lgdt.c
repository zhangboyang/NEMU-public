#include "cpu/exec/helper.h"


static inline void do_execute_rm2r() { // move gpr to cr
	print_asm("lgdt fword %s", op_src->str);
	cpu.GDTR = lnaddr_read(op_src->addr + 2, 4);
	cpu.GDT_LIMIT = lnaddr_read(op_src->addr, 2);
}

make_helper(lgdt_rm2r_l) {
    return idex(eip, decode_rm2r_l, do_execute_rm2r);
}

