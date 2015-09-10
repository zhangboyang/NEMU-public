#include "nemu.h"

#define ENTRY_START 0x100000

extern uint8_t entry [];
extern uint32_t entry_len;
extern char *exec_file;

void load_elf_tables(int, char *[]);
void init_regex();
void init_wp_list();
void init_ddr3();
void init_device();
void init_sdl();
void init_mmio_map();
void flush_tlb();

FILE *log_fp = NULL;

static void init_log() {
	log_fp = fopen("log.txt", "w");
	Assert(log_fp, "Can not open 'log.txt'");
}

static void welcome() {
	printf("Welcome to NEMU!\n");
	printf("The executable is " c_purple "%s" c_normal ".\n", exec_file);
	printf("For help, type \"help\"\n");
}

void init_monitor(int argc, char *argv[]) {
	/* Perform some global initialization */

	/* Open the log file. */
	init_log();

	/* Load the string table and symbol table from the ELF file for future use. */
	load_elf_tables(argc, argv);

	/* Compile the regular expressions. */
	init_regex();

	/* Initialize the watchpoint link list. */
	init_wp_list();

	/* Display welcome message. */
	welcome();
	
	/* PA4: optimized mmip map need initialize */
    init_mmio_map();
    
	/* Init device and sdl */
	init_device();
    init_sdl();
}

#ifdef USE_RAMDISK
static void init_ramdisk() {
	int ret;
	const int ramdisk_max_size = 0xa0000;
	FILE *fp = fopen(exec_file, "rb");
	Assert(fp, "Can not open '%s'", exec_file);

	fseek(fp, 0, SEEK_END);
	size_t file_size = ftell(fp);
	Assert(file_size < ramdisk_max_size, "file size(%zd) too large", file_size);

	fseek(fp, 0, SEEK_SET);
	ret = fread(hwa_to_va(0), file_size, 1, fp);
	assert(ret == 1);
	fclose(fp);
}
#endif

static void load_entry() {
	int ret;
	FILE *fp = fopen("entry", "rb");
	Assert(fp, "Can not open 'entry'");

	fseek(fp, 0, SEEK_END);
	size_t file_size = ftell(fp);

	fseek(fp, 0, SEEK_SET);
	ret = fread(hwa_to_va(ENTRY_START), file_size, 1, fp);
	assert(ret == 1); FORCE_USE(ret);
	fclose(fp);
}

void restart() {
	/* Perform some initialization to restart a program */
	/* Initialize DRAM. */
	init_ddr3();
	
#ifdef USE_RAMDISK
	/* Read the file with name `argv[1]' into ramdisk. */
	init_ramdisk();
#endif

	/* Read the entry code into memory. */
	load_entry();

	/* Set the initial instruction pointer. */
	cpu.eip = ENTRY_START;

    /* PA2: init EFLAGS */
    cpu.EFLAGS.EFLAGS = 0x2;
    #ifdef INVF_CHECK
    cpu.EFLAGS_INVALID.EFLAGS = 0;
    #endif
    
    /* PA3: init CR0 */
    cpu.CR0 = 0;
    
    /* PA3: init segment registers */
    memset(cpu.seg_reg, 0, sizeof(cpu.seg_reg));
    memset(cpu.seg_base, 0, sizeof(cpu.seg_base));
    memset(cpu.seg_limit, -1, sizeof(cpu.seg_limit));

    /* PA3: flush tlb */
    flush_tlb();
	
	#ifdef USE_MEMORY_CACHE
	/* Init L1 Cache */
	extern void l1cache_init();
	l1cache_init();
	/* Init L2 Cache */
	extern void l2cache_init();
	l2cache_init();
	#endif
}
