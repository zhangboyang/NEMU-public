#include "cpu/exec/template-start.h"

#define instr inc

static inline void do_execute () {
	DATA_TYPE result = EFLAGS_BASE(op_src->val, 1, 0, 0, 0); /* inc will not update CF */
	OPERAND_W(op_src, result);

	print_asm_template1();
}

make_instr_helper(rm)
#if DATA_BYTE == 2 || DATA_BYTE == 4
make_instr_helper(r)
#endif

#include "cpu/exec/template-end.h"
