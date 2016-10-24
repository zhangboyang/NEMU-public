#include "cpu/exec/helper.h"

#define DATA_BYTE 1
#include "cmpxchg-template.h"
#undef DATA_BYTE

#define DATA_BYTE 2
#include "cmpxchg-template.h"
#undef DATA_BYTE

#define DATA_BYTE 4
#include "cmpxchg-template.h"
#undef DATA_BYTE


/* for instruction encoding overloading */

make_helper_v(cmpxchg_r2rm)
