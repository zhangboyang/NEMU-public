#include "trap.h"

// function pointer test

typedef void (*myfp_t)();

int cnt = 0;

void donothing()
{
}

void addcnt()
{
    cnt++;
}

int main()
{
	volatile myfp_t fp[1000] = {};
	
	fp[3] = addcnt;
	fp[499] = addcnt;
	fp[977] = addcnt;
	fp[976] = donothing;
	
	__asm__ __volatile__ ("call *%0"::"m"(fp[3]));
	__asm__ __volatile__ ("call *%0"::"m"(fp[976]));
	__asm__ __volatile__ ("call *%0"::"m"(fp[499]));
	__asm__ __volatile__ ("call *%0"::"m"(fp[977]));
	
	
	fp[3]();
	fp[499]();
	fp[977]();
	fp[976]();
	
	nemu_assert(cnt == 6);
	
	HIT_GOOD_TRAP;

	return 0;
}

