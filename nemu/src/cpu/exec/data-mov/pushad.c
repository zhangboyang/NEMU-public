#include "cpu/exec/helper.h"

make_helper(pushad) {

    assert(ops_decoded.is_data_size_16 == 0);
    
    uint32_t old_esp = cpu.ESP;
    
    PUSH_DWORD(cpu.EAX);
    PUSH_DWORD(cpu.ECX);
    PUSH_DWORD(cpu.EDX);
    PUSH_DWORD(cpu.EBX);
    PUSH_DWORD(old_esp);
    PUSH_DWORD(cpu.EBP);
    PUSH_DWORD(cpu.ESI);
    PUSH_DWORD(cpu.EDI);
    
	return 1;
}
