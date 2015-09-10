#include "cpu/exec/template-start.h"

#ifdef DEBUG
#define PRINT_SETCC_ASM(INSTR) \
	print_asm(str(INSTR) " %s", op_src->str);
#else
#define PRINT_SETCC_ASM(INSTR) do { } while (0);
#endif

#define SETCC_DO_EXECUTE(INSTR, TYPE, COND) \
static inline void do_execute_with_instr_and_type(INSTR, TYPE)() { \
    OPERAND_W(op_src, (COND)); \
    PRINT_SETCC_ASM(INSTR) \
}

#define SETCC_MAKE_INSTR_HELPER(INSTR) make_instr_helper_with_instr(rm, INSTR)

#define MAKE_SETCC(INSTR, COND) \
    SETCC_DO_EXECUTE(INSTR, rm, COND) \
    SETCC_MAKE_INSTR_HELPER(INSTR)


#define OF READF(OF)
#define SF READF(SF)
#define ZF READF(ZF)
#define PF READF(PF)
#define CF READF(CF)


/* SETCC */
MAKE_SETCC(seto , OF == 1)              // 90
MAKE_SETCC(setno, OF == 0)              // 91
MAKE_SETCC(setb , CF == 1)              // 92
MAKE_SETCC(setae, CF == 0)              // 93
MAKE_SETCC(sete , ZF == 1)              // 94
MAKE_SETCC(setne, ZF == 0)              // 95
MAKE_SETCC(setbe, CF == 1 || ZF == 1)   // 96
MAKE_SETCC(seta , CF == 0 && ZF == 0)   // 97
MAKE_SETCC(sets , SF == 1)              // 98
MAKE_SETCC(setns, SF == 0)              // 99
MAKE_SETCC(setpe, PF == 1)              // 9a
MAKE_SETCC(setpo, PF == 0)              // 9b
MAKE_SETCC(setl , SF != OF)             // 9c
MAKE_SETCC(setge, SF == OF)             // 9d
MAKE_SETCC(setle, ZF == 1 || SF != OF)  // 9e
MAKE_SETCC(setg , ZF == 0 && SF == OF)  // 9f


#undef PRINT_SETCC_ASM
#undef SETCC_DO_EXECUTE
#undef SETCC_MAKE_INSTR_HELPER
#undef MAKE_SETCC
#undef OF
#undef SF
#undef ZF
#undef PF
#undef CF

#include "cpu/exec/template-end.h"
