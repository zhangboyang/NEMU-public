#include "FLOAT.h"


// zby is too lazy to do this in bit-level since we can use limited-operation inside VM
// inorder to write this outside VM, zby used inline-asm and some trick like NEMU TRAP

#define NULL ((void *)0)

#define ZBY_INSTR ".byte 0xd5\n\t"

#define CALL_ZBY_FUNC2(FUNC_ID, ARG1, ARG2, RET) \
	__asm__ __volatile("movl %1, %%eax\n\t" \
	                   "movl %2, %%ecx\n\t" \
	                   "movl %3, %%edx\n\t" \
	                   ZBY_INSTR \
	                   "movl %%eax, %0\n\t" \
	                   :"=m"(RET) \
	                   :"i"(FUNC_ID), "m"(ARG1), "m"(ARG2) \
	                   :"eax", "ecx", "edx")

#define CALL_ZBY_FUNC1(FUNC_ID, ARG1, RET) \
	__asm__ __volatile("movl %1, %%eax\n\t" \
	                   "movl %2, %%ecx\n\t" \
	                   ZBY_INSTR \
	                   "movl %%eax, %0\n\t" \
	                   :"=m"(RET) \
	                   :"i"(FUNC_ID), "m"(ARG1) \
	                   :"eax", "ecx")



typedef unsigned long long ULL;
typedef long long LL;

static void ULLdivULL(ULL a, ULL b, ULL *qp, ULL *rp)
{
    ULL r = 0, q = 0;
    int i;
    for (i = 0; i < 64; i++) {
        r = (r << 1) + (a >> 63);
        a <<= 1;
        q <<= 1;
        if (r >= b) {
            r -= b;
            q += 1;
        }
    }
    if (qp) *qp = q;
    if (rp) *rp = r;
}

static void LLdivLL(LL a, LL b, LL *qp, LL *rp)
{
    int qf = 0, rf = 0;
    if (a < 0) { qf = rf = 1; a = -a; }
    if (b < 0) { qf ^= 1; b = -b; }

    ULLdivULL(a, b, (ULL *) qp, (ULL *) rp);

    if (qp && qf) *qp = -*qp;
    if (rp && rf) *rp = -*rp;
}

FLOAT F_mul_F(FLOAT a, FLOAT b) {
    /*volatile FLOAT ret;
    CALL_ZBY_FUNC2(0, a, b, ret);
	return ret;*/
	long long la = a, lb = b;
    return (la * lb) >> 16;
}

FLOAT F_div_F(FLOAT a, FLOAT b) {
    /*volatile FLOAT ret;
    CALL_ZBY_FUNC2(1, a, b, ret);
	return ret;*/
	long long la = a, lb = b, ret;
    la <<= 16; // must be LL shift
    LLdivLL(la, lb, &ret, NULL);
    return ret;
}

FLOAT f2F(float a) {
    volatile FLOAT ret;
    CALL_ZBY_FUNC1(2, *(int *)&a, ret);
	return ret;
}

FLOAT Fabs(FLOAT a) { // abs is easy!
    int ret = a;
	return ret >= 0 ? ret : -ret;
}

FLOAT sqrt(FLOAT x) {
	FLOAT dt, t = int2F(2);

	do {
		dt = F_div_int((F_div_F(x, t) - t), 2);
		t += dt;
	} while(Fabs(dt) > f2F(1e-4));

	return t;
}

FLOAT pow(FLOAT x, FLOAT y) {
	/* we only compute x^0.333 */
	FLOAT t2, dt, t = int2F(2);

	do {
		t2 = F_mul_F(t, t);
		dt = (F_div_F(x, t2) - t) / 3;
		t += dt;
	} while(Fabs(dt) > f2F(1e-4));

	return t;
}

