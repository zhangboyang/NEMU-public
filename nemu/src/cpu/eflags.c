#include "common.h"

#ifdef USE_EFLAGS_FUNC
#include "cpu/reg.h"

#define DATA_BYTE 1
#include "eflags-template.h"
#undef DATA_BYTE

#define DATA_BYTE 2
#include "eflags-template.h"
#undef DATA_BYTE

#define DATA_BYTE 4
#include "eflags-template.h"
#undef DATA_BYTE

#endif
