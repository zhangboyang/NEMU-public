#include <stdio.h>
#include <stdlib.h>

#define NUL ""

/* clear CF */
#define CLC "clc\n\t"

/* set CF */
#define STC "stc\n\t"

/* OF, SF, ZF, PF, CF */
#define ALU_EFLAGS_MASK 0x8c5

int my_rand()
{
	int a = 0;
	a = (a << 8) + (rand() & 0xff);
	a = (a << 8) + (rand() & 0xff);
	a = (a << 8) + (rand() & 0xff);
	a = (a << 8) + (rand() & 0xff);
	return a;
}

/* execute instr and get EFLAGS and EAX */
#define MAKE_FUNC_GET(NAME, INSTR) \
int get_ ## NAME (int a, int b, int *r) \
{ \
	volatile int f, tr; \
	__asm__ __volatile__ ("mov %2, %%eax\n\t" \
	                      "mov %3, %%ecx\n\t" \
	                       INSTR \
	                      "pushf\n\t" \
	                      "pop %%edx\n\t" \
	                      "mov %%eax, %0\n\t" \
	                      "mov %%edx, %1\n\t" \
	                      :"=m"(tr), "=m"(f) \
	                      :"m"(a), "m"(b) \
	                      :"eax", "ecx", "edx"); \
	*r = tr; \
	return f; \
}

#define MAKE_FUNC_PRINT(NAME, DESC) \
void print_ ## NAME (int a, int b, int mask) \
{ \
    int f, r; \
    f = get_ ## NAME (a, b, &r); \
    f &= mask; \
    printf(DESC " %08x %08x %08x %08x %08x\n", a, b, r, f, mask); \
}

#define MAKE_FUNC(DESC, NAME, INSTR) MAKE_FUNC_GET(NAME, INSTR) MAKE_FUNC_PRINT(NAME, DESC)



int testdata[] = {
    0, 0x7fffffff,
};
int testdata_len = sizeof(testdata) / sizeof(testdata[0]);


/* N - full random data
   M - half random data
   F - brute force range
   D - max diff with testdata[]
*/

void run_func(void (*print)(int, int, int), int mask, int maxn, int maxm, int maxf, int maxd)
{
    int i, j, di, dj;
    int a, b;
    int cnt;
    
    /* generate by a, b=testdata[]+diff; cnt = (2D+1)(2D+1) */
    for (i = 0; i < testdata_len; i++)
        for (j = 0; j < testdata_len; j++)
            for (di = -maxd; di <= maxd; di++)
                for (dj = -maxd; dj <= maxd; dj++) {
                    a = testdata[i] + di;
                    b = testdata[j] + dj;
                    print(a, b, mask);
                }
    
    /* generate by brute force; cnt=F*F */
    for (a = 0; a <= maxf; a++)
        for (b = 0; b <= maxf; b++)
            print(a, b, mask);
    
    /* generate by a=testdata[]+diff and b=rand() with swap; cnt=M(2D+1) */
    for (i = 0; i < testdata_len; i++)
        for (di = -maxd; di <= maxd; di++) {
            a = testdata[i] + di;
            for (j = 0; j < maxm; j++) {
                b = my_rand();
                print(a, b, mask);
                print(b, a, mask);
            }
        }
    
    /* generate by a+b=testdata[]+diff with random and swap; cnt = M(2D+1)*/
    for (cnt = 0; cnt < maxm; cnt++)
        for (i = 0; i < testdata_len; i++)
            for (di = -maxd; di <= maxd; di++) {
                a = my_rand();
                b = testdata[i] + di - a;
                print(a, b, mask);
                print(b, a, mask);
            }
    
    /* generate by full random and swap; cnt=N */
    for (cnt = 0; cnt < maxn; cnt++) {
        a = my_rand();
        b = my_rand();
        print(a, b, mask);
        print(b, a, mask);
    }
}



#define RUN_FUNC(NAME, MASK, N, M, F, D) run_func(print_ ## NAME, (MASK), (N), (M), (F), (D))

#define MAKE_FUNC_SERIES(NAME, PREPARE, INSTR) \
    MAKE_FUNC(#NAME " 4", NAME ## 4, PREPARE INSTR " %%ecx, %%eax\n\t") \
    MAKE_FUNC(#NAME " 2", NAME ## 2, PREPARE INSTR " %%cx, %%ax\n\t") \
    MAKE_FUNC(#NAME " 1", NAME ## 1, PREPARE INSTR " %%cl, %%al\n\t")

#define RUN_FUNC_SERIES(NAME, MASK, N, M, F, D) ({ \
    RUN_FUNC(NAME ## 4, (MASK), (N), (M), (F), (D)); \
    RUN_FUNC(NAME ## 2, (MASK), (N), (M), (F), (D)); \
    RUN_FUNC(NAME ## 1, (MASK), (N), (M), (F), (D));}) \




MAKE_FUNC_SERIES(add, NUL, "add")
MAKE_FUNC_SERIES(stcadc, STC, "adc")
MAKE_FUNC_SERIES(clcadc, CLC, "adc")
MAKE_FUNC_SERIES(sub, NUL, "sub")
MAKE_FUNC_SERIES(stcsbb, STC, "sbb")
MAKE_FUNC_SERIES(clcsbb, CLC, "sbb")

int main()
{
    int N, M, F, D;
    N = 20000;
    M = 2000;
    F = 0xff;
    D = 10;
    
    RUN_FUNC_SERIES(add, ALU_EFLAGS_MASK, N, M, F, D);
    RUN_FUNC_SERIES(stcadc, ALU_EFLAGS_MASK, N, M, F, D);
    RUN_FUNC_SERIES(clcadc, ALU_EFLAGS_MASK, N, M, F, D);
    
    RUN_FUNC_SERIES(sub, ALU_EFLAGS_MASK, N, M, F, D);
    RUN_FUNC_SERIES(stcsbb, ALU_EFLAGS_MASK, N, M, F, D);
    RUN_FUNC_SERIES(clcsbb, ALU_EFLAGS_MASK, N, M, F, D);
    
	return 0;
}
