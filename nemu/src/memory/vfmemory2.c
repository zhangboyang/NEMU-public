#include "common.h"
#include "memory/memory.h"
#include "device/mmio.h"
#include "cpu/reg.h"

#ifdef USE_VERY_FAST_MEMORY_VER2


/*
    NOTE:
        VFMEMORY2 REQUIRE x86-64 MACHINE (because address space must bigger than 4GB)
        DOESN'T SUPPORT mmio AND segment translation
*/

#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

#include "sigsegv.h"

#include <stdio.h>
#include <string.h>



#ifdef VFMEMORY2_IDENTITY_MAP
// use [0x70000000, 0x80000000] for NEMU
// don't forget to add "-Wl,-Ttext-segment=0x70000000" to LD_FLAGS
#define VIRT_LOW_BASE 0x10000
#define VIRT_LOW_LIMIT 0x70000000
#define VIRT_HIGH_BASE 0x80000000
#endif




#define SHM_FILE_NAME "/nemu-physmem"
#define PHYS_MEMSIZE (1024 * 1024 * 128)
#define VIRT_MEMSIZE (1024LL * 1024 * 4096)

static int create_memfd()
{
    int fd;
    int ret;
    fd = shm_open(SHM_FILE_NAME, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (fd == -1) return -1;
    ret = ftruncate(fd, PHYS_MEMSIZE);
    if (ret == -1) return -1;
    ret = shm_unlink(SHM_FILE_NAME);
    if (ret == -1) return -1;
    return fd;
}

//static void *vbase = ((void *)0x0000100000000000);
static void *vbase = NULL;
static void *pbase = NULL;
static int memfd = -1, zerofd = -1;

static void *vmm_init() // init the Virtual Memory Manager
{
    memfd = create_memfd();
    zerofd = open("/dev/zero", O_RDONLY);
#ifdef VFMEMORY2_IDENTITY_MAP
    vbase = NULL;
    if (mmap((void *) VIRT_LOW_BASE, VIRT_LOW_LIMIT - VIRT_LOW_BASE, PROT_NONE, MAP_SHARED | MAP_FIXED, zerofd, 0) != (void *) VIRT_LOW_BASE) {
        panic("mmap() low area failed!");
    }
    if (mmap((void *) VIRT_HIGH_BASE, VIRT_MEMSIZE - VIRT_HIGH_BASE, PROT_NONE, MAP_SHARED | MAP_FIXED, zerofd, 0) != (void *) VIRT_HIGH_BASE) {
        panic("mmap() high area failed!");
    }
#else
    vbase = mmap(vbase, VIRT_MEMSIZE, PROT_NONE, MAP_SHARED, zerofd, 0);
#endif
    pbase = mmap(NULL, PHYS_MEMSIZE, PROT_READ | PROT_WRITE, MAP_SHARED, memfd, 0);
    

    return vbase;
}

static void *vmm_clearmapping()
{
#ifdef VFMEMORY2_IDENTITY_MAP
    if (mmap((void *) VIRT_LOW_BASE, VIRT_LOW_LIMIT, PROT_NONE, MAP_SHARED | MAP_FIXED, zerofd, 0) != (void *) VIRT_LOW_BASE) {
        panic("mmap() low area failed!");
    }
    if (mmap((void *) VIRT_HIGH_BASE, VIRT_MEMSIZE - VIRT_HIGH_BASE, PROT_NONE, MAP_SHARED | MAP_FIXED, zerofd, 0) != (void *) VIRT_HIGH_BASE) {
        panic("mmap() high area failed!");
    }
#else
    vbase = mmap(vbase, VIRT_MEMSIZE, PROT_NONE, MAP_SHARED | MAP_FIXED, zerofd, 0);
#endif
    return vbase;
}

static void *vmm_createmapping(unsigned vaddr, unsigned paddr, unsigned len)
{
    return mmap(vbase + vaddr, len, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_FIXED, memfd, paddr);
}










static sigsegv_dispatcher dispatcher;
static int handler(void *fault_address, int serious)
{
    return sigsegv_dispatch(&dispatcher, fault_address);
}

static int pfh_handler(void *fault_address, void *user_arg);


static void pfh_init() // init the Page Fault Handler
{
    sigsegv_init(&dispatcher);
    sigsegv_install_handler(&handler);
    sigsegv_register(&dispatcher, vbase, VIRT_MEMSIZE, &pfh_handler, NULL);
}












uint8_t *hw_mem = NULL;
uint8_t *virt_mem = NULL;


// ================ HWADDR: SLOW VERSION, FOR OLD SLOW ROUTINE ==============
static inline uint32_t hwaddr_read_no_mmio_check(hwaddr_t addr, size_t len)
{
    uint32_t ret;
    ret = (*(uint32_t *)(hw_mem + addr)) & ((1LL << (len << 3)) - 1);
    return ret;
}
static inline void hwaddr_write_no_mmio_check(hwaddr_t addr, size_t len, uint32_t data)
{
    if (len == 4) {
        *(uint32_t *)(hw_mem + addr) = data;
    } else if (len == 2) {
        *(uint16_t *)(hw_mem + addr) = (uint16_t) data;
    } else {
        assert(len == 1);
        *(uint8_t *)(hw_mem + addr) = (uint8_t) data;
    }
}
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









extern hwaddr_t page_translate_nocache(lnaddr_t addr);
extern int verify_page_translate(lnaddr_t addr, hwaddr_t hwaddr);


static int pfh_handler(void *fault_address, void *user_arg)
{
    unsigned vaddr = fault_address - vbase;
    unsigned paddr = page_translate_nocache(vaddr);
    //printf("page fault at %p, vaddr = %08X, paddr = %08X\n", fault_address, vaddr, paddr);
    vmm_createmapping(GET_PAGE_NUMBER_INPLACE(vaddr), GET_PAGE_NUMBER_INPLACE(paddr), PAGE_SIZE);
    return 1;
}





void init_ddr3()
{
    vmm_init();
    pfh_init();
    hw_mem = pbase;
    virt_mem = vbase;
}

void vfmemory2_flush()
{
    vmm_clearmapping();
}

#endif
