#include <stdio.h>

#include "include/cpu/reg.h"
CPU_state cpu;

#define DATA_BYTE 1
#include "include/cpu/exec/template-start.h"
#define haha DATA_BYTE
unsigned eflags1(unsigned a, unsigned b, unsigned c, unsigned d) { return EFLAGS_ALU(a, b, c, d); }
#include "include/cpu/exec/template-end.h"
#undef DATA_BYTE
#define DATA_BYTE 2
#include "include/cpu/exec/template-start.h"
#define haha DATA_BYTE
unsigned eflags2(unsigned a, unsigned b, unsigned c, unsigned d) { return EFLAGS_ALU(a, b, c, d); }
#include "include/cpu/exec/template-end.h"
#undef DATA_BYTE
#define DATA_BYTE 4
#include "include/cpu/exec/template-start.h"
#define haha DATA_BYTE
unsigned eflags4(unsigned a, unsigned b, unsigned c, unsigned d) { return EFLAGS_ALU(a, b, c, d); }
#include "include/cpu/exec/template-end.h"
#undef DATA_BYTE



int main()
{
    unsigned (*eflags)(unsigned, unsigned, unsigned, unsigned);
    int *cnt;
    int cnt1 = 0, cnt2 = 0, cnt4 = 0;
    int sz;
    
    int a, b, f, rightf, r, rightr;
    int rmask, fmask;
    
    char buf[100];
    char *instr;
    
    while (scanf("%s%d%x%x%x%x%x", buf, &sz, &a, &b, &rightr, &rightf, &fmask) == 7) {
    
        instr = buf;
        
        if (strncmp(instr, "stc", 3) == 0) {
            cpu.CF = 1;
            instr += 3;
        } else if (strncmp(instr, "clc", 3) == 0) {
            cpu.CF = 0;
            instr += 3;
        }
        
        switch (sz) {
            case 1: eflags = eflags1; cnt = &cnt1; break;
            case 2: eflags = eflags2; cnt = &cnt2; break;
            case 4: eflags = eflags4; cnt = &cnt4; break;
            default: sz = 0; eflags = NULL; cnt = NULL;
        }
        
        if (!sz) { puts("ERROR: unknown instruction size"); return 1; }
        
        if (strcmp(instr, "add") == 0) r = eflags(a, b, 0, 0);
        else if (strcmp(instr, "adc") == 0) r = eflags(a, b, 0, cpu.CF);
        else if (strcmp(instr, "sub") == 0) r = eflags(a, b, 1, 0);
        else if (strcmp(instr, "sbb") == 0) r = eflags(a, b, 1, cpu.CF);
        else { printf("ERROR: unknown instruction %s\n", buf); return 1; }
        
        rmask = ((1 << (sz * 8 - 1)) << 1) - 1;
        rightr = rightr & rmask;
        f = cpu.EFLAGS & fmask;
        if (f != rightf || r != rightr) {
            printf("ERROR: type=%s sz=%d a=%x b=%x r=%x rightr=%x f=%x rightf=%x\n", buf, sz, a, b, r, rightr, f, rightf);
            return 1;
        }
        
        (*cnt)++;
    }
    
    printf("1 byte: %d tests\n", cnt1);
    printf("2 byte: %d tests\n", cnt2);
    printf("4 byte: %d tests\n", cnt4);
    
    return 0;
}
