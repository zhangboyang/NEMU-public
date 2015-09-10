#include "cpu/exec/helper.h"


make_helper(ret) {
    cpu.EIP = POP_DWORD(); cpu.protect_eip = 1;
    print_asm("ret");
    return 1;
}

make_helper(ret_i_w) {
    cpu.EIP = POP_DWORD(); cpu.protect_eip = 1;
    
    DATA16_PANIC("ret");
    
    int32_t imm = (int32_t) instr_fetch(eip + 1, 2); // sign extend
    cpu.ESP = cpu.ESP + ((uint32_t) imm);
    print_asm("ret $0x%x", (uint32_t) imm);
    return 3;
}

