#include "cpu/exec/template-start.h"

#define instr test

static inline void do_execute () {
	EFLAGS_B(op_dest->val & op_src->val);
	print_asm_template2();
}

make_instr_helper(i2a)
make_instr_helper(i2rm)
#if DATA_BYTE == 2 || DATA_BYTE == 4
make_instr_helper(si2rm)
#endif
make_instr_helper(r2rm)
make_instr_helper(rm2r)

#include "cpu/exec/template-end.h"
