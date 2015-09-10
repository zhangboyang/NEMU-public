#ifndef __MEMORY_H__
#define __MEMORY_H__

#include "common.h"

#include "x86-inc/mmu.h"

#define HW_MEM_SIZE (128 * 1024 * 1024)

extern uint8_t *hw_mem;

/* convert the hardware address in the test program to virtual address in NEMU */
#define hwa_to_va(p) ((void *)(hw_mem + (unsigned)p))
/* convert the virtual address in NEMU to hardware address in the test program */
#define va_to_hwa(p) ((hwaddr_t)((void *)p - (void *)hw_mem))

#define hw_rw(addr, type) *(type *)({\
	Assert(addr < HW_MEM_SIZE, "physical address(0x%08x) is out of bound", addr); \
	hwa_to_va(addr); \
})

uint32_t lnaddr_read(lnaddr_t, size_t);
uint32_t hwaddr_read(hwaddr_t, size_t);
uint32_t hwaddr_read_nocache(hwaddr_t, size_t);
uint32_t safe_swaddr_read(swaddr_t, size_t, int *, uint8_t);
void lnaddr_write(lnaddr_t, size_t, uint32_t);
void hwaddr_write(hwaddr_t, size_t, uint32_t);
void hwaddr_write_nocache(hwaddr_t, size_t, uint32_t);


// useful macros for segment-translation
#define GET_SEG_LIMIT(x) ({ \
    unsigned long long __x = (x); \
    unsigned long long __limit = (((__x) & 0xffff) | (((__x) >> 32) & 0xf0000)); \
    ((__x) & (1ULL << 55)) ? ((__limit << 12) | 0xfff): (__limit); \
})
#define GET_SEG_BASE(x) ({ \
    unsigned long long __x = (x); \
    (((__x >> 16) & 0xffffff) | ((x >> 32) & 0xff000000)); \
})
#define SEG_PRESENT(desc) (((desc) >> 47) & 1)




// useful macros for page-translation
#define GET_PAGE_OFFSET(addr) ((addr) & 0xfff)
#define GET_PAGE_NUMBER(addr) ((addr) >> 12)
#define GET_PAGE_NUMBER_INPLACE(addr) ((addr) & 0xfffff000)
#define ROUND_TO_PAGE(addr) ((addr) & 0xfffff000)

#define GET_PAGE_PAGE(addr) (((addr) >> 12) & 0x3ff)
#define GET_PAGE_DIR(addr) (((addr) >> 22) & 0x3ff)

#define GET_PAGE_FRAME(e) ((e) & 0xfffff000)
#define PAGE_PRESENT(e) ((e) & 1)

#define MAX_PAGES (1 << 20)

#define PAGE_NUMBER_NOT_EQU(addr, raddr) (GET_PAGE_NUMBER_INPLACE((addr) ^ (raddr)) != 0)
#define ADDR_CROSS_PAGE(addr, len) (PAGE_NUMBER_NOT_EQU((addr), (addr) + (len) - 1))

#ifndef USE_VERY_FAST_MEMORY
uint32_t swaddr_read(swaddr_t, size_t, uint8_t);
void swaddr_write(swaddr_t, size_t, uint32_t, uint8_t);
#else
#include "memory/vfmemory.h"
#endif

#endif
