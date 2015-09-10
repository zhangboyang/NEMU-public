#include "common.h"
#include <stdlib.h>
#include <elf.h>

char *exec_file = NULL;

static char *strtab = NULL;
static Elf32_Sym *symtab = NULL;
static int nr_symtab_entry;

void enum_functions(void (*callback)(const char *name, unsigned start, unsigned len))
{
    int i;
    for (i = 0; i < nr_symtab_entry; i++) {
        const char *sym_name = symtab[i].st_name ? strtab + symtab[i].st_name : NULL;
        uint32_t sym_size = symtab[i].st_size;
        uint32_t sym_val = symtab[i].st_value;
        if (ELF32_ST_TYPE(symtab[i].st_info) == STT_FUNC) {
            callback(sym_name, sym_val, sym_size);
        }
    }
}

void show_elf_symbols() /* extract symbols from ELF file */
{
    int i;
    printf(c_yellow "  %8s %4s %-7s %s" c_normal "\n", "value", "size", "type", "name");
    for (i = 0; i < nr_symtab_entry; i++) {
        const char *sym_name = symtab[i].st_name ? strtab + symtab[i].st_name : NULL;
        const char *sym_type;
        const char *sym_color;
        switch (ELF32_ST_TYPE(symtab[i].st_info)) {
            case STT_NOTYPE: sym_type = "NOTYPE"; sym_color = c_normal; break;
            case STT_FILE  : sym_type = "FILE"; sym_color = c_normal; break;
            case STT_OBJECT: sym_type = "OBJECT"; sym_color = c_green; break;
            case STT_FUNC  : sym_type = "FUNC"; sym_color = c_purple; break;
            default: sym_type = "UNKNOWN"; sym_color = c_normal; break;
        }
        uint32_t sym_size = symtab[i].st_size;
        uint32_t sym_val = symtab[i].st_value;
        if (sym_name)
            printf("%s  %08x %04u %-7s %s" c_normal "\n", sym_color, sym_val, sym_size, sym_type, sym_name);
    }
}

char *get_func_name(uint32_t loc, uint32_t *st, uint32_t *off)
{
    int i;
    for (i = 0; i < nr_symtab_entry; i++) {
        if (ELF32_ST_TYPE(symtab[i].st_info) == STT_FUNC) {
            char *sym_name = symtab[i].st_name ? strtab + symtab[i].st_name : NULL;
            uint32_t sym_size = symtab[i].st_size;
            uint32_t sym_val = symtab[i].st_value;
            if (sym_val <= loc && loc < sym_val + sym_size) {
                if (st) *st = sym_val;
                if (off) *off = loc - sym_val;
                return sym_name;
            }
        }
    }
    return NULL;
}

uint32_t *get_symval_addr(char *sym_name, int sym_name_len) /* get val addr for expr() */
{
    /* Elf32_Addr must equal to uint32_t
       since we use pointers without converting values
       see below
    */
    assert(sizeof(Elf32_Addr) == sizeof(uint32_t));
    
    int i;
    for (i = 0; i < nr_symtab_entry; i++) {
        const char *cur_sym_name = symtab[i].st_name ? strtab + symtab[i].st_name : NULL;
        uint32_t *cur_sym_val_addr = (uint32_t *) &symtab[i].st_value; // here
        if (cur_sym_name) {
            if (strncmp(cur_sym_name, sym_name, sym_name_len) == 0 && cur_sym_name[sym_name_len] == '\0')
                return cur_sym_val_addr;
        }
    }
    
    return NULL;
}

void load_elf_tables(int argc, char *argv[]) {
	int ret;
	Assert(argc == 2, "run NEMU with format 'nemu [program]'");
	exec_file = argv[1];

	FILE *fp = fopen(exec_file, "rb");
	Assert(fp, "Can not open '%s'", exec_file);

	uint8_t buf[sizeof(Elf32_Ehdr)];
	ret = fread(buf, sizeof(Elf32_Ehdr), 1, fp);
	assert(ret == 1); FORCE_USE(ret);

	/* The first several bytes contain the ELF header. */
	Elf32_Ehdr *elf = (void *)buf;
	char magic[] = {ELFMAG0, ELFMAG1, ELFMAG2, ELFMAG3}; FORCE_USE(magic);

	/* Check ELF header */
	assert(memcmp(elf->e_ident, magic, 4) == 0);		// magic number
	assert(elf->e_ident[EI_CLASS] == ELFCLASS32);		// 32-bit architecture
	assert(elf->e_ident[EI_DATA] == ELFDATA2LSB);		// littel-endian
	assert(elf->e_ident[EI_VERSION] == EV_CURRENT);		// current version
	assert(elf->e_ident[EI_OSABI] == ELFOSABI_SYSV || 	// UNIX System V ABI
			elf->e_ident[EI_OSABI] == ELFOSABI_LINUX); 	// UNIX - GNU
	assert(elf->e_ident[EI_ABIVERSION] == 0);			// should be 0
	assert(elf->e_type == ET_EXEC);						// executable file
	assert(elf->e_machine == EM_386);					// Intel 80386 architecture
	assert(elf->e_version == EV_CURRENT);				// current version


	/* Load symbol table and string table for future use */

	/* Load section header table */
	uint32_t sh_size = elf->e_shentsize * elf->e_shnum;
	Elf32_Shdr *sh = malloc(sh_size);
	fseek(fp, elf->e_shoff, SEEK_SET);
	ret = fread(sh, sh_size, 1, fp);
	assert(ret == 1);

	/* Load section header string table */
	char *shstrtab = malloc(sh[elf->e_shstrndx].sh_size);
	fseek(fp, sh[elf->e_shstrndx].sh_offset, SEEK_SET);
	ret = fread(shstrtab, sh[elf->e_shstrndx].sh_size, 1, fp);
	assert(ret == 1);

	int i;
	for(i = 0; i < elf->e_shnum; i ++) {
		if(sh[i].sh_type == SHT_SYMTAB && 
				strcmp(shstrtab + sh[i].sh_name, ".symtab") == 0) {
			/* Load symbol table from exec_file */
			symtab = malloc(sh[i].sh_size);
			fseek(fp, sh[i].sh_offset, SEEK_SET);
			ret = fread(symtab, sh[i].sh_size, 1, fp);
			assert(ret == 1);
			nr_symtab_entry = sh[i].sh_size / sizeof(symtab[0]);
		}
		else if(sh[i].sh_type == SHT_STRTAB && 
				strcmp(shstrtab + sh[i].sh_name, ".strtab") == 0) {
			/* Load string table from exec_file */
			strtab = malloc(sh[i].sh_size);
			fseek(fp, sh[i].sh_offset, SEEK_SET);
			ret = fread(strtab, sh[i].sh_size, 1, fp);
			assert(ret == 1);
		}
	}

	free(sh);
	free(shstrtab);

	assert(strtab != NULL && symtab != NULL);

	fclose(fp);
}

