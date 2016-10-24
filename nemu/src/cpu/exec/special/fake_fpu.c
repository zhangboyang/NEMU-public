#include "cpu/exec/helper.h"

make_helper(fake_fpu) {
    
    uint8_t instr = instr_fetch(cpu.EIP, 1);
    
    
    static int fldz_flag = 0;
    static int fstpl_flag = 0;

    int len = 0;
    switch (instr) {
        case 0xd9:
            print_asm("fldz (ignored)");
            if (!fldz_flag) {
                printf(" %sfldz at %04X:%08X, ignored%s\n", c_red c_bold, cpu.CS, cpu.EIP, c_normal);
                fldz_flag = 1;
            }
            len = 2;
            break;
        case 0xdd:
            print_asm("fstpl  -0xf0(%%ebp) (ignored)");
            if (!fstpl_flag) {
                printf(" %sfstpl at %04X:%08X, ignored%s\n", c_red c_bold, cpu.CS, cpu.EIP, c_normal);
                fstpl_flag = 1;
            }
            len = 6;
            break;
        default:
            panic_to_ui("floating point instruction is detected!");
    }

	return len;
}
