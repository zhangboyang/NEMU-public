#include "cpu/exec/template-start.h"

#define instr rol

static inline void do_execute () {
	DATA_TYPE src = op_src->val;
	DATA_TYPE dest = op_dest->val;

	uint8_t count = src & 0x1f;
	dest = (dest << count) | (dest >> (DATA_BYTE * 8 - count));

	OPERAND_W(op_dest, dest);

	/* There is no need to update EFLAGS, since no other instructions 
	 * in PA will test the flags updated by this instruction.
	 */
	INVF_ALU(); /* set flags to invalid, since we are too lazy to update EFLAGS */

	print_asm_template2();
}

make_instr_helper(rm_1)
make_instr_helper(rm_cl)
make_instr_helper(rm_imm)

#include "cpu/exec/template-end.h"
