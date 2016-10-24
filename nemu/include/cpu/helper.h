#ifndef __HELPER_H__
#define __HELPER_H__

#include "nemu.h"
#include "cpu/decode/operand.h"

/* All function defined with 'make_helper' return the length of the operation. */
#define make_helper(name) int name(swaddr_t eip)

#ifdef USE_VERY_FAST_MEMORY
static uint32_t __attribute__((noinline)) instr_fetch_slow(swaddr_t addr, size_t len) {
	return swaddr_read(addr, len, R_CS);
}

static inline uint32_t instr_fetch(swaddr_t addr, size_t len) {
    if (likely(cpu.fast_data_ptr != NULL)) {
        unsigned offset = addr - cpu.fast_data_base;
        uint32_t *data = (void *) cpu.fast_data_ptr + offset;
        return *data & ((1LL << (len << 3)) - 1);
    }
	return instr_fetch_slow(addr, len);
}
#else
static inline uint32_t instr_fetch(swaddr_t addr, size_t len) {
	return swaddr_read(addr, len, R_CS);
}
#endif

/* Instruction Decode and EXecute */
static inline int idex(swaddr_t eip, int (*decode)(swaddr_t), void (*execute) (void)) {
	/* eip is pointing to the opcode */
	int len = decode(eip + 1);
	execute();
	return len + 1;	// "1" for opcode
}

/* shared by all helper function */
extern Operands ops_decoded;

#define op_src (&ops_decoded.src)
#define op_src2 (&ops_decoded.src2)
#define op_dest (&ops_decoded.dest)



/* useful macros by ZBY */
#define MEM_W_DWORD(seg, addr, data) swaddr_write((addr), 4, (data), (seg))
#define MEM_W_WORD(seg, addr, data) swaddr_write((addr), 2, (data), (seg))
#define MEM_W_BYTE(seg, addr, data) swaddr_write((addr), 1, (data), (seg))
#define MEM_R_DWORD(seg, addr) swaddr_read((addr), 4, (seg))
#define MEM_R_WORD(seg, addr) swaddr_read((addr), 2, (seg))
#define MEM_R_BYTE(seg, addr) swaddr_read((addr), 1, (seg))

/* stack operations */
#define PUSH_DWORD(data) ({ cpu.ESP -= 4; MEM_W_DWORD(R_SS, cpu.ESP, (data)); })
#define PUSH_WORD(data) ({ cpu.ESP -= 2; MEM_W_WORD(R_SS, cpu.ESP, (data) & 0xffff); })
#define PUSH_BYTE(data) ({ cpu.ESP -= 4; MEM_W_DWORD(R_SS, cpu.ESP, (unsigned) (int) (signed char) ((data) & 0xff)); }) // push BTYE is just push DWORD (sign-extended)
#define POP_DWORD() ({ uint32_t ret = MEM_R_DWORD(R_SS, cpu.ESP); cpu.ESP += 4; ret; })
#define POP_WORD() ({ uint32_t ret = MEM_R_WORD(R_SS, cpu.ESP); cpu.ESP += 2; ret; })

#endif
