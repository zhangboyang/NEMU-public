#include "common.h"
#include "memory.h"
#include <string.h>
#include <elf.h>

#define ELF_OFFSET_IN_DISK 0

#ifdef HAS_DEVICE
void ide_read(uint8_t *, uint32_t, uint32_t);
#else
void ramdisk_read(uint8_t *, uint32_t, uint32_t);
#endif

#define STACK_SIZE (1 << 20)

void create_video_mapping();
uint32_t get_ucr3();


/*
static void alloc_virtual_memory(void *dest, int len)
{
    printk("alloc dest=%08x len=%08x\n", (unsigned) dest, len);
    int page_cnt = len >> PAGE_SHIFT;
    if ((len & ((1 << PAGE_SHIFT) - 1))) page_cnt++;
    unsigned destaddr = (unsigned) dest;
    unsigned physaddr;
    destaddr = ROUND_TO_PAGE(destaddr);
    int i;
    //printk("page_cnt=%d\n", page_cnt);
    for (i = 0; i < page_cnt; i++) {
        physaddr = mm_malloc(destaddr, PAGE_SIZE);
        printk("alloc one page: physaddr=%08x destaddr=%08x size=%08x\n", physaddr, destaddr, PAGE_SIZE);
        destaddr += PAGE_SIZE;
    }
}*/

static unsigned alloc_one_clean_page(unsigned virtaddr)
{
    unsigned physaddr = mm_malloc(virtaddr, PAGE_SIZE);
    //static unsigned myaddr = 0xf0000000; if (myaddr < 0xf0ff0000) { mm_malloc(myaddr, PAGE_SIZE); myaddr += PAGE_SIZE; } // use this to test cross-page read, 15MB
    //printk("%08X ===> %08X\n", virtaddr, physaddr);
    memset(PADDR_TO_KADDR(physaddr), 0, PAGE_SIZE);
    return physaddr;
}

/* read elf data and store it in memory */
static void read_from_elf(void *dest, size_t offset, size_t size)
{
    /* ELF is stored at 0x00000000 */
    //memcpy(dest, (void *) offset, size);
    //ramdisk_read(dest, offset, size);
#ifdef HAS_DEVICE
    ide_read(dest, offset, size);
#else
    ramdisk_read(dest, offset, size);
#endif
}

static void load_segment_from_elf(unsigned destmem, size_t memsz, size_t elfoff, size_t filesz)
{
    printk("load segment from elf: dest=0x%x memsz=0x%x elfoff=0x%x filesz=0x%x\n", destmem, memsz, elfoff, filesz);

    int pgoffset = GET_PAGE_OFFSET(destmem);
    memsz += pgoffset;
    destmem = ROUND_TO_PAGE(destmem);
    //int cur_page = 0;
    //int tot_page = filesz / PAGE_SIZE + !!(filesz % PAGE_SIZE);
    char *dest;    
    int cur_size;
    while (filesz > 0) {
        cur_size = (PAGE_SIZE - pgoffset) < filesz ? (PAGE_SIZE - pgoffset) : filesz;
        
        dest = PADDR_TO_KADDR(alloc_one_clean_page(destmem)); // new page should zero-filled
        
        //printk("reading page %d / %d\n", ++cur_page, tot_page);
        
        read_from_elf(dest + pgoffset, elfoff, cur_size);
        pgoffset = 0;
        elfoff += cur_size;
        
        destmem += PAGE_SIZE;
        memsz -= cur_size;
        filesz -= cur_size;
    }
    
    printk("clearing memory ...\n");
    while (memsz > 0) {
        cur_size = PAGE_SIZE < memsz ? PAGE_SIZE : memsz;
        alloc_one_clean_page(destmem);
        destmem += PAGE_SIZE;
        memsz -= cur_size;
    }
}

uint32_t loader() {
//	Elf32_Ehdr *elf;
//	Elf32_Phdr *ph = NULL;

	uint8_t buf[4096];

#ifdef HAS_DEVICE
	ide_read(buf, ELF_OFFSET_IN_DISK, 4096);
#else
	ramdisk_read(buf, ELF_OFFSET_IN_DISK, 4096);
#endif

//	elf = (void*)buf;

    int i;
/*    for (i = 0; i < 4096; i++) {
        printk("%02x ", (unsigned) buf[i]);
        if (i % 16 == 15) printk("\n");
    }*/
	/* DONE: fix the magic number with the correct one */
	const uint32_t elf_magic = 0x464c457f;
	uint32_t *p_magic = (void *)buf;
	nemu_assert(*p_magic == elf_magic);

    
    /* load ELF header */
    static Elf32_Ehdr ehdr;
    static Elf32_Phdr phdr;
    
    read_from_elf(&ehdr, 0, sizeof(ehdr));
    printk("loading user program ...\n");
    printk("phoff=0x%x phcnt=0x%x phsize=0x%x\n", ehdr.e_phoff, ehdr.e_phnum, ehdr.e_phentsize);
    nemu_assert(ehdr.e_phentsize == sizeof(phdr));
    
    extern uint32_t brk;
	/* Load each program segment */
	
	for(i = 0; i < ehdr.e_phnum; i++) {
	    read_from_elf(&phdr, ehdr.e_phoff + ehdr.e_phentsize * i, ehdr.e_phentsize);
	    
	    //printk("ph: type=%d off=%d vaddr=%d filesz=%d memsz=%d\n", phdr.p_type, phdr.p_offset, phdr.p_vaddr, phdr.p_filesz, phdr.p_memsz);
	               
		/* Scan the program header table, load each segment into memory */
		if (phdr.p_type == PT_LOAD) {
			load_segment_from_elf(phdr.p_vaddr, phdr.p_memsz, phdr.p_offset, phdr.p_filesz);
#ifdef IA32_PAGE
			/* Record the program break for future use. */
			
			uint32_t new_brk = ROUND_TO_PAGE(phdr.p_vaddr + phdr.p_memsz - 1);
			if(brk < new_brk) { brk = new_brk; }
#endif
		}
	}
    
	volatile uint32_t entry = ehdr.e_entry;

#ifdef IA32_PAGE
	mm_malloc(KOFFSET - STACK_SIZE, STACK_SIZE);

#ifdef HAS_DEVICE
	create_video_mapping();
#endif

	write_cr3(get_ucr3());
#endif

	return entry;
}
