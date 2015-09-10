#include "cpu/exec/template-start.h"

#ifdef DEBUG
#define PRINT_JCC_ASM(INSTR) \
	print_asm(str(INSTR) " offset:0x%x", op_src->val);
#else
#define PRINT_JCC_ASM(INSTR) do { } while (0);
#endif

/* note: immediate should be sign extented */
#define JCC_DO_EXECUTE(INSTR, TYPE, COND) \
static inline void do_execute_with_instr_and_type(INSTR, TYPE)() { \
    if (COND) { \
        if (DATA_BYTE == 1 || DATA_BYTE == 4) \
            cpu.EIP += (uint32_t)((int32_t)((DATA_TYPE_S) op_src->val)); \
        else { \
            assert(0); \
        } \
    } \
    PRINT_JCC_ASM(INSTR) \
}

#define JCC_MAKE_INSTR_HELPER(INSTR) make_instr_helper_with_instr(i, INSTR)

#define MAKE_JCC(INSTR, COND) \
    JCC_DO_EXECUTE(INSTR, i, COND) \
    JCC_MAKE_INSTR_HELPER(INSTR)


#define OF READF(OF)
#define SF READF(SF)
#define ZF READF(ZF)
#define PF READF(PF)
#define CF READF(CF)


/* JCC */
MAKE_JCC(jo , OF == 1)              // 70
MAKE_JCC(jno, OF == 0)              // 71
MAKE_JCC(jb , CF == 1)              // 72
MAKE_JCC(jae, CF == 0)              // 73
MAKE_JCC(je , ZF == 1)              // 74
MAKE_JCC(jne, ZF == 0)              // 75
MAKE_JCC(jbe, CF == 1 || ZF == 1)   // 76
MAKE_JCC(ja , CF == 0 && ZF == 0)   // 77
MAKE_JCC(js , SF == 1)              // 78
MAKE_JCC(jns, SF == 0)              // 79
MAKE_JCC(jpe, PF == 1)              // 7a
MAKE_JCC(jpo, PF == 0)              // 7b
MAKE_JCC(jl , SF != OF)             // 7c
MAKE_JCC(jge, SF == OF)             // 7d
MAKE_JCC(jle, ZF == 1 || SF != OF)  // 7e
MAKE_JCC(jg , ZF == 0 && SF == OF)  // 7f
MAKE_JCC(jmp, 1)                    // eb/e9

#if DATA_BYTE == 2 || DATA_BYTE == 4
static inline void do_execute_with_instr_and_type(jmp, rm)() {
    DB2_PANIC("jmp r/m16");
	cpu.EIP = op_src->val; cpu.protect_eip = 1;
#define instr jmp
	print_asm_template1();
#undef instr
}

make_instr_helper_with_instr(rm, jmp)
#endif

#if DATA_BYTE == 1
MAKE_JCC(jecxz, (ops_decoded.is_addr_size_16 ? cpu.CX : cpu.ECX) == 0)
#endif

#undef PRINT_JCC_ASM
#undef JCC_DO_EXECUTE
#undef JCC_MAKE_INSTR_HELPER
#undef MAKE_JCC
#undef OF
#undef SF
#undef ZF
#undef PF
#undef CF

#include "cpu/exec/template-end.h"
