#include "cpu/exec/helper.h"
#include "device/port-io.h"

make_helper(outb) {
	print_asm("out    %%al,(%%dx)");
	pio_write(cpu.DX, 1, (uint32_t) cpu.AL);
	return 1;
}

make_helper(outl) {
    assert(ops_decoded.is_data_size_16 == 0);
	print_asm("out    %%eax,(%%dx)");
	pio_write(cpu.DX, 4, cpu.EAX);
	return 1;
}

