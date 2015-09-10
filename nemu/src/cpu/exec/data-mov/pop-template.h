#include "cpu/exec/template-start.h"

#define instr pop

#if DATA_BYTE == 2 || DATA_BYTE == 4
static inline void do_execute() {

#if DATA_BYTE == 2
	OPERAND_W(op_src, POP_WORD());
#elif DATA_BYTE == 4
	OPERAND_W(op_src, POP_DWORD());
#else
#error unknown DATA_BYTE
#endif

	print_asm_template1();
	//printf("DATA_BYTE = %d\n", DATA_BYTE);
}


make_instr_helper(rm)
make_instr_helper(r)
#endif


#include "cpu/exec/template-end.h"
