#include "cpu/exec/template-start.h"

#define instr cmpxchg

static inline void do_execute () {
    //EFLAGS(op_dest->val, op_src->val, 1);
    INVF_ALU(); // too lazy to update flags

    DATA_TYPE a = REG(R_EAX);
    if (a == op_dest->val) {
        WRITEF(ZF, 1);
	    OPERAND_W(op_dest, op_src->val);
    } else {
        WRITEF(ZF, 0);
        cpu.EAX = (cpu.EAX & ~MASK) | (op_dest->val & MASK);
    }

	print_asm_template2();
}

make_instr_helper(r2rm);

#include "cpu/exec/template-end.h"
