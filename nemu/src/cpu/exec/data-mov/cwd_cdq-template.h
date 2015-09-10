#include "cpu/exec/template-start.h"

#define instr cwd_cdq

#if DATA_BYTE == 2 || DATA_BYTE == 4
make_helper(concat3(instr, _, SUFFIX))
{
#if DATA_BYTE == 2
    short AX = cpu.AX;
    cpu.DX = AX < 0 ? 0xffff : 0;
    print_asm("cwd");
#elif DATA_BYTE == 4
	int EAX = cpu.EAX;
    cpu.EDX = EAX < 0 ? 0xffffffff : 0;
    print_asm("cdq");
#else
#error unknown DATA_BYTE
#endif
    return 1;
}

#endif


#include "cpu/exec/template-end.h"
