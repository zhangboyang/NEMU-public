#include "cpu/exec/helper.h"
#include "device/port-io.h"

make_helper(fake_fpu) {
    
    uint8_t instr = instr_fetch(cpu.EIP, 1);
    
    int len = 0;
    switch (instr) {
        case 0xd9:
            print_asm("fldz (ignored)");
            //printf("fldz at %04X:%08X, ignored\n", cpu.CS, cpu.EIP);
            len = 2;
            break;
        case 0xdd:
            print_asm("fstpl  -0xf0(%%ebp) (ignored)");
            //printf("fstpl at %04X:%08X, ignored\n", cpu.CS, cpu.EIP);
            len = 6;
            break;
        default:
            assert(0);
    }

	return len;
}
