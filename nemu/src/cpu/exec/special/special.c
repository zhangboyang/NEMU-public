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
    panic_to_ui("invalid instruction");
}

static void putchar_to_buffer(int ch)
{
    static char buf[4096];
    static int len;
    buf[len++] = ch;
    if (ch == '\n' || ch == '\0' || len == sizeof(buf)) {
        printf("%s", c_yellow);
		extern void print_current_time();
        print_current_time();
        printf("%s", c_normal);
        printf("%s", c_blue c_bold);
        printf("stdout: ");
        printf("%s", c_normal);
        printf("%.*s", len, buf);
        if (buf[len - 1] != '\n') putchar('\n');
        len = 0;
    }
}

static int getchar_from_buffer()
{
    static char buf[4096] = {};
    static int ptr = 0;
    if (buf[ptr] == '\0') {
        printf("%s", c_yellow);
        extern void print_current_time();
        print_current_time();
        printf("%s", c_normal);
        printf("%s", c_purple c_bold);
        printf("stdin (input): ");
        printf("%s", c_normal);
        if (!fgets(buf, sizeof(buf), stdin)) {
            buf[ptr = 0] = 0;
            return -1;
        }
        ptr = 0;
    }
    return (unsigned) (unsigned char) buf[ptr++];
}

make_helper(nemu_trap) {
	print_asm("nemu trap (eax = %d)", cpu.eax);

	switch(cpu.eax) {
		case 2: // output string
		    do {
		        uint32_t str = cpu.ecx;
		        uint32_t len = cpu.edx;
		        int ch;
                while (len--) {
                    ch = swaddr_read(str, 1, R_DS);
                    if (ch == 0) break;
                    putchar_to_buffer(ch);
                    str++;
                }
            } while (0);
		   	break;
		
		case 3: // read from stdin
		    do {
		        int ret = 0;
		        uint32_t str = cpu.ecx;
		        uint32_t len = cpu.edx;
		        
		        while (len > 0) {
		            int ch = getchar_from_buffer();
		            if (ch < 0) break;
		            swaddr_write(str, 1, ch, R_DS);
		            str++;
		            ret++;
		            len--;
		            if (ch == '\n') break;
		        }
		        cpu.eax = ret;
		    } while (0);
		    break;

        case 100:
            do {
                extern int seg_gs_prepare_selector, seg_gs_prepare_base, seg_gs_prepare_limit;
                seg_gs_prepare_base = cpu.EBX;
                seg_gs_prepare_limit = cpu.ECX;
                seg_gs_prepare_selector = (cpu.EDX * 8 + 3) & 0xffff;

                printf("prepare gs segment with %04X B:%08X L:%08X\n",
                    seg_gs_prepare_selector, seg_gs_prepare_base, seg_gs_prepare_limit);

            } while (0);
            break;

		default:
			printf("\33[1;31mnemu: HIT %s TRAP\33[0m at eip = 0x%08x\n\n",
					(cpu.eax == 0 ? "GOOD" : "BAD"), cpu.eip);
            extern jmp_buf jbuf;
            longjmp(jbuf, 3);
	}

	return 1;
}

