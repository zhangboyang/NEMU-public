#include "cpu/exec/helper.h"
#include "device/port-io.h"

make_helper(inb) {
	print_asm("in    %%al,(%%dx)");
	cpu.AL = (uint8_t) pio_read(cpu.DX, 1);
	return 1;
}

make_helper(inl) {
    assert(ops_decoded.is_data_size_16 == 0);
	print_asm("in    %%eax,(%%dx)");
	cpu.EAX = pio_read(cpu.DX, 4);
	//printf("read long = %08x\n", cpu.EAX);
	return 1;
}

