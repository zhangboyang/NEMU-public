#include "cpu/exec/template-start.h"

#ifdef DEBUG
#define PRINT_CMOVCC_ASM(INSTR) \
	print_asm(str(INSTR) " %s, %s", op_src->str, op_dest->str);
#else
#define PRINT_CMOVCC_ASM(INSTR) do { } while (0);
#endif

#define CMOVCC_DO_EXECUTE(INSTR, TYPE, COND) \
static inline void do_execute_with_instr_and_type(INSTR, TYPE)() { \
    if (COND) OPERAND_W(op_dest, op_src->val); \
    PRINT_CMOVCC_ASM(INSTR) \
}

#define CMOVCC_MAKE_INSTR_HELPER(INSTR) make_instr_helper_with_instr(rm2r, INSTR)

#define MAKE_CMOVCC(INSTR, COND) \
    CMOVCC_DO_EXECUTE(INSTR, rm2r, COND) \
    CMOVCC_MAKE_INSTR_HELPER(INSTR)


#define OF READF(OF)
#define SF READF(SF)
#define ZF READF(ZF)
#define PF READF(PF)
#define CF READF(CF)


/* CMOVCC */
MAKE_CMOVCC(cmovo , OF == 1)              // 40
MAKE_CMOVCC(cmovno, OF == 0)              // 41
MAKE_CMOVCC(cmovb , CF == 1)              // 42
MAKE_CMOVCC(cmovae, CF == 0)              // 43
MAKE_CMOVCC(cmove , ZF == 1)              // 44
MAKE_CMOVCC(cmovne, ZF == 0)              // 45
MAKE_CMOVCC(cmovbe, CF == 1 || ZF == 1)   // 46
MAKE_CMOVCC(cmova , CF == 0 && ZF == 0)   // 47
MAKE_CMOVCC(cmovs , SF == 1)              // 48
MAKE_CMOVCC(cmovns, SF == 0)              // 49
MAKE_CMOVCC(cmovpe, PF == 1)              // 4a
MAKE_CMOVCC(cmovpo, PF == 0)              // 4b
MAKE_CMOVCC(cmovl , SF != OF)             // 4c
MAKE_CMOVCC(cmovge, SF == OF)             // 4d
MAKE_CMOVCC(cmovle, ZF == 1 || SF != OF)  // 4e
MAKE_CMOVCC(cmovg , ZF == 0 && SF == OF)  // 4f


#undef PRINT_CMOVCC_ASM
#undef CMOVCC_DO_EXECUTE
#undef CMOVCC_MAKE_INSTR_HELPER
#undef MAKE_CMOVCC
#undef OF
#undef SF
#undef ZF
#undef PF
#undef CF

#include "cpu/exec/template-end.h"
