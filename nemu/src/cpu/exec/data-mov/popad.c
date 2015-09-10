#include "cpu/exec/helper.h"
make_helper(popad) {

    assert(ops_decoded.is_data_size_16 == 0);
    
    cpu.EDI = POP_DWORD();
    cpu.ESI = POP_DWORD();
    cpu.EBP = POP_DWORD();
    cpu.ESP += 4;
    cpu.EBX = POP_DWORD();
    cpu.EDX = POP_DWORD();
    cpu.ECX = POP_DWORD();
    cpu.EAX = POP_DWORD();
    
	return 1;
}
