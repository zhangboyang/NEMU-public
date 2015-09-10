#include "trap.h"

.globl start
start:

    // you need use 'info r' command to test this testcase
    cld
    std //DF=1
    cld //DF=0

    clc
    stc //CF=0 
    clc //CF=1

    clc //CF=0
    cmc //CF=1

    stc //CF=1
    cmc //CF=0

    HIT_GOOD_TRAP
