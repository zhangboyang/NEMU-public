#ifdef USE_VERY_FAST_MEMORY_VER2
#ifdef VFMEMORY2_IDENTITY_MAP
#define virt_mem ((void *)0)
#else
extern uint8_t *virt_mem;
#endif
static inline uint32_t swaddr_read(swaddr_t addr, size_t len, uint8_t sreg)
{
    return (*(uint32_t *)(virt_mem + addr)) & ((1LL << (len << 3)) - 1);
}

static inline void swaddr_write(swaddr_t addr, size_t len, uint32_t data, uint8_t sreg)
{
    if (len == 4) {
        *(uint32_t *)(virt_mem + addr) = data;
    } else if (len == 2) {
        *(uint16_t *)(virt_mem + addr) = (uint16_t) data;
    } else {
        assert(len == 1);
        *(uint8_t *)(virt_mem + addr) = (uint8_t) data;
    }
}
#ifdef VFMEMORY2_IDENTITY_MAP
#undef virt_mem
#endif
#endif
