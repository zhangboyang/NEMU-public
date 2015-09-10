#include <stdio.h>
#include <stdlib.h>

/* OF, SF, ZF, PF, CF */
#define ALU_EFLAGS_MASK 0x8c5

/* execute instr and get EFLAGS and EAX */
#define MAKE_FUNC_GET(NAME, INSTR) \
int get_ ## NAME (int a, int *r) \
{ \
	volatile int f, tr; \
	__asm__ __volatile__ ("mov %2, %%eax\n\t" \
	                       INSTR \
	                      "pushf\n\t" \
	                      "clc\n\t"\
	                      "pop %%edx\n\t" \
	                      "mov %%eax, %0\n\t" \
	                      "mov %%edx, %1\n\t" \
	                      :"=m"(tr), "=m"(f) \
	                      :"m"(a) \
	                      :"eax", "ecx", "edx"); \
	*r = tr; \
	return f; \
}

MAKE_FUNC_GET(add1, "mov $1, %%ecx\n\t"
                    "sub %%ecx, %%eax\n\t")

MAKE_FUNC_GET(inc, "dec %%eax\n\t")

int main()
{
    int a = 0;
    int f1, f2, r1, r2;
    
    do {
        f1 = get_add1(a, &r1) & ALU_EFLAGS_MASK;
        f2 = get_inc(a, &r2) & ALU_EFLAGS_MASK;
        
        if ((a & 0xffffff) == 0)
            printf("a=%x f1=%x r1=%x f2=%x r2=%x\n", a, f1, r1, f2, r2);
        
        if (f1 != f2 || r1 != r2) {
            printf("a=%x f1=%x r1=%x f2=%x r2=%x\n", a, f1, r1, f2, r2);
            puts("ERROR");
//            return 0;
        }
        
        a++;
    } while (a != 0);
    
    puts("TEST FINISHED");
	return 0;
}
