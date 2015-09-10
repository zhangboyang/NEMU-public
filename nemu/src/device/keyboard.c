#include "device/port-io.h"
#include "device/i8259.h"
#include "monitor/monitor.h"

#define I8042_DATA_PORT 0x60
#define KEYBOARD_IRQ 1

static uint8_t *i8042_data_port_base;
static bool newkey;

void keyboard_intr(uint8_t scancode) {
	if(nemu_state == RUNNING && newkey == false) {
		i8042_data_port_base[0] = scancode;
		//void print_current_time(); print_current_time(); printf("  raise keyboard intr! scancode = %d\n", (int) scancode);
		i8259_raise_intr(KEYBOARD_IRQ);
		newkey = true;
	} else {
	    void print_current_time(); print_current_time();
	    printf("%s%s  intr not sent! scancode = %d%s\n", c_bold, c_red, (int) scancode, c_normal);
	}
}

void i8042_io_handler(ioaddr_t addr, size_t len, bool is_write) {
	if(!is_write) {
		newkey = false;
	}
}

void init_i8042() {
	i8042_data_port_base = add_pio_map(I8042_DATA_PORT, 1, i8042_io_handler);
	newkey = false;
}

