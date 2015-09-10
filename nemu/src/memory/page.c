#include <stdlib.h>
#include "common.h"
#include "cpu/reg.h"
#include "memory/memory.h"

extern int safe_read_flag;
extern int safe_read_failed;

char page_errbuf[4096]; // 4K should enough
int page_nopanic = 0;
int page_errflag = 0;



hwaddr_t __attribute__((noinline)) page_translate_nocache(lnaddr_t addr)
{
    if (unlikely(!cpu.PE || !cpu.PG)) return addr;
    
    
    
    unsigned dir = GET_PAGE_DIR(addr);
    unsigned page = GET_PAGE_PAGE(addr);
    unsigned offset = GET_PAGE_OFFSET(addr);
    
    //printf("page-translate: addr=%08x dir=%08x page=%08x offset=%08x\n", addr, dir, page, offset);
    
    // find PDE (page dir entry) first
    unsigned pgdir = GET_PAGE_FRAME(cpu.CR3);
    unsigned pde = hwaddr_read(pgdir + dir * 4, 4);
    
    //printf("pde=%08x\n", pde);
    
#ifdef DEBUG
    if (!PAGE_PRESENT(pde)) {
        if (safe_read_flag) safe_read_failed = 1;
        else {
            sprintf(page_errbuf, "pde : pte not present, addr=%08X, eip=%08X", addr, cpu.eip);
            if (page_nopanic) { page_errflag = 1; return 0; }
            panic("%s", page_errbuf);
        }
    }
#else
    if (!PAGE_PRESENT(pde)) panic("pde : pte not present, addr=%08X, eip=%08X", addr, cpu.eip);
#endif

    // find PTE (page table entry)
    unsigned pgtable = GET_PAGE_FRAME(pde);
    unsigned pte = hwaddr_read(pgtable + page * 4, 4);
    
    //printf("pte=%08x\n", pte);
#ifdef DEBUG
    if (!PAGE_PRESENT(pte)) {
        if (safe_read_flag) safe_read_failed = 1;
        else {
            sprintf(page_errbuf, "pte: page frame not present, addr=%08X, eip=%08X", addr, cpu.eip);
            if (page_nopanic) { page_errflag = 1; return 0; }
            panic("%s", page_errbuf);
        }
    }
#else
    if (!PAGE_PRESENT(pte)) panic("pte: page frame not present, addr=%08X, eip=%08X", addr, cpu.eip);
#endif

    unsigned hwaddr = GET_PAGE_FRAME(pte) | offset;
    //printf("translate result: lnaddr %08X => hwaddr %08X\n", addr, hwaddr);
    return hwaddr;
}

int __attribute__((noinline)) verify_page_translate(lnaddr_t addr, hwaddr_t hwaddr)
{
    //printf("WANT TO VERIFY: %08X ==?==> %08X\n", addr, hwaddr);
    if (unlikely(!cpu.PE || !cpu.PG)) return 0;
    unsigned dir = GET_PAGE_DIR(addr);
    unsigned page = GET_PAGE_PAGE(addr);
    // find PDE (page dir entry) first
    unsigned pgdir = GET_PAGE_FRAME(cpu.CR3);
    unsigned pde = hwaddr_read(pgdir + dir * 4, 4);
    if (!PAGE_PRESENT(pde)) return 0;
    // find PTE (page table entry)
    unsigned pgtable = GET_PAGE_FRAME(pde);
    unsigned pte = hwaddr_read(pgtable + page * 4, 4);
    if (!PAGE_PRESENT(pte)) return 0;
    //printf("VERIFY: %08X ==> %08X,   expected = %08X\n", addr, GET_PAGE_FRAME(pte), GET_PAGE_NUMBER_INPLACE(hwaddr));
    return GET_PAGE_NUMBER_INPLACE(hwaddr) == GET_PAGE_FRAME(pte);
}





// ================= TLB ==================



#ifdef USE_TLB

#define TLB_SIZE 64

struct tlb_entry {
    int valid;
    unsigned vpn, ppn; // tag is vpn, data is ppn
};

static struct tlb_entry tlb_data[TLB_SIZE];

static struct tlb_entry *find_tlb_entry(unsigned vpn)
{
    int i;
    for (i = 0; i < TLB_SIZE; i++)
        if (tlb_data[i].valid && tlb_data[i].vpn == vpn) {
            // move found to front for efficiency
            // 0, 1, 2, ..., i  , i+1, i+2
            // i, 0, 1, ..., i-1, i+1, i+2
            
            struct tlb_entry tmp;
            tmp = tlb_data[i];
            memmove(&tlb_data[1], &tlb_data[0], sizeof(struct tlb_entry) * i);
            tlb_data[0] = tmp;
            
            return &tlb_data[0];
        }
    return NULL;
}


static void insert_to_tlb(unsigned vpn, unsigned ppn)
{
    int i;
    struct tlb_entry *e;
    for (i = 0; i < TLB_SIZE; i++)
        if (!tlb_data[i].valid) {
            e = &tlb_data[i];
            goto done;
        }
    e = &tlb_data[rand() % TLB_SIZE];
    memmove(&tlb_data[1], &tlb_data[0], sizeof(struct tlb_entry) * (TLB_SIZE - 1)); // drop out last item
    e = &tlb_data[0];
done:
    e->valid = 1;
    e->vpn = vpn;
    e->ppn = ppn;
}

void flush_tlb()
{
    memset(tlb_data, 0, sizeof(tlb_data));
}

void show_tlb()
{
    int i;
    for (i = 0; i < TLB_SIZE; i++) {
        printf("%s", tlb_data[i].valid ? c_green : c_yellow);
        printf("  %3d: %c %08X => %08X  ", i, (tlb_data[i].valid ? 'Y' : 'N'), tlb_data[i].vpn, tlb_data[i].ppn);
        printf("%s", c_normal);
        if (i % 2 == 1) printf("\n");
    }
    printf("\n");
}

hwaddr_t page_translate(lnaddr_t addr)
{

    unsigned vpn = GET_PAGE_NUMBER_INPLACE(addr);
    struct tlb_entry *e = find_tlb_entry(vpn);
    if (e) { // hit
        return e->ppn | GET_PAGE_OFFSET(addr);
    } else { // miss
        hwaddr_t hwaddr = page_translate_nocache(addr);
        insert_to_tlb(vpn, GET_PAGE_NUMBER_INPLACE(hwaddr));
        return hwaddr;
    }
}



// ================= NO TLB ===================

#else // no tlb
hwaddr_t page_translate(lnaddr_t addr)
{
    return page_translate_nocache(addr);
}
void flush_tlb()
{
}
void show_tlb()
{
    printf("%s  no tlb%s\n", c_red, c_normal);
}
#endif

