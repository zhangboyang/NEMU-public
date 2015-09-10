#ifndef __SETCC_H__
#define __SETCC_H__

#define MAKE_SETCC_HELPER(INSTR) \
    make_helper(concat(INSTR, _rm_b)); \

MAKE_SETCC_HELPER(seto)  // 90
MAKE_SETCC_HELPER(setno) // 91
MAKE_SETCC_HELPER(setb)  // 92
MAKE_SETCC_HELPER(setae) // 93
MAKE_SETCC_HELPER(sete)  // 94
MAKE_SETCC_HELPER(setne) // 95
MAKE_SETCC_HELPER(setbe) // 96
MAKE_SETCC_HELPER(seta)  // 97
MAKE_SETCC_HELPER(sets)  // 98
MAKE_SETCC_HELPER(setns) // 99
MAKE_SETCC_HELPER(setpe) // 9a
MAKE_SETCC_HELPER(setpo) // 9b
MAKE_SETCC_HELPER(setl)  // 9c
MAKE_SETCC_HELPER(setge) // 9d
MAKE_SETCC_HELPER(setle) // 9e
MAKE_SETCC_HELPER(setg)  // 9f


#endif
