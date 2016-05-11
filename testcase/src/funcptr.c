#include "trap.h"

// function pointer test


typedef int (*myfp1_t)(int a, int b);

int myadd(int a, int b)
{
    return a + b;
}

typedef void (*myfp2_t)();

void hgt()
{
    HIT_GOOD_TRAP;
}

int main()
{
	volatile myfp1_t fp1 = myadd;
	volatile myfp2_t fp2 = hgt;
	
	volatile int a = 10000100;
	volatile int b = 1234;
	volatile int c = 55555678;
	volatile int d = 10001334;
	
	c = fp1(a, b);
	
	nemu_assert(c == d);
	
	fp2();
	
	HIT_BAD_TRAP;

	return 0;
}



