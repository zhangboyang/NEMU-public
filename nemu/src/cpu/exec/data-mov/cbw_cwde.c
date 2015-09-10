#include "cpu/exec/helper.h"

#define DATA_BYTE 2
#include "cbw_cwde-template.h"
#undef DATA_BYTE

#define DATA_BYTE 4
#include "cbw_cwde-template.h"
#undef DATA_BYTE

make_helper_v(cbw_cwde)
