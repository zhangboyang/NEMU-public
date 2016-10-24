#include "cpu/exec/helper.h"

#define DATA_BYTE 2
#include "bsf-template.h"
#undef DATA_BYTE

#define DATA_BYTE 4
#include "bsf-template.h"
#undef DATA_BYTE

/* for instruction encoding overloading */

make_helper_v(bsf_rm2r)
