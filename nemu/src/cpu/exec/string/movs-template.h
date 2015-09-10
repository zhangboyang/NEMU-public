#include "cpu/exec/template-start.h"

#define instr movs

make_helper(concat3(instr, _, SUFFIX))
{
    ADDR16_PANIC(str(instr));
    MEM_W(R_ES, cpu.EDI, MEM_R(R_DS, cpu.ESI));
    cpu.EDI += DF_DATA_BYTE;
    cpu.ESI += DF_DATA_BYTE;
    
    return 1;
}



#include "cpu/exec/template-end.h"
