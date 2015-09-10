#include "cpu/exec/template-start.h"

#define instr push

static inline void do_execute() {

#if DATA_BYTE == 1
    PUSH_BYTE(op_src->val);
#elif DATA_BYTE == 2
	PUSH_WORD(op_src->val);
#elif DATA_BYTE == 4
	PUSH_DWORD(op_src->val);
#else
#error unknown DATA_BYTE
#endif

	print_asm_template1();
	//printf("DATA_BYTE = %d\n", DATA_BYTE);
}

#if DATA_BYTE == 2 || DATA_BYTE == 4
make_instr_helper(rm)
make_instr_helper(r)
#endif
make_instr_helper(i)


#include "cpu/exec/template-end.h"
