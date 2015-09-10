#include "cpu/exec/helper.h"

make_helper(leave) {
    cpu.ESP = cpu.EBP;
    cpu.EBP = POP_DWORD();
    print_asm("leave");
    return 1;
}

