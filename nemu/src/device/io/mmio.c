#include "common.h"
#include "device/mmio.h"
#include "memory/memory.h"
#include "misc.h"


#define MMIO_SPACE_MAX (256 * 1024)
#define NR_MAP 8

static uint8_t mmio_space_pool[MMIO_SPACE_MAX];
static uint32_t mmio_space_free_index = 0;

typedef struct {
	hwaddr_t low;
	hwaddr_t high;
	uint8_t *mmio_space;
	mmio_callback_t callback;
} MMIO_t;

static MMIO_t maps[NR_MAP];
static int nr_map = 0;

// by ZBY
//signed char mmio_bitmap[MAX_PAGES];
void init_mmio_map()
{
    //memset(mmio_bitmap, -1, sizeof(mmio_bitmap));
}
/* device interface */
void* add_mmio_map(hwaddr_t addr, size_t len, mmio_callback_t callback) {
	assert(nr_map < NR_MAP);
	assert(mmio_space_free_index + len <= MMIO_SPACE_MAX);

	uint8_t *space_base = &mmio_space_pool[mmio_space_free_index];
	maps[nr_map].low = addr;
	maps[nr_map].high = addr + len - 1;
	
	// combine PAGE-TRANSLATE and MMIO needs align to page
	if (GET_PAGE_OFFSET(nr_map) != 0 || GET_PAGE_OFFSET(nr_map + len) != 0) {
	    panic("mmio map not aligned to page");
	}
	
/*	int page_start = GET_PAGE_NUMBER(addr);
	int pages = len / PAGE_SIZE;
	while (pages--) {
    	//printf("addr=%d, len=%d, page_start = %d, nr_map = %d\n", (int) addr, (int) len, page_start, nr_map);
	    mmio_bitmap[page_start++] = nr_map;
	}
//	exit(1);*/

	maps[nr_map].mmio_space = space_base;
	maps[nr_map].callback = callback;
	nr_map ++;
	mmio_space_free_index += len;
	return space_base;
}

/* bus interface */

int is_mmio(hwaddr_t addr) {
	int i;
	for(i = 0; i < nr_map; i ++) {
		if(unlikely(addr <= maps[i].high) && likely(addr >= maps[i].low)) {
			return i;
		}
	}
	return -1;
	//return mmio_bitmap[GET_PAGE_NUMBER(addr)];
}

uint32_t mmio_read(hwaddr_t addr, size_t len, int map_NO) {
	assert(len == 1 || len == 2 || len == 4);
	MMIO_t *map = &maps[map_NO];
	uint32_t data = *(uint32_t *)(map->mmio_space + (addr - map->low)) 
		& (~0u >> ((4 - len) << 3));
	map->callback(addr, len, false);
	return data;
}

void mmio_write(hwaddr_t addr, size_t len, uint32_t data, int map_NO) {
	assert(len == 1 || len == 2 || len == 4);
	MMIO_t *map = &maps[map_NO];
	uint32_t mask = (~0u >> ((4 - len) << 3));
	memcpy_with_mask(map->mmio_space + (addr - map->low), &data, len, (void *)&mask);
	maps[map_NO].callback(addr, len, true);
}
