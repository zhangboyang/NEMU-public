#include "common.h"
#include "x86.h"

//#define USE_DMA_READ

#define IDE_PORT_BASE   0x1F0

void dma_prepare(void *);
void dma_issue_read(void);

void clear_ide_intr(void);
void wait_ide_intr(void);

static void waitdisk() {
	while ( (in_byte(IDE_PORT_BASE + 7) & (0x80 | 0x40)) != 0x40);
}

static void
ide_prepare(uint32_t sector) {
	waitdisk();

#ifdef USE_DMA_READ
	out_byte(IDE_PORT_BASE + 1, 1);
#else
	out_byte(IDE_PORT_BASE + 1, 0);
#endif
    //printk("%d %d\n", sector, (sector >> 8) & 0xFF);
//    printk("2:%02x, 3:%02x, 4:%02x, 5:%02x, 6:%02x\n",
//        1, sector & 0xFF,  (sector >> 8) & 0xFF, (sector >> 16) & 0xFF, 0xE0 | ((sector >> 24) & 0xFF));
	out_byte(IDE_PORT_BASE + 2, 1);
	out_byte(IDE_PORT_BASE + 3, sector & 0xFF);
	out_byte(IDE_PORT_BASE + 4, (sector >> 8) & 0xFF);
	out_byte(IDE_PORT_BASE + 5, (sector >> 16) & 0xFF);
	out_byte(IDE_PORT_BASE + 6, 0xE0 | ((sector >> 24) & 0xFF));
//	panic("aha!");
}

static inline void
issue_read() {
#ifdef USE_DMA_READ
	out_byte(IDE_PORT_BASE + 7, 0xc8);
	dma_issue_read();
#else
	out_byte(IDE_PORT_BASE + 7, 0x20);
#endif
}

static inline void
issue_write() {
	out_byte(IDE_PORT_BASE + 7, 0x30);
}

void
disk_do_read(void *buf, uint32_t sector) {
#ifdef USE_DMA_READ
	dma_prepare(buf);

	clear_ide_intr();
#endif
    //printk("sector=%d 0x%08x\n", sector, sector);
	ide_prepare(sector);
	issue_read();

#ifdef USE_DMA_READ
	wait_ide_intr();
#else
	int i;
	for (i = 0; i < 512 / sizeof(uint32_t); i ++) {
		*(((uint32_t*)buf) + i) = in_long(IDE_PORT_BASE);
		//printk("%08x ", (unsigned) *(((uint32_t*)buf) + i));
	}
#endif

}

void
disk_do_write(void *buf, uint32_t sector) {
	int i;

	ide_prepare(sector);
	issue_write();

	for (i = 0; i < 512 / sizeof(uint32_t); i ++) {
		out_long(IDE_PORT_BASE, *(((uint32_t*)buf) + i));
	}
}
