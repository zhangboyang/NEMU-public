#include "common.h"
#include "cpu/reg.h"
#include "memory/memory.h"

extern int safe_read_flag;
extern int safe_read_failed;


lnaddr_t seg_translate(swaddr_t addr, size_t len, uint8_t sreg)
{
    if (!cpu.PE) return addr;
    uint32_t base = cpu.seg_base[sreg];
#ifdef DEBUG
    uint32_t limit = cpu.seg_limit[sreg];
    if (addr > limit || addr + len - 1 > limit) {
        if (safe_read_flag) safe_read_failed = 1;
        else panic("can't do segment-translation");
    }
#endif
    return base + addr;
}



void load_segment(int sreg_index, int selector)
{
	int selector_index = selector >> 3;
	unsigned long long desc;
	uint32_t desc_data[2];
	desc_data[0] = lnaddr_read(cpu.GDTR + selector_index * 8, 4); // little endian
	desc_data[1] = lnaddr_read(cpu.GDTR + selector_index * 8 + 4, 4);
	memcpy(&desc, desc_data, 8);
	
	//printf("segment desc: %016llX\n", desc);
	
	cpu.seg_reg[sreg_index] = selector;
	cpu.seg_limit[sreg_index] = GET_SEG_LIMIT(desc);
	cpu.seg_base[sreg_index] = GET_SEG_BASE(desc);
	
	//printf("segment %s set to %04X B:%08X L:%08X\n", seg_regs[sreg_index], cpu.seg_reg[sreg_index], cpu.seg_base[sreg_index], cpu.seg_limit[sreg_index]);

    // check for flat mode
    int i;
    cpu.seg_is_flat = 1;
    for (i = 0; i < 6; i++)
        if (cpu.seg_base[i] != 0 || cpu.seg_limit[i] != 0xffffffff) {
            cpu.seg_is_flat = 0;
            break;
        }
#ifdef DEBUG
    if (!SEG_PRESENT(desc)) {
        if (safe_read_flag) safe_read_failed = 1;
        else panic("can't load segment that not present");
    }
#endif

}

