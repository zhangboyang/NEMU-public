#include "common.h"
#include "memory.h"
#include <string.h>

static PDE updir[NR_PDE] align_to_page; // user pagedir
static CR3 ucr3; // user cr3

PDE* get_updir() { return updir; }
uint32_t get_ucr3() { return ucr3.val; }

PDE* get_kpdir();

uint32_t brk = 0;

/* The brk() system call handler. */
void mm_brk(uint32_t new_brk) {
	if(new_brk > brk) {
		mm_malloc(brk, new_brk - brk);
	}
	brk = new_brk;
}

void init_mm() {
	PDE *kpdir = get_kpdir();

	/* make all PDE invalid */
	memset(updir, 0, NR_PDE * sizeof(PDE));

	/* create the same mapping above 0xc0000000 as the kernel mapping does */
	memcpy(&updir[KOFFSET / PT_SIZE], &kpdir[KOFFSET / PT_SIZE], 
			(PHY_MEM / PT_SIZE) * sizeof(PDE));

	ucr3.val = (uint32_t)va_to_pa((uint32_t)updir) & ~0xfff;
}


static PTE uptable_array[1][NR_PTE] align_to_page;

void create_user_mapping(int uptable_id, unsigned physaddr, unsigned virtaddr)
{
    // create a single page mapping in user's address space
    // physaddr and virtaddr should aligned to page
    assert((physaddr & 0xfff) == 0);
    assert((virtaddr & 0xfff) == 0);
    
    // can't alloc more than 1024 pages since we have only one pagetable
    assert(uptable_id == 0);
    
    //printk("user mapping: %08X => %08X\n", virtaddr, physaddr);
    
    unsigned dir = GET_PAGE_DIR(virtaddr);
    unsigned page = GET_PAGE_PAGE(virtaddr);
    
    PTE *pt = uptable_array[uptable_id];
    assert(((unsigned) pt & 0xfff) == 0);
    
    updir[dir].val = make_pde(va_to_pa(pt));
    
    pt[page].val = make_pte(physaddr);
}
