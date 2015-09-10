#include "trap.h"


void test_inc(int x)
{
    volatile int a, b;
    a = b = x;
    __asm__ __volatile__ ("incl %1\n\t":"+m"(a));
    __asm__ __volatile__ ("addl $1, %1\n\t":"+m"(b));
    nemu_assert(a == b);
    nemu_assert(a == x + 1);
    
    a = b = x;
    __asm__ __volatile__ ("incl %1\n\t":"+a"(a));
    __asm__ __volatile__ ("addl $1, %1\n\t":"+a"(b));
    nemu_assert(a == b);
    nemu_assert(a == x + 1);
}

void test_dec(int x)
{
    volatile int a, b;
    a = b = x;
    __asm__ __volatile__ ("decl %1\n\t":"+m"(a));
    __asm__ __volatile__ ("subl $1, %1\n\t":"+m"(b));
    nemu_assert(a == b);
    nemu_assert(a == x - 1);
    
    a = b = x;
    __asm__ __volatile__ ("decl %1\n\t":"+a"(a));
    __asm__ __volatile__ ("subl $1, %1\n\t":"+a"(b));
    nemu_assert(a == b);
    nemu_assert(a == x - 1);
}


int data[] = {
    0, 1, -1, 2, -2, 0xaabbccdd, 0xffeeddcc
};
int datalen = sizeof(data) / sizeof(data[0]);

int main()
{	
	int i;
	for (i = 0; i < datalen; i++) {
	    test_inc(data[i]);
	    test_dec(data[i]);
	}

	HIT_GOOD_TRAP;
	return 0;
}
