#include "cpu/exec/helper.h"
#include "cpu/decode/modrm.h"

#define MAKE_FLAGCTL_FUNC(NAME, PROGRAM) \
make_helper(NAME) { \
	print_asm(str(NAME)); \
	PROGRAM \
	return 1; \
}

MAKE_FLAGCTL_FUNC(cld, WRITEF(DF, 0); )
MAKE_FLAGCTL_FUNC(std, WRITEF(DF, 1); )

MAKE_FLAGCTL_FUNC(clc, WRITEF(CF, 0); )
MAKE_FLAGCTL_FUNC(stc, WRITEF(CF, 1); )
MAKE_FLAGCTL_FUNC(cmc, WRITEF(CF, !READF(CF)); )

MAKE_FLAGCTL_FUNC(cli, WRITEF(IF, 0); )
MAKE_FLAGCTL_FUNC(sti, WRITEF(IF, 1); )
