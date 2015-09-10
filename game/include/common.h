#ifndef __COMMON_H__
#define __COMMON_H__

#include <stdio.h>
#include <stdint.h>

#include "trap.h"
#include "debug.h"

typedef uint8_t bool;

#define true         1
#define false        0


// ZBY's timing feature
#define USE_ZBY_TIMING

#ifdef USE_ZBY_TIMING
#define ZBY_TIMING_BEGIN(ID) do { __asm__ __volatile__ (".byte 0xd5"::"a"(300), "d"(ID)); } while (0)
#define ZBY_TIMING_END(ID, STR) do { __asm__ __volatile__ (".byte 0xd5"::"a"(301), "c"(STR), "d"(ID)); } while (0)
#else
#define ZBY_TIMING_BEGIN(ID) do { } while (0)
#define ZBY_TIMING_END(ID, STR) do { } while (0)
#endif



#endif
