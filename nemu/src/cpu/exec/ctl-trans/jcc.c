#include "cpu/exec/helper.h"

#define DATA_BYTE 1
#include "jcc-template.h"
#undef DATA_BYTE

#define DATA_BYTE 2
#include "jcc-template.h"
#undef DATA_BYTE

#define DATA_BYTE 4
#include "jcc-template.h"
#undef DATA_BYTE

/* for instruction encoding overloading */

#define MAKE_JCC_HELPER_V(INSTR) make_helper_v(concat(INSTR, _i))


MAKE_JCC_HELPER_V(jo)  // 70
MAKE_JCC_HELPER_V(jno) // 71
MAKE_JCC_HELPER_V(jb)  // 72
MAKE_JCC_HELPER_V(jae) // 73
MAKE_JCC_HELPER_V(je)  // 74
MAKE_JCC_HELPER_V(jne) // 75
MAKE_JCC_HELPER_V(jbe) // 76
MAKE_JCC_HELPER_V(ja)  // 77
MAKE_JCC_HELPER_V(js)  // 78
MAKE_JCC_HELPER_V(jns) // 79
MAKE_JCC_HELPER_V(jpe) // 7a
MAKE_JCC_HELPER_V(jpo) // 7b
MAKE_JCC_HELPER_V(jl)  // 7c
MAKE_JCC_HELPER_V(jge) // 7d
MAKE_JCC_HELPER_V(jle) // 7e
MAKE_JCC_HELPER_V(jg)  // 7f
MAKE_JCC_HELPER_V(jmp) // eb/e9

make_helper_v(jmp_rm) // ff/4

