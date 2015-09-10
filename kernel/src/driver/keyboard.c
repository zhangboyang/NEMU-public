#include "common.h"

#define I8042_DATA_PORT 0x60
#define KEYBOARD_IRQ 1


void add_irq_handle(int, void (*)(void));

void keyboard_intr()
{
    //__asm__ __volatile__ ("int3");
    printk("keyboard intr!\n");
    unsigned char x = in_byte(I8042_DATA_PORT);
    printk("key=%02X\n", (unsigned) x);
}

void init_keyboard()
{
    add_irq_handle(KEYBOARD_IRQ, keyboard_intr);
}
