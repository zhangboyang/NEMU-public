#include <setjmp.h>
#include "common.h"
#include "cpu/exec/helper.h"

extern jmp_buf jbuf;

extern void load_segment(int sreg_index, int selector);

#define GET_GATE_SELECTOR(desc) (((desc) >> 16) & 0xffff)
#define GET_GATE_OFFSET(desc) (((desc) & 0xffff) | (((desc) >> 32) & 0xffff0000))

void raise_intr(uint8_t NO) {
	/* Trigger an interrupt/exception with ``NO''.
	 * That is, use ``NO'' to index the IDT.
	 */

//    printf("interrupt 0x%02x\n", NO);
//	printf("current cs:eip = %04X:%08X\n", cpu.CS, cpu.EIP);
//    void print_registers(); print_registers();
    
	PUSH_DWORD(cpu.EFLAGS.EFLAGS);
	PUSH_DWORD((uint32_t) cpu.CS);
	PUSH_DWORD(cpu.EIP);
	
    // read gate desc    
    unsigned long long desc;
	uint32_t desc_data[2];
	desc_data[0] = lnaddr_read(cpu.IDTR + NO * 8, 4); // little endian
	desc_data[1] = lnaddr_read(cpu.IDTR + NO * 8 + 4, 4);
	memcpy(&desc, desc_data, 8);
	
	int next_cs = GET_GATE_SELECTOR(desc);
	int next_eip = GET_GATE_OFFSET(desc);
	

	
	//printf("target cs:eip = %04X:%08X\n", next_cs, next_eip);
	
	load_segment(R_CS, next_cs);
	cpu.EIP = next_eip;
	
	// 110 for interrupt gate, 111 for trap gate, 100 for task gate
	int type = desc >> (16 + 16 + 3 + 5) & 7;
	if (type == 6) {
	    cpu.EFLAGS.IF = 0; // should clear IF when going through interrupt gate
	}
	//printf("type=%d\n", type);
	
	
	/* Jump back to cpu_exec() */
	longjmp(jbuf, 1);
	panic("shouldn't reach here");
}

