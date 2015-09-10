#ifndef __COMMON_H__
#define __COMMON_H__

//#define USE_RAMDISK

/* You will define this macro in PA4 */
#define HAS_DEVICE


/* DEBUG OPTION, NOTE: VERY SLOW */
//#define DEBUG


/* for assert() */
#ifndef DEBUG
    #define NDEBUG
#endif


//#define LOG_FILE

#include "debug.h"
#include "macro.h"

#include <stdint.h>
#include <assert.h>
#include <string.h>

typedef uint8_t bool;
typedef uint32_t hwaddr_t;
typedef uint32_t lnaddr_t;
typedef uint32_t swaddr_t;

typedef uint16_t ioaddr_t;

#pragma pack (1)
typedef union {
	uint32_t _4;
	uint32_t _3	: 24;
	uint16_t _2;
	uint8_t _1;
} unalign;
#pragma pack ()

#define false 0
#define true 1

/* added by ZBY */

/* ZBY: the timer interrupt freq */
#define TIMER_HZ 100

/* ZBY: uncomment to disable invalid flag read check */
#ifdef DEBUG
    #define INVF_CHECK
#endif

/* ZBY: uncomment to disable cpu_state difference display */
#ifdef DEBUG
    #define REG_DIFF_HIGHLIGHT
#endif

/* ZBY: ui feature */
#ifdef DEBUG
    #define UI_FEATURE_WATCHPOINT
    #define UI_FEATURE_NEXT_INSTRUCTION
    #define UI_FEATURE_PERF
#endif

/* ZBY: faster DRAM */
#ifndef DEBUG
    #define USE_VERY_FAST_MEMORY
#endif

/* ZBY: use tlb when page-translate */
#ifdef DEBUG
    #define USE_TLB
#endif

/* ZBY: use memory cache */
#ifdef DEBUG
    #define USE_MEMORY_CACHE
#endif

/* ZBY: uncomment to use function-based eflags, useful when using perf */
//#define USE_EFLAGS_FUNC

/* ZBY: uncomment to use inline-asm-based eflags instead of macro */
#define USE_ASM_EFLAGS

// from gcc manual
#define offsetof(type, member)  __builtin_offsetof (type, member)


//#define DISABLE_COLOR
#ifndef DISABLE_COLOR
// color escape code
#define c_normal         "\x1b[0m"
#define c_bold           "\x1b[1m"
#define c_red            "\x1b[31m"
#define c_green          "\x1b[32m"
#define c_yellow         "\x1b[33m"
#define c_blue           "\x1b[34m"
#define c_purple         "\x1b[35m"
#define c_cyan           "\x1b[36m"
#else
#define c_normal         ""
#define c_bold           ""
#define c_red            ""
#define c_green          ""
#define c_yellow         ""
#define c_blue           ""
#define c_purple         ""
#define c_cyan           ""
#endif
#endif

/* likely and unlikely */
#define likely(x)       __builtin_expect((x),1)
#define unlikely(x)     __builtin_expect((x),0)

/* use a var, keep gcc happy and not generate warnings */
#define FORCE_USE(x) ((void) (x))

