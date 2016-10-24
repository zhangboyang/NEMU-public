#include "cpu/decode/modrm.h"
#include "cpu/helper.h"

#if !defined(USE_VERY_FAST_MEMORY) && !defined(USE_VERY_FAST_MEMORY_VER2)
int load_addr(swaddr_t eip, ModR_M *m, Operand *rm) {
	assert(m->mod != 3);

	int32_t disp;
	int instr_len, disp_offset, disp_size = 4;
	int base_reg = -1, index_reg = -1, scale = 0;
	swaddr_t addr = 0;

    rm->sreg = R_DS;
    
	if(m->R_M == R_ESP) {
		SIB s;
		s.val = instr_fetch(eip + 1, 1);
		base_reg = s.base;
		disp_offset = 2;
		scale = s.ss;

		if(s.index != R_ESP) { index_reg = s.index; }
	}
	else {
		// no SIB
		base_reg = m->R_M;
		disp_offset = 1;
	}

	if(m->mod == 0) {
		if(base_reg == R_EBP) { base_reg = -1; }
		else { disp_size = 0; }
	}
	else if(m->mod == 1) { disp_size = 1; }

	instr_len = disp_offset;
	if(disp_size != 0) {
		// has disp
		disp = instr_fetch(eip + disp_offset, disp_size);
		if(disp_size == 1) { disp = (int8_t)disp; }

		instr_len += disp_size;
		addr += disp;
	}

	if(base_reg >= 0) {
		addr += reg_l(base_reg);
		if (base_reg == R_ESP || base_reg == R_EBP) {
    		rm->sreg = R_SS;
        }
	}

	if(index_reg >= 0) {
		addr += reg_l(index_reg) << scale;
	}

#ifdef DEBUG
	char disp_buf[16];
	char base_buf[8];
	char index_buf[8];

	if(disp_size != 0) {
		// has disp
		sprintf(disp_buf, "%s%#x", (disp < 0 ? "-" : ""), (disp < 0 ? -disp : disp));
	}
	else { disp_buf[0] = '\0'; }

	if(base_reg == -1) { base_buf[0] = '\0'; }
	else { 
		sprintf(base_buf, "%%%s", regsl[base_reg]); 
	}

	if(index_reg == -1) { index_buf[0] = '\0'; }
	else { 
		sprintf(index_buf, ",%%%s,%d", regsl[index_reg], 1 << scale); 
	}

	if(base_reg == -1 && index_reg == -1) {
		sprintf(rm->str, "%s", disp_buf);
	}
	else {
		sprintf(rm->str, "%s:%s(%s%s)", seg_regs[rm->sreg], disp_buf, base_buf, index_buf);
	}
#endif

	rm->type = OP_TYPE_MEM;
	rm->addr = addr;

	return instr_len;
}
#else
// faster version of load_addr

inline int load_addr(swaddr_t eip, ModR_M *m, Operand *rm) {

	
    //static int cnt[100]; if (cnt[99]++ % 100000 == 0) { int i; for (i = 0; i < 10; i++) { printf("cnt[%d] = %d\n", i, cnt[i]); cnt[i] = 0; } printf("\n"); }

    int base_reg = m->R_M;
    int mod = m->mod;

    unsigned eip_p1_data = instr_fetch(eip + 1, 4);
    rm->type = OP_TYPE_MEM;
    
	if (base_reg == R_ESP) {
    	swaddr_t addr = 0;
		SIB s;
		s.val = eip_p1_data;
		rm->type = OP_TYPE_MEM;
		rm->sreg = R_SS;

        //cnt[s.index != R_ESP]++;
		if (s.index != R_ESP) { addr = reg_l(s.index) << s.ss; } // likely
	    if (mod == 0) { // likely
		    if (s.base != R_EBP) {
		        rm->addr = addr + reg_l(s.base);
		        return 2;
		    }
	    } else {
    	    addr += reg_l(s.base);
	        if (mod == 1) { // likely
                rm->addr = addr + (int8_t) (eip_p1_data >> 8);
                return 3;
            }
	    }
	    rm->addr = addr + instr_fetch(eip + 2, 4);
	    return 6;
	} else {
        rm->sreg = R_DS;
	    if (mod == 0) {
		    if (base_reg == R_EBP) {
			    rm->addr = eip_p1_data;
	            return 5;
		    } else {
                rm->addr = reg_l(base_reg);
		        return 1;
	        }
	    } else if (mod == 1) {
	        rm->addr = reg_l(base_reg) + (int8_t) eip_p1_data;
	        return 2;
	    } else {
	        rm->addr = reg_l(base_reg) + eip_p1_data;
	        return 5;
	    }
	}
}
#endif

static inline int read_ModR_M_with_size(swaddr_t eip, Operand *rm, Operand *reg, int size) {
	ModR_M m;
	m.val = instr_fetch(eip, 1);
	reg->type = OP_TYPE_REG;
	reg->reg = m.reg;

	if (m.mod == 3) {
		rm->type = OP_TYPE_REG;
		rm->reg = m.R_M;
		
		switch(size) {
			case 1: rm->val = reg_b(m.R_M); break;
			case 2: rm->val = reg_w(m.R_M); break;
			case 4: rm->val = reg_l(m.R_M); break;
			default: assert(0);
		}
#ifdef DEBUG
		switch(size) {
			case 1: sprintf(rm->str, "%%%s", regsb[m.R_M]); break;
			case 2: sprintf(rm->str, "%%%s", regsw[m.R_M]); break;
			case 4: sprintf(rm->str, "%%%s", regsl[m.R_M]); break;
		}
#endif
		return 1;
	}
	else {
		int instr_len = load_addr(eip, &m, rm);
        //if (unlikely(cpu.seg_gs_prefix)) rm->sreg = R_GS;
		rm->val = swaddr_read(rm->addr, size, rm->sreg);
		return instr_len;
	}
}

int read_ModR_M(swaddr_t eip, Operand *rm, Operand *reg) {
    return read_ModR_M_with_size(eip, rm, reg, rm->size);
}

int read_ModR_M_b(swaddr_t eip, Operand *rm, Operand *reg) {
    return read_ModR_M_with_size(eip, rm, reg, 1);
}

int read_ModR_M_w(swaddr_t eip, Operand *rm, Operand *reg) {
    return read_ModR_M_with_size(eip, rm, reg, 2);
}

int read_ModR_M_l(swaddr_t eip, Operand *rm, Operand *reg) {
    return read_ModR_M_with_size(eip, rm, reg, 4);
}
