#include "common.h"
#include "memory/memory.h"
#include "device/mmio.h"
#include "cpu/reg.h"

/* very fast memory implementation */

#ifdef USE_VERY_FAST_MEMORY


extern hwaddr_t page_translate_nocache(lnaddr_t addr);
extern int verify_page_translate(lnaddr_t addr, hwaddr_t hwaddr);
extern uint32_t swaddr_read_slow(swaddr_t addr, size_t len, uint8_t sreg);
extern void swaddr_write_slow(swaddr_t addr, size_t len, uint32_t data, uint8_t sreg);



char fast_mem[HW_MEM_SIZE];
uint8_t *hw_mem = (void *)fast_mem;



// ================ HWADDR: SLOW VERSION, FOR OLD SLOW ROUTINE ==============
uint32_t hwaddr_read(hwaddr_t addr, size_t len) {
    int mmio_id = is_mmio(addr);
    if (likely(mmio_id < 0)) {
        return hwaddr_read_no_mmio_check(addr, len);
    } else {
        return mmio_read(addr, len, mmio_id);
    }
}

void hwaddr_write(hwaddr_t addr, size_t len, uint32_t data) {
    int mmio_id = is_mmio(addr);
    if (likely(mmio_id < 0)) {
        hwaddr_write_no_mmio_check(addr, len, data);
    } else {
        mmio_write(addr, len, data, mmio_id);
    }
}





// ================ SWADDR_READ : VERY IMPORTANT =================

unsigned addr_translate_data[MAX_PAGES];

uint32_t __attribute__((noinline)) swaddr_read_not_common(swaddr_t addr, size_t len, uint8_t sreg)
{
    unsigned ppn = addr_translate_data[GET_PAGE_NUMBER(addr)];
    unsigned addr_status = ppn & VFMEMORY_ADDR_MASK;
    if (addr_status == VFMEMORY_ADDR_NOTCONTINUOUS) {
        // this page is not physically continuous
        if (likely(!ADDR_CROSS_PAGE(addr, len))) {
            // not cross-page
            unsigned hwaddr = GET_PAGE_FRAME(ppn) | GET_PAGE_OFFSET(addr);
            return hwaddr_read_no_mmio_check(hwaddr, len);
        } else {
            int shift = 0;
            uint32_t ret = 0;
            while (len--) {
                unsigned hwaddr = page_translate_nocache(addr++);
                uint32_t cur = hwaddr_read(hwaddr, 1);
                ret = ret | (cur << shift);
                shift += 8;
            }
            return ret;
        }
    } else if (addr_status == VFMEMORY_ADDR_MMIO) {
        // the second common case: is mmio area
        int mmio_id = GET_PAGE_NUMBER_INPLACE(ppn); // ppn and mmio share the same bit-range
        return mmio_read(addr, len, mmio_id);
    } else if (addr_status == VFMEMORY_ADDR_UNKNOWN) {
        // the third common case: mmio not checked, need page-translation
        // first we need to check mmio
        int mmio_id = is_mmio(addr);
        unsigned vpn = GET_PAGE_NUMBER(addr);
        if (likely(mmio_id < 0)) {
            unsigned hwaddr = page_translate_nocache(addr);
            unsigned frame = GET_PAGE_NUMBER_INPLACE(hwaddr);
            // see if a continuous physical page follows this page
            if (vpn == 0xfffff || !verify_page_translate(GET_PAGE_NUMBER_INPLACE(addr) + PAGE_SIZE, frame + PAGE_SIZE) || is_mmio(frame + PAGE_SIZE) >= 0) {
                frame |= VFMEMORY_ADDR_NOTCONTINUOUS;
            }
            // see backwards (useful when accessing newly allocated page)
            if (vpn > 0 && verify_page_translate(GET_PAGE_NUMBER_INPLACE(addr) - PAGE_SIZE, frame - PAGE_SIZE) && is_mmio(frame + PAGE_SIZE) < 0) {
                addr_translate_data[vpn - 1] = frame - PAGE_SIZE;
            }
            addr_translate_data[vpn] = frame;
            return hwaddr_read_no_mmio_check(hwaddr, len);
        } else {
            addr_translate_data[vpn] = (mmio_id << 12) | VFMEMORY_ADDR_MMIO;
            return mmio_read(addr, len, mmio_id);
        }
    } else {
        // data is invalid, may be we are:
        //     protect mode is disabled, or
        //     seg translate is not flat,
        //     page translate is disabled
        // so we should use the original routine
        assert(addr_status == VFMEMORY_ADDR_INVALID);
        return swaddr_read_slow(addr, len, sreg);
    }
}




// SWADDR_WRITE
void __attribute__((noinline)) swaddr_write_not_common(swaddr_t addr, size_t len, uint32_t data, uint8_t sreg)
{
    unsigned ppn = addr_translate_data[GET_PAGE_NUMBER(addr)];
    unsigned addr_status = ppn & VFMEMORY_ADDR_MASK;
    if (addr_status == VFMEMORY_ADDR_NOTCONTINUOUS) {
        // this page is not physically continuous
        if (likely(!ADDR_CROSS_PAGE(addr, len))) {
            // not cross-page
            unsigned hwaddr = GET_PAGE_FRAME(ppn) | GET_PAGE_OFFSET(addr);
            hwaddr_write_no_mmio_check(hwaddr, len, data);
        } else {
            while (len--) {
                unsigned hwaddr = page_translate_nocache(addr++);
                hwaddr_write(hwaddr, 1, data & 0xff);
                data >>= 8;
            }
        }
    } else if (addr_status == VFMEMORY_ADDR_MMIO) {
        // the second common case: is mmio area
        int mmio_id = GET_PAGE_NUMBER_INPLACE(ppn); // ppn and mmio share the same bit-range
        mmio_write(addr, len, data, mmio_id);
    } else if (addr_status == VFMEMORY_ADDR_UNKNOWN) {
        // the third common case: mmio not checked, need page-translation
        // first we need to check mmio
        int mmio_id = is_mmio(addr);
        unsigned vpn = GET_PAGE_NUMBER(addr);
        if (likely(mmio_id < 0)) {
            unsigned hwaddr = page_translate_nocache(addr);
            unsigned frame = GET_PAGE_NUMBER_INPLACE(hwaddr);
            // see if a continuous physical page follows this page
            if (vpn == 0xfffff || !verify_page_translate(GET_PAGE_NUMBER_INPLACE(addr) + PAGE_SIZE, frame + PAGE_SIZE) || is_mmio(frame + PAGE_SIZE) >= 0) {
                frame |= VFMEMORY_ADDR_NOTCONTINUOUS;
            }
            // see backwards (useful when accessing newly allocated page)
            if (vpn > 0 && verify_page_translate(GET_PAGE_NUMBER_INPLACE(addr) - PAGE_SIZE, frame - PAGE_SIZE) && is_mmio(frame + PAGE_SIZE) < 0) {
                addr_translate_data[vpn - 1] = frame - PAGE_SIZE;
            }
            addr_translate_data[vpn] = frame;
            hwaddr_write_no_mmio_check(hwaddr, len, data);
        } else {
            addr_translate_data[vpn] = (mmio_id << 12) | VFMEMORY_ADDR_MMIO;
            mmio_write(addr, len, data, mmio_id);
        }
    } else {
        // data is invalid, may be we are:
        //     protect mode is disabled, or
        //     seg translate is not flat,
        //     page translate is disabled
        // so we should use the original routine
        assert(addr_status == VFMEMORY_ADDR_INVALID);
        swaddr_write_slow(addr, len, data, sreg);
    }
}











void make_translate_invalid() // mark all addr to use old and slow method
{
    memset(addr_translate_data, VFMEMORY_ADDR_INVALID, sizeof(addr_translate_data));
}

void make_translate_flushed() 
{
    // recheck status:
        // if
        //      protect mode on, and
        //      page translation enabled, and
        //      seg translation is flat
        // then
        //      mark all addr need to check mmio and page-translation
        // else
        //      mark all invalid
    
    if (cpu.PE && cpu.PG && cpu.seg_is_flat)
        memset(addr_translate_data, VFMEMORY_ADDR_UNKNOWN, sizeof(addr_translate_data));
    else
        make_translate_invalid();
}

void init_ddr3()
{
    make_translate_invalid();
}
#endif
