#include "trap.h"

#define CBW "cbtw\n\t"
#define CWDE "cwtl\n\t"

#define MAKE_EAX_EDX_ASM(INSTR) \
    __asm__ __volatile__ ("mov %1, %%eax\n\t" \
                          INSTR \
                          "mov %%eax, %0\n\t" \
                          :"=m"(newa) \
                          :"m"(a) \
                          :"eax")

void test_cbw(int x)
{
    volatile int a, newa;
    
    a = x;
    
    MAKE_EAX_EDX_ASM(CBW);
    
    nemu_assert((short) newa == (short)(signed char) a);
}

void test_cwde(int x)
{
    volatile int a, newa;
    
    a = x;
    
    MAKE_EAX_EDX_ASM(CWDE);
    
    nemu_assert(newa == (int)(short) a);
}


int data[] = {
    0, 1, -1, 2, -2, 0x1111ffff, 0xffff1111, 0x11223344, 0x7777777, 0xffff, 0x7fff, 0xff11, 0x11ff
};
int datalen = sizeof(data) / sizeof(data[0]);

int main()
{	
	int i;
	for (i = 0; i < datalen; i++) {
	    test_cbw(data[i]);
	    test_cwde(data[i]);
	}

	HIT_GOOD_TRAP;
	return 0;
}
