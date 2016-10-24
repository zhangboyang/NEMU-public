#include "common.h"
#include "cpu/reg.h"
#include "memory/memory.h"
#include "device/mmio.h"

// dram functions
extern uint32_t dram_read(hwaddr_t, size_t);
extern void dram_write(hwaddr_t, size_t, uint32_t);

// translate functions
extern lnaddr_t seg_translate(swaddr_t addr, size_t len, uint8_t sreg);
extern hwaddr_t page_translate(lnaddr_t addr);

/* Memory accessing interfaces */


#if !defined(USE_VERY_FAST_MEMORY) && !defined(USE_VERY_FAST_MEMORY_VER2)
// ================ HWADDR ==================

uint32_t hwaddr_read_nocache(hwaddr_t addr, size_t len) {
	return dram_read(addr, len) & (~0u >> ((4 - len) << 3));
}

void hwaddr_write_nocache(hwaddr_t addr, size_t len, uint32_t data) {
	dram_write(addr, len, data);
}




inline uint32_t hwaddr_read(hwaddr_t addr, size_t len) {
    assert(len <= 4);
    
    // check for mmio
    int mmio_id = is_mmio(addr);
    if (likely(mmio_id < 0)) { // not mmio, read cache
#ifdef USE_MEMORY_CACHE
        uint32_t ret = 0;
	    extern void l1cache_read(void *dest, unsigned addr, int len);
	    l1cache_read(&ret, addr, len);
    	return ret;
#else
        return hwaddr_read_nocache(addr, len);
#endif
    } else {
        return mmio_read(addr, len, mmio_id);
    }
}

inline void hwaddr_write(hwaddr_t addr, size_t len, uint32_t data) {
    assert(len <= 4);
    
    // check for mmio
    int mmio_id = is_mmio(addr);
    if (likely(mmio_id < 0)) { // not mmio, write cache
#ifdef USE_MEMORY_CACHE
	    extern void l1cache_write(unsigned addr, void *src, int len);
	    l1cache_write(addr, &data, len);
#else
        hwaddr_write_nocache(addr, len, data);
#endif
    } else {
        mmio_write(addr, len, data, mmio_id);
    }
}
#endif


// ================ LNADDR ==================


uint32_t __attribute__((noinline)) lnaddr_crosspage_read(lnaddr_t addr, lnaddr_t raddr, size_t len)
{
    // data cross the page boundary
    // read data byte by byte
    
    hwaddr_t first_hwaddr, second_hwaddr;
    volatile uint32_t result;
    uint8_t *result_bytes = (void *) &result;

    // data cross the page boundary
    int poffset = GET_PAGE_OFFSET(addr);
    int rpoffset = GET_PAGE_OFFSET(raddr);
    
    int i, j = 0;
    
    // read (PAGE_SIZE - poffset) bytes from page 'pn'
    assert(PAGE_SIZE - poffset > 0);
    assert(PAGE_SIZE - poffset < 4);
    first_hwaddr = page_translate(addr);
    for (i = 0; i < PAGE_SIZE - poffset; i++)
        result_bytes[j++] = hwaddr_read(first_hwaddr + i, 1);
    
    // read (rpoffset + 1) bytes from page 'rpn'
    assert(rpoffset + 1 > 0);
    assert(rpoffset + 1 < 4);
    second_hwaddr = page_translate(ROUND_TO_PAGE(raddr));
    for (i = 0; i < rpoffset + 1; i++)
        result_bytes[j++] = hwaddr_read(second_hwaddr + i, 1);
    
    assert(j == len);

    #ifdef DEBUG
    volatile uint32_t sresult;
    uint8_t *sresult_bytes = (void *) &sresult;
    for (i = 0; i < len; i++)
        sresult_bytes[i] = hwaddr_read(page_translate(addr + i), 1);
    assert(sresult == result);
    #endif
    
    return result;
}
inline uint32_t lnaddr_read(lnaddr_t addr, size_t len) {
    // [addr, addr + 1, addr + 2, addr + 3] addr + 4)
    //                             ^ raddr
    //  4095 | 0      | 1       | 2       | 3
    // [FIRST, 0x00   , 0x00    , 0x00]
    //        [SECOND===================  , 0x00 ]
    // [REQUESTED                         ]
    
    assert(len == 1 || len == 2 || len == 4);
    
    if (ADDR_CROSS_PAGE(addr, len)) {
        lnaddr_t raddr = addr + len - 1;
        return lnaddr_crosspage_read(addr, raddr, len);
    } else {
        hwaddr_t hwaddr = page_translate(addr);
	    return hwaddr_read(hwaddr, len);
	}
}
void __attribute__((noinline)) lnaddr_crosspage_write(lnaddr_t addr, size_t len, uint32_t data)
{
    uint8_t *data_bytes = (void *) &data;
    int i;
    for (i = 0; i < len; i++)
        hwaddr_write(page_translate(addr + i), 1, data_bytes[i]);
}
inline void lnaddr_write(lnaddr_t addr, size_t len, uint32_t data) {
    assert(len == 1 || len == 2 || len == 4);
//    lnaddr_t raddr = addr + len - 1;
    if (ADDR_CROSS_PAGE(addr, len)) {
        lnaddr_crosspage_write(addr, len, data);
    } else {
        hwaddr_t hwaddr = page_translate(addr);
        //printf("lnaddr_write: addr=%08x hwaddr=%08x data=%08x len=%d\n", addr, hwaddr, data, (int) len);
	    hwaddr_write(hwaddr, len, data);
	}
}



// ================ SWADDR ==================


uint32_t swaddr_read_slow(swaddr_t addr, size_t len, uint8_t sreg) {
#ifdef DEBUG
	assert(len == 1 || len == 2 || len == 4);
#endif
    lnaddr_t lnaddr = seg_translate(addr, len, sreg);
	return lnaddr_read(lnaddr, len);
}


void swaddr_write_slow(swaddr_t addr, size_t len, uint32_t data, uint8_t sreg) {
#ifdef DEBUG
	assert(len == 1 || len == 2 || len == 4);
#endif
    lnaddr_t lnaddr = seg_translate(addr, len, sreg);
	lnaddr_write(lnaddr, len, data);
}


#if !defined(USE_VERY_FAST_MEMORY) && !defined(USE_VERY_FAST_MEMORY_VER2)
#if 0
// temp code for faster-qemu-PPT
int __attribute__((noinline)) verify_page_translate(lnaddr_t addr, hwaddr_t hwaddr);
uint32_t myTLB2[1 << 20];
uint32_t __attribute__((noinline)) swaddr_read_miss(swaddr_t addr, size_t len, uint8_t sreg)
{
    uint32_t pa1 = page_translate(addr);
    int pa1_mmio_id = is_mmio(pa1);
    if (pa1_mmio_id < 0) {
        if (verify_page_translate(addr + 0x1000, pa1 + 0x1000)) {
            if (is_mmio(page_translate(addr + 0x1000)) < 0) {
                myTLB2[addr >> 12] = pa1 & ~0xFFF;
            }
        }
        return hwaddr_read(pa1, len);
    } else {
        return mmio_read(pa1, len, pa1_mmio_id);
    }
}
void flush_myTLB2()
{
    printf("FLUSH TLB2!!!\n");
    memset(myTLB2, -1, sizeof(myTLB2));
}
uint32_t inline swaddr_read(swaddr_t addr, size_t len, uint8_t sreg)
{
    uint32_t result = myTLB2[addr >> 12];
    if (likely((result & 0xFFF) == 0)) {
        return dram_read(result | (addr & 0xFFF), len);
    } else {
        return swaddr_read_miss(addr, len, sreg);
    }
}
void __attribute__((noinline)) swaddr_write_miss(swaddr_t addr, size_t len, uint32_t data, uint8_t sreg)
{
    uint32_t pa1 = page_translate(addr);
    int pa1_mmio_id = is_mmio(pa1);
    if (pa1_mmio_id < 0) {
        if (verify_page_translate(addr + 0x1000, pa1 + 0x1000)) {
            if (is_mmio(page_translate(addr + 0x1000)) < 0) {
                myTLB2[addr >> 12] = pa1 & ~0xFFF;
            }
        }
        hwaddr_write(pa1, len, data);
    } else {
        mmio_write(pa1, len, data, pa1_mmio_id);
    }
}
void inline swaddr_write(swaddr_t addr, size_t len, uint32_t data, uint8_t sreg)
{
    uint32_t result = myTLB2[addr >> 12];
    if (likely((result & 0xFFF)) == 0) {
        dram_write(result | (addr & 0xFFF), len, data);
    } else {
        swaddr_write_miss(addr, len, data, sreg);
    }
}
#endif

#if 1
uint32_t swaddr_read(swaddr_t addr, size_t len, uint8_t sreg) {
    return swaddr_read_slow(addr, len, sreg);
}
void swaddr_write(swaddr_t addr, size_t len, uint32_t data, uint8_t sreg) {
    swaddr_write_slow(addr, len, data, sreg);
}
#endif

#endif

// ================ UI: SAFE_SWADDR ==================

int safe_read_flag, safe_read_failed;
int ui_safe_read_failed;
uint32_t safe_swaddr_read(swaddr_t addr, size_t len, int *success, uint8_t sreg) {
    /* debugger need safe-read feature */
#ifdef DEBUG
	assert(len == 1 || len == 2 || len == 4);
#endif
    uint32_t ret;
    safe_read_flag = 1;
    safe_read_failed = 0;
	ret = swaddr_read_slow(addr, len, sreg);
	if (safe_read_failed) {
	    ret = 0xCCCCCCCC & (((1 << (len * 8 - 1)) << 1) - 1);
	    if (success == NULL) ui_safe_read_failed = 1;
	}
	safe_read_flag = 0;
	if (success != NULL) *success = !safe_read_failed;
	return ret;
}



