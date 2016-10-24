#include "cpu/exec/helper.h"

#define MEM_GS_R(addr) ({ \
    uint32_t ret; \
    assert(cpu.seg_gs_prefix); \
    if (likely(cpu.seg_is_flat)) { \
        /* in vfmemory flat mode, we must manually do seg translation */ \
	    ret = swaddr_read((addr) + cpu.seg_base[R_GS], 4, R_DS); \
    } else { \
    	ret = swaddr_read((addr), 4, R_GS); \
    } \
    cpu.seg_gs_prefix = 0; \
    ret; \
})

make_helper(gsinstr)
{
    assert(!ops_decoded.is_data_size_16);
    assert(!ops_decoded.is_addr_size_16);
    
    int len = 0;
    swaddr_t addr = 0;
    int rdest = 0;
    uint8_t modrm = 0;
    
    
    uint8_t instr = instr_fetch(eip, 1);
    switch (instr) {
        case 0xa1: // moffset2a
    	    addr = instr_fetch(eip + 1, 4);
    	    rdest = R_EAX;
    	    len = 5;
    	    break;
    	case 0x8b: // mov rm2r
    	    modrm = instr_fetch(eip + 1, 1);
    	    addr = instr_fetch(eip + 2, 4);
    	    rdest = (modrm >> 3) & 7;
    	    len = 6;
    	    assert((modrm & 7) == 5);
    	    assert((modrm >> 6) == 0);
    	    break;
    	default:
    	    panic_to_ui("unknown instruction %02X with gs prefix\n", instr);
    }
    
    reg_l(rdest) = MEM_GS_R(addr);

	print_asm("mov" str(SUFFIX) " gs:0x%x,%%%s", addr, regsl[rdest]);
	return len;
}
