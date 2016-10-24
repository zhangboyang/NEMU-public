#include "cpu/exec/helper.h"

#define DATA_BYTE 1
#include "bt-template.h"
#undef DATA_BYTE

#define DATA_BYTE 2
#include "bt-template.h"
#undef DATA_BYTE

#define DATA_BYTE 4
#include "bt-template.h"
#undef DATA_BYTE

/* for instruction encoding overloading */

make_helper_v(bt_rm_imm)
make_helper_v(bt_r2rm)
