#include "cpu/exec/helper.h"

#if DATA_BYTE == 1

#define SUFFIX b
#define DATA_TYPE uint8_t
#define DATA_TYPE_S int8_t

#elif DATA_BYTE == 2

#define SUFFIX w
#define DATA_TYPE uint16_t
#define DATA_TYPE_S int16_t

#elif DATA_BYTE == 4

#define SUFFIX l
#define DATA_TYPE uint32_t
#define DATA_TYPE_S int32_t

#else

#error unknown DATA_BYTE

#endif

#define REG(index) concat(reg_, SUFFIX) (index)
#define REG_NAME(index) concat(regs, SUFFIX) [index]

#define MEM_R(seg, addr) swaddr_read(addr, DATA_BYTE, (seg))
#define MEM_W(seg, addr, data) swaddr_write(addr, DATA_BYTE, data, (seg))

#define OPERAND_W(op, src) concat(write_operand_, SUFFIX) (op, src)

#define MSB(n) ((DATA_TYPE)(n) >> ((DATA_BYTE << 3) - 1))



/* defined by ZBY */
#if DATA_BYTE == 1
#define MASK ((uint32_t) 0xff)
#define SIGN_SHIFT 7
#elif DATA_BYTE == 2
#define MASK ((uint32_t) 0xffff)
#define SIGN_SHIFT 15
#elif DATA_BYTE == 4
#define MASK ((uint32_t) 0xffffffff)
#define SIGN_SHIFT 31
#else
#error unknown DATA_BYTE
#endif


/* panic if DATA_BYTE == 2 */
#if DATA_BYTE == 2
#define DB2_PANIC(DESC) do { panic("data byte 2 of " DESC " is not implemented"); } while (0)
#else
#define DB2_PANIC(DESC) do { } while (0)
#endif



#ifdef USE_EFLAGS_FUNC
/* EFLAGS : func-version */
#define UPDATE_EFLAGS_BASE concat3(update_eflags_base, _, SUFFIX)
#define UPDATE_EFLAGS_ALU concat3(update_eflags_alu, _, SUFFIX)
#define UPDATE_EFLAGS concat3(update_eflags, _, SUFFIX)
#define UPDATE_EFLAGS_BINARY concat3(update_eflags_binary, _, SUFFIX)
#define UPDATE_EFLAGS_INVF_ALU concat3(update_eflags_invf_alu, _, SUFFIX)

extern uint32_t UPDATE_EFLAGS_BASE(uint32_t op1, uint32_t op2, uint32_t cin, uint32_t patch, uint32_t updatecf);
extern uint32_t UPDATE_EFLAGS_ALU(uint32_t a, uint32_t b, uint32_t c, uint32_t p);
extern uint32_t UPDATE_EFLAGS(uint32_t op1, uint32_t op2, uint32_t cin);
extern uint32_t UPDATE_EFLAGS_BINARY(uint32_t result);

#define EFLAGS_BASE(op1, op2, cin, patch, updatecf) (UPDATE_EFLAGS_BASE((op1), (op2), (cin), (patch), (updatecf)))
#define EFLAGS_ALU(a, b, c, p) (UPDATE_EFLAGS_ALU((a), (b), (c), (p)))
#define EFLAGS(op1, op2, cin) (UPDATE_EFLAGS((op1), (op2), (cin)))
#define EFLAGS_B(result) (UPDATE_EFLAGS_BINARY((result)))

#else
/* EFLAGS : marco-version */
#include "cpu/eflags-marco.h"

#endif






/* INVF_ALU() : set OF, CF, ZF, PF, SF to invalid */
#define INVF_ALU() do { INVF(OF); INVF(CF); INVF(ZF); INVF(PF); INVF(SF); } while (0)

/* DF_DATA_BYTE : offset value by DF */
#define DF_DATA_BYTE (unlikely(READF(DF)) ? -DATA_BYTE : DATA_BYTE)

