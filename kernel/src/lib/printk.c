#include "common.h"
#include <stdio.h>

void serial_printc(char);

#ifndef HAS_DEVICE

void __attribute__((__noinline__)) 
printk(const char *ctl, ...) {
	static char buf[0x1000];
	void *args = (void **)&ctl + 1;
	int len = vsnprintf(buf, 256, ctl, args);
	nemu_assert(len < sizeof(buf));
	__asm__ __volatile__ (".byte 0xd5" :: "a"(100), "c"(buf));
	
}
#else


/* __attribute__((__noinline__))  here is to disable inlining for this function to avoid some optimization problems for gcc 4.7 */
void __attribute__((__noinline__)) 
printk(const char *ctl, ...) {
	static char buf[256];
	void *args = (void **)&ctl + 1;
	int len = vsnprintf(buf, 256, ctl, args);
	int i;
	for(i = 0; i < len; i ++) {
		serial_printc(buf[i]);
	}
}
#endif
