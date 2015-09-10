#if DATA_BYTE == 1
#define SUFFIX b
#define MASK ((uint32_t) 0xff)
#define SIGN_SHIFT 7
#elif DATA_BYTE == 2
#define SUFFIX w
#define MASK ((uint32_t) 0xffff)
#define SIGN_SHIFT 15
#elif DATA_BYTE == 4
#define SUFFIX l
#define MASK ((uint32_t) 0xffffffff)
#define SIGN_SHIFT 31
#else
#error unknown DATA_BYTE
#endif


#include "cpu/eflags-marco.h"

#define UPDATE_EFLAGS_BASE concat3(update_eflags_base, _, SUFFIX)
#define UPDATE_EFLAGS_ALU concat3(update_eflags_alu, _, SUFFIX)
#define UPDATE_EFLAGS concat3(update_eflags, _, SUFFIX)
#define UPDATE_EFLAGS_BINARY concat3(update_eflags_binary, _, SUFFIX)
#define UPDATE_EFLAGS_INVF_ALU concat3(update_eflags_invf_alu, _, SUFFIX)

uint32_t UPDATE_EFLAGS_BASE(uint32_t op1, uint32_t op2, uint32_t cin, uint32_t patch, uint32_t updatecf)
{
    return EFLAGS_BASE(op1, op2, cin, patch, updatecf);
}

uint32_t UPDATE_EFLAGS_ALU(uint32_t a, uint32_t b, uint32_t c, uint32_t p)
{
    return UPDATE_EFLAGS_BASE(a, b, c, p, 1);
}

uint32_t UPDATE_EFLAGS(uint32_t op1, uint32_t op2, uint32_t cin)
{
    return UPDATE_EFLAGS_ALU(op1, op2, cin, 0);
}

uint32_t UPDATE_EFLAGS_BINARY(uint32_t result)
{
    return EFLAGS_B(result);
}


#undef SUFFIX
#undef MASK
#undef SIGN_SHIFT


#undef EFLAGS_BASE
#undef EFLAGS_ALU
#undef EFLAGS
#undef EFLAGS_B

#undef UPDATE_EFLAGS_BASE
#undef UPDATE_EFLAGS_ALU
#undef UPDATE_EFLAGS
#undef UPDATE_EFLAGS_B
#undef UPDATE_EFLAGS_INVF_ALU
