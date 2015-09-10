#include "cpu/exec/helper.h"

#define DATA_BYTE 2
#include "cmovcc-template.h"
#undef DATA_BYTE

#define DATA_BYTE 4
#include "cmovcc-template.h"
#undef DATA_BYTE

#define MAKE_CMOVCC_HELPER_V(INSTR) make_helper_v(concat(INSTR, _rm2r))

MAKE_CMOVCC_HELPER_V(cmovo)  // 40
MAKE_CMOVCC_HELPER_V(cmovno) // 41
MAKE_CMOVCC_HELPER_V(cmovb)  // 42
MAKE_CMOVCC_HELPER_V(cmovae) // 43
MAKE_CMOVCC_HELPER_V(cmove)  // 44
MAKE_CMOVCC_HELPER_V(cmovne) // 45
MAKE_CMOVCC_HELPER_V(cmovbe) // 46
MAKE_CMOVCC_HELPER_V(cmova)  // 47
MAKE_CMOVCC_HELPER_V(cmovs)  // 48
MAKE_CMOVCC_HELPER_V(cmovns) // 49
MAKE_CMOVCC_HELPER_V(cmovpe) // 4a
MAKE_CMOVCC_HELPER_V(cmovpo) // 4b
MAKE_CMOVCC_HELPER_V(cmovl)  // 4c
MAKE_CMOVCC_HELPER_V(cmovge) // 4d
MAKE_CMOVCC_HELPER_V(cmovle) // 4e
MAKE_CMOVCC_HELPER_V(cmovg)  // 4f
