#ifndef __CMOVCC_H__
#define __CMOVCC_H__

#define MAKE_CMOVCC_HELPER(INSTR) \
    make_helper(concat(INSTR, _rm2r_v)); \

MAKE_CMOVCC_HELPER(cmovo)  // 40
MAKE_CMOVCC_HELPER(cmovno) // 41
MAKE_CMOVCC_HELPER(cmovb)  // 42
MAKE_CMOVCC_HELPER(cmovae) // 43
MAKE_CMOVCC_HELPER(cmove)  // 44
MAKE_CMOVCC_HELPER(cmovne) // 45
MAKE_CMOVCC_HELPER(cmovbe) // 46
MAKE_CMOVCC_HELPER(cmova)  // 47
MAKE_CMOVCC_HELPER(cmovs)  // 48
MAKE_CMOVCC_HELPER(cmovns) // 49
MAKE_CMOVCC_HELPER(cmovpe) // 4a
MAKE_CMOVCC_HELPER(cmovpo) // 4b
MAKE_CMOVCC_HELPER(cmovl)  // 4c
MAKE_CMOVCC_HELPER(cmovge) // 4d
MAKE_CMOVCC_HELPER(cmovle) // 4e
MAKE_CMOVCC_HELPER(cmovg)  // 4f


#endif
