#include "cpu/exec/template-start.h"

#define instr movzb
#if DATA_BYTE == 2 || DATA_BYTE == 4
static inline void do_execute() {
    //printf("op_src->reg=%d %p\n", op_src->reg, op_src);
    unsigned char val = op_src->type == OP_TYPE_REG ? reg_b(op_src->reg) : op_src->val;
    unsigned int result = val;
    OPERAND_W(op_dest, result);
	print_asm_template2();
}
make_instr_helper(rm2r)
#endif
#undef instr

#define instr movzw
#if DATA_BYTE == 2 || DATA_BYTE == 4
static inline void do_execute() {
#if DATA_BYTE == 2
        panic("no 16 bit movzw instruction");
#endif
    unsigned short val = op_src->val;
    unsigned int result = val;
    OPERAND_W(op_dest, result);
	print_asm_template2();
}
make_instr_helper(rm2r)
#endif
#undef instr



#include "cpu/exec/template-end.h"
