#include "nemu.h"
#include <stdlib.h>
#include <time.h>

CPU_state cpu;

CPU_state old_cpu;

const char *regsL[] = {"EAX", "ECX", "EDX", "EBX", "ESP", "EBP", "ESI", "EDI"};
const char *regsl[] = {"eax", "ecx", "edx", "ebx", "esp", "ebp", "esi", "edi"};
const char *regsw[] = {"ax" , "cx" , "dx" , "bx" , "sp" , "bp" , "si" , "di" };
const char *regsb[] = {"al" , "cl" , "dl" , "bl" , "ah" , "ch" , "dh" , "bh" };
const char* seg_regs[] = {"es", "cs", "ss", "ds", "fs", "gs"}; // R_ES, R_CS, R_SS, R_DS, R_FS, R_GS 

void reg_test() {
#ifndef DEBUG
    return;
#endif
	srand(time(0));
	uint32_t sample[8];
	uint32_t eip_sample = rand();
	cpu.eip = eip_sample;

	int i, j;
	for (i = R_EAX; i <= R_EDI; i ++) {
		sample[i] = rand();
		reg_l(i) = sample[i];
		assert(reg_w(i) == (sample[i] & 0xffff));
	}

	assert(reg_b(R_AL) == (sample[R_EAX] & 0xff));
	assert(reg_b(R_AH) == ((sample[R_EAX] >> 8) & 0xff));
	assert(reg_b(R_BL) == (sample[R_EBX] & 0xff));
	assert(reg_b(R_BH) == ((sample[R_EBX] >> 8) & 0xff));
	assert(reg_b(R_CL) == (sample[R_ECX] & 0xff));
	assert(reg_b(R_CH) == ((sample[R_ECX] >> 8) & 0xff));
	assert(reg_b(R_DL) == (sample[R_EDX] & 0xff));
	assert(reg_b(R_DH) == ((sample[R_EDX] >> 8) & 0xff));
	
	assert(cpu.AL == (sample[R_EAX] & 0xff));
	assert(cpu.AH == ((sample[R_EAX] >> 8) & 0xff));
	assert(cpu.BL == (sample[R_EBX] & 0xff));
	assert(cpu.BH == ((sample[R_EBX] >> 8) & 0xff));
	assert(cpu.CL == (sample[R_ECX] & 0xff));
	assert(cpu.CH == ((sample[R_ECX] >> 8) & 0xff));
	assert(cpu.DL == (sample[R_EDX] & 0xff));
	assert(cpu.DH == ((sample[R_EDX] >> 8) & 0xff));

	assert(sample[R_EAX] == cpu.eax);
	assert(sample[R_ECX] == cpu.ecx);
	assert(sample[R_EDX] == cpu.edx);
	assert(sample[R_EBX] == cpu.ebx);
	assert(sample[R_ESP] == cpu.esp);
	assert(sample[R_EBP] == cpu.ebp);
	assert(sample[R_ESI] == cpu.esi);
	assert(sample[R_EDI] == cpu.edi);
	
	assert(reg_w(R_AX) == cpu.AX);
	assert(reg_w(R_CX) == cpu.CX);
	assert(reg_w(R_DX) == cpu.DX);
	assert(reg_w(R_BX) == cpu.BX);
	assert(reg_w(R_SP) == cpu.SP);
	assert(reg_w(R_BP) == cpu.BP);
	assert(reg_w(R_SI) == cpu.SI);
	assert(reg_w(R_DI) == cpu.DI);
	
	assert(offsetof(CPU_state, eax) == 0);
	assert(offsetof(CPU_state, ecx) == 4);
	assert(offsetof(CPU_state, edx) == 8);
	assert(offsetof(CPU_state, ebx) == 12);
	assert(offsetof(CPU_state, esp) == 16);
	assert(offsetof(CPU_state, ebp) == 20);
	assert(offsetof(CPU_state, esi) == 24);
	assert(offsetof(CPU_state, edi) == 28);
	
	for (i = 0; i < 100; i++) { // loop 100 times for sure
        cpu.EFLAGS.EFLAGS = rand();
        assert(cpu.EFLAGS.CF == ((cpu.EFLAGS.EFLAGS >> 0) & 1));
        assert(cpu.EFLAGS.PF == ((cpu.EFLAGS.EFLAGS >> 2) & 1));
        assert(cpu.EFLAGS.ZF == ((cpu.EFLAGS.EFLAGS >> 6) & 1));
        assert(cpu.EFLAGS.SF == ((cpu.EFLAGS.EFLAGS >> 7) & 1));
        assert(cpu.EFLAGS.IF == ((cpu.EFLAGS.EFLAGS >> 9) & 1));
        assert(cpu.EFLAGS.DF == ((cpu.EFLAGS.EFLAGS >> 10) & 1));
        assert(cpu.EFLAGS.OF == ((cpu.EFLAGS.EFLAGS >> 11) & 1));
        
        for (j = 0; j < 4; j++)
            cpu.CR[j] = rand();
        assert(cpu.CR[0] == cpu.CR0);
        assert(cpu.CR[1] == cpu.CR1);
        assert(cpu.CR[2] == cpu.CR2);
        assert(cpu.CR[3] == cpu.CR3);
        
        assert(cpu.PE == !!(cpu.CR0 & 1));
        assert(cpu.MP == !!(cpu.CR0 & 2));
        assert(cpu.EM == !!(cpu.CR0 & 4));
        assert(cpu.TS == !!(cpu.CR0 & 8));
        assert(cpu.ET == !!(cpu.CR0 & 16));
        assert(cpu.PG == !!(cpu.CR0 & (1 << 31)));
        
        for (j = 0; j < 6; j++)
            cpu.seg_reg[j] = rand();
        assert(cpu.seg_reg[0] == cpu.ES);
        assert(cpu.seg_reg[1] == cpu.CS);
        assert(cpu.seg_reg[2] == cpu.SS);
        assert(cpu.seg_reg[3] == cpu.DS);
        assert(cpu.seg_reg[4] == cpu.FS);
        assert(cpu.seg_reg[5] == cpu.GS);
    }
    
	assert(eip_sample == cpu.eip);
}

