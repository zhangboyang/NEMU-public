#include "trap.h"

.globl start
start:
    
    mov $1, %ecx
    jecxz bad
    jcxz bad
    mov $0, %ecx
    jecxz 1f
    int3
1:
    jcxz 1f
    int3
1:
    mov $0x12340000, %ecx
    jecxz bad
    jcxz 1f
    int3
1:
    HIT_GOOD_TRAP
bad:
    HIT_BAD_TRAP
