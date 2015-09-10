#include "cpu/exec/helper.h"
#include "monitor/monitor.h"

#include <stdlib.h>
#include <setjmp.h>

make_helper(inv) {
	/* invalid opcode */

	uint32_t temp[8];
	temp[0] = instr_fetch(eip, 4);
	temp[1] = instr_fetch(eip + 4, 4);

	uint8_t *p = (void *)temp;
	printf("invalid opcode(eip = 0x%08x): %02x %02x %02x %02x %02x %02x %02x %02x ...\n\n", 
			eip, p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7]);

	//extern char logo [];
	char mylogo[] = {
c_purple
"...........................................................\n"
"...........................................................\n"
"..########..#..............................................\n"
".........#..#...........................###........###.....\n"
"........#...#...........................###........###.....\n"
".......#....#...........................###........###.....\n"
"......#.....########..#......#.............................\n"
".....#......#......#..#......#.............................\n"
"....#.......#......#..#......#........##..............##...\n"
"...#........#......#..#......#.........##............##....\n"
"..########..########..########..........##..........##.....\n"
".............................#............##......##.......\n"
".............................#..............######.........\n"
"......................########.............................\n"
"...........................................................\n"
c_bold c_red
	};
	
	printf("There are two cases which will trigger this unexpected exception:\n\
1. The instruction at eip = 0x%08x is not implemented.\n\
2. Something is implemented incorrectly.\n", eip);
	printf("Find this eip value(0x%08x) in the disassembling result to distinguish which case it is.\n\n", eip);
	printf("\33[1;31mIf it is the first case, see\n%s\nfor more details.\n\nIf it is the second case, remember:\n\
* The machine is always right!\n\
* Every line of untested code is always wrong!\33[0m\n\n", mylogo);

	nemu_state = END;
	extern jmp_buf jbuf;
    longjmp(jbuf, 1);
}

make_helper(nemu_trap) {
	print_asm("nemu trap (eax = %d)", cpu.eax);

	switch(cpu.eax) {
		case 2:
		    do {
		        uint32_t str = cpu.ecx;
		        uint32_t len = cpu.edx;
		        int ch;
		        extern void print_current_time();
		        printf("%s", c_yellow);
		        print_current_time();
		        printf("%s", c_normal);
                printf("TRAP: ");
                while (len--) {
                    ch = swaddr_read(str, 1, R_DS);
                    if (ch == 0) break;
                    putchar(ch);
                    str++;
                }
            } while (0);
		   	break;

		default:
			printf("\33[1;31mnemu: HIT %s TRAP\33[0m at eip = 0x%08x\n\n",
					(cpu.eax == 0 ? "GOOD" : "BAD"), cpu.eip);
			nemu_state = END;
			extern jmp_buf jbuf;
		    longjmp(jbuf, 1);
	}

	return 1;
}

