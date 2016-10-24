#include "cpu/exec/template-start.h"

#define instr scas

make_helper(concat3(instr, _, SUFFIX))
{
    ADDR16_PANIC(str(instr));
    EFLAGS((cpu.EAX & MASK), MEM_R(R_ES, cpu.EDI), 1);
    cpu.EDI += DF_DATA_BYTE;
   
    return 1;
}



#include "cpu/exec/template-end.h"
