#ifndef __JCC_H__
#define __JCC_H__

#define MAKE_JCC_HELPER(INSTR) \
    make_helper(concat(INSTR, _i_b)); \
    make_helper(concat(INSTR, _i_v)); \

MAKE_JCC_HELPER(jo)  // 70
MAKE_JCC_HELPER(jno) // 71
MAKE_JCC_HELPER(jb)  // 72
MAKE_JCC_HELPER(jae) // 73
MAKE_JCC_HELPER(je)  // 74
MAKE_JCC_HELPER(jne) // 75
MAKE_JCC_HELPER(jbe) // 76
MAKE_JCC_HELPER(ja)  // 77
MAKE_JCC_HELPER(js)  // 78
MAKE_JCC_HELPER(jns) // 79
MAKE_JCC_HELPER(jpe) // 7a
MAKE_JCC_HELPER(jpo) // 7b
MAKE_JCC_HELPER(jl)  // 7c
MAKE_JCC_HELPER(jge) // 7d
MAKE_JCC_HELPER(jle) // 7e
MAKE_JCC_HELPER(jg)  // 7f
MAKE_JCC_HELPER(jmp) // eb/e9

make_helper(jecxz_i_b); // e3
make_helper(jmp_rm_v); // ff/4

#endif
