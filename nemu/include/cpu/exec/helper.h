#ifndef __EXEC_HELPER_H__
#define __EXEC_HELPER_H__

#include "cpu/helper.h"
#include "cpu/decode/decode.h"

#define make_helper_v(name) \
	make_helper(concat(name, _v)) { \
	    if (unlikely(ops_decoded.is_data_size_16)) \
    		return concat(name, _w)(eip); \
        else \
            return concat(name, _l)(eip); \
	}

#define do_execute concat4(do_, instr, _, SUFFIX)

#define make_instr_helper(type) \
	make_helper(concat5(instr, _, type, _, SUFFIX)) { \
		return idex(eip, concat4(decode_, type, _, SUFFIX), do_execute); \
	}

/* add by ZBY */

/* for CALL */
#define do_execute_with_type(type) concat6(do_, instr, _, type, _, SUFFIX)
#define make_instr_helper_with_type(type) \
	make_helper(concat5(instr, _, type, _, SUFFIX)) { \
		return idex(eip, concat4(decode_, type, _, SUFFIX), do_execute_with_type(type)); \
	}

/* for JCC */
#define do_execute_with_instr_and_type(instr, type) concat6(do_, instr, _, type, _, SUFFIX)
#define make_instr_helper_with_instr(type, instr) \
	make_helper(concat5(instr, _, type, _, SUFFIX)) { \
		return idex(eip, concat4(decode_, type, _, SUFFIX), do_execute_with_instr_and_type(instr, type)); \
	}
	


/* 16-bit addr_size or data_size panic template */
#define ADDR16_PANIC(DESC) do { if (ops_decoded.is_addr_size_16) panic("16-bit addr_size of " DESC " is not implemented"); } while (0)
#define DATA16_PANIC(DESC) do { if (ops_decoded.is_data_size_16) panic("16-bit data_size of " DESC " is not implemented"); } while (0)

/* only add b to a on lower 16 bits, high 16 bits not touched */
#define WORD_ADD(a, b) ({ \
    uint32_t __a = (uint32_t)(a), __b = (uint32_t)(b); \
    (__a & 0xffff0000) | ((__a + __b) & 0xffff); \
})




extern char assembly[];
#ifdef DEBUG
#define print_asm(...) Assert(snprintf(assembly, 80, __VA_ARGS__) < 80, "buffer overflow!")
#else
#define print_asm(...)
#endif

#define print_asm_template1() \
	print_asm(str(instr) str(SUFFIX) " %s", op_src->str)

#define print_asm_template2() \
	print_asm(str(instr) str(SUFFIX) " %s,%s", op_src->str, op_dest->str)

#define print_asm_template3() \
	print_asm(str(instr) str(SUFFIX) " %s,%s,%s", op_src->str, op_src2->str, op_dest->str)

#endif
