#include "cpu/exec/template-start.h"

#define instr movsb
#if DATA_BYTE == 2 || DATA_BYTE == 4
static inline void do_execute() {
    signed char val = op_src->type == OP_TYPE_REG ? reg_b(op_src->reg) : op_src->val;
    int result = val;
    OPERAND_W(op_dest, result);
	print_asm_template2();
}
make_instr_helper(rm2r)
#endif
#undef instr

#define instr movsw
#if DATA_BYTE == 2 || DATA_BYTE == 4
static inline void do_execute() {
#if DATA_BYTE == 2
        panic("no 16 bit movzw instruction");
#endif
    short val = op_src->val;
    int result = val;
    OPERAND_W(op_dest, result);
	print_asm_template2();
}
make_instr_helper(rm2r)
#endif
#undef instr



#include "cpu/exec/template-end.h"
