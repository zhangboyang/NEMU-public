#ifndef __FLOAT_H__
#define __FLOAT_H__

#include "trap.h"
#include <math.h>

typedef float FLOAT;

#define F2int(a) ((int)(a))
#define int2F(a) ((float)(a))
#define F_mul_int(a, b) ((a) * ((int)(b)))
#define F_div_int(a, b) ((a) / ((int)(b)))
#define f2F(a) (a)
#define F_mul_F(a, b) ((a) * (b))
#define F_div_F(a, b) ((a) / (b))
#define Fabs fabs

#endif
