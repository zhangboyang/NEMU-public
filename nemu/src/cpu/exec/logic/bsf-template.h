#include "cpu/exec/template-start.h"

#define instr bsf

static inline void do_execute () {

	uint32_t val = op_src->val;

	if (val) {
    	WRITEF(ZF, 0);
    	
    	int temp = 0;
    	while ((val & (1 << temp)) == 0) temp++;
    	OPERAND_W(op_dest, temp);
    	
    } else {
        WRITEF(ZF, 1);
    }

	print_asm_template2();
}

make_instr_helper(rm2r)

#include "cpu/exec/template-end.h"
