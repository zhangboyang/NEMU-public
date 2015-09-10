
#ifdef USE_VERY_FAST_MEMORY


// ================ SWADDR_READ : VERY IMPORTANT =================
// physical continuous-check, page-translate, seg-translate, mmio-check is combined in this part
//
// addr_translate_data[PPN]
//  31---+          +---20    +---10    +---0
//       10987654321098765432109876543210
//       ********************************
//       [=======VPN========]000000000000  ===> most common case: page_translate result cached, physical continuous, seg-translate is flat mode, not mmio area
//       [=====MMIOID=======]????????XXXX  ===> mmio area
//       [=======VPN========]????????YYYY  ===> page_translate result cached, NOT physically continuous, seg-translate is flat mode, not mmio area
//       ????????????????????????????ZZZZ  ===> need mmio-check and page_translate
//       ????????????????????????????WWWW  ===> initial status: need use slow methods

//  bit0: fast-mode-flag (invalid flag): 0=>seg is flat mode; 1=>using slow methods
//  bit1: page-cache-flag: 0=>page cached; 1=>need translate
//  bit2: mmio-flag: 0=>not mmio area; 1=>is_mmio_area

#define VFMEMORY_ADDR_MASK 0x0f

#define VFMEMORY_ADDR_NORMAL 0x00 // must be 0
#define VFMEMORY_ADDR_MMIO 0x02
#define VFMEMORY_ADDR_NOTCONTINUOUS 0x08
#define VFMEMORY_ADDR_UNKNOWN 0x04
#define VFMEMORY_ADDR_INVALID 0x0f





extern char fast_mem[];
extern unsigned addr_translate_data[MAX_PAGES];

extern uint32_t swaddr_read_not_common(swaddr_t addr, size_t len, uint8_t sreg);
extern void __attribute__((noinline)) swaddr_write_not_common(swaddr_t addr, size_t len, uint32_t data, uint8_t sreg);

// ================ HWADDR: FAST VERSION, NO MMIO CHECK =================
static inline uint32_t hwaddr_read_no_mmio_check(hwaddr_t addr, size_t len)
{
    uint32_t ret;
//    memcpy(&ret, &fast_mem[addr], len);
    ret = (*(uint32_t *)(fast_mem + addr)) & ((1LL << (len << 3)) - 1);
    return ret;
}
static inline void hwaddr_write_no_mmio_check(hwaddr_t addr, size_t len, uint32_t data)
{
//    memcpy(&fast_mem[addr], &data, len);
    char *dst = fast_mem + addr;
    do {
        *dst++ = data;
        data >>= 8;
    } while (--len);
}

static inline uint32_t swaddr_read(swaddr_t addr, size_t len, uint8_t sreg)
{
    // assume we are using page-translation
    unsigned ppn = addr_translate_data[GET_PAGE_NUMBER(addr)];
    unsigned addr_status = ppn & VFMEMORY_ADDR_MASK; // get last 4 bits
    if (likely(addr_status == VFMEMORY_ADDR_NORMAL)) {
        // it's the most common case: page-cached
        unsigned hwaddr = ppn | GET_PAGE_OFFSET(addr);
        return hwaddr_read_no_mmio_check(hwaddr, len);
    } /*else if (addr_status == 8) {
        // we are not sure if this operation cross page
        // we need check here
        if (likely(!ADDR_CROSS_PAGE(addr, len))) {
            // not cross-page
            unsigned hwaddr = GET_PAGE_FRAME(ppn) | GET_PAGE_OFFSET(addr);
            return hwaddr_read_no_mmio_check(hwaddr, len);
        }
        // cross page, we will handle it in not-common
    }*/
    return swaddr_read_not_common(addr, len, sreg);
}

static inline void swaddr_write(swaddr_t addr, size_t len, uint32_t data, uint8_t sreg)
{
    // assume we are using page-translation
    unsigned ppn = addr_translate_data[GET_PAGE_NUMBER(addr)];
    unsigned addr_status = ppn & VFMEMORY_ADDR_MASK; // get last 4 bits
    if (likely(addr_status == VFMEMORY_ADDR_NORMAL)) {
        // it's the most common case: page-cached
        unsigned hwaddr = ppn | GET_PAGE_OFFSET(addr);
        hwaddr_write_no_mmio_check(hwaddr, len, data);
        return;
    } /*else if (addr_status == 8) {
        // we are not sure if this operation cross page
        // we need check here
        if (likely(!ADDR_CROSS_PAGE(addr, len))) {
            // not cross-page
            unsigned hwaddr = GET_PAGE_FRAME(ppn) | GET_PAGE_OFFSET(addr);
            hwaddr_write_no_mmio_check(hwaddr, len, data);
            return;
        }
        // cross page, we will handle it in not-common
    }*/
    swaddr_write_not_common(addr, len, data, sreg);
}



// get pointer to data, instead of returning data directly
// if this addr is not suitable for direct read (i.e. the not common case), NULL is returned
// a addr is suitable, means it can safely read with in current page and next page
// [DON'T READ ME][  OK        ][  OK        ][DON'T READ ME]
//                       ^ addr
static inline char *swaddr_getptr(swaddr_t addr)
{
    // assume we are using page-translation
    unsigned ppn = addr_translate_data[GET_PAGE_NUMBER(addr)];
    unsigned addr_status = ppn & VFMEMORY_ADDR_MASK; // get last 4 bits
    if (likely(addr_status == VFMEMORY_ADDR_NORMAL)) {
        // it's the most common case: page-cached
        unsigned hwaddr = ppn | GET_PAGE_OFFSET(addr);
        return fast_mem + hwaddr;
    }
    return NULL;
}

#endif
