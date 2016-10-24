#include "monitor/monitor.h"
#include "cpu/helper.h"
#include "device/i8259.h"
#include <setjmp.h>
#include <time.h>

/* The assembly code of instructions executed is only output to the screen
 * when the number of instructions executed is less than this value.
 * This is useful when you use the ``si'' command.
 * You can modify this value as you want.
 */
#define MAX_INSTR_TO_PRINT 10

int nemu_state = STOP;

int exec(swaddr_t);

char assembly[80];
char asm_buf[128];

/* Used with exception handling. */
jmp_buf jbuf;

void print_bin_instr(swaddr_t eip, int len) {
	int i;
	int l = sprintf(asm_buf, "%8x:   ", eip);
	for(i = 0; i < len; i ++) {
		l += sprintf(asm_buf + l, "%02x ", instr_fetch(eip + i, 1));
	}
	sprintf(asm_buf + l, "%*.s", 50 - (12 + 3 * len), "");
}

/* This function will be called when an `int3' instruction is being executed. */
void do_int3() {
	printf("\nHit breakpoint at eip = 0x%08x\n", cpu.eip);
	nemu_state = STOP;
    cpu.EIP++;
    longjmp(jbuf, 4);
}




extern int show_as_code(int base, int range);
extern int txt_line_cnt;




// MUST: SHOW_FREQ % DEV_CHECK_FREQ == 0
#define SHOW_FREQ (1 << 26)
#define DEV_CHECK_FREQ (1 << 15)


/* halt instr loop count */
long long hlt_cnt = 0;
static long long last_hlt_cnt = 0;

/* execute instr count */
// executed_instr_count is in CPUstate
static long long last_instr_count;
static clock_t last_clock;


/* Simulate how the CPU works. */
void cpu_exec(uint32_t n32) {
    volatile unsigned long long n = (long long) (int) n32;
    last_hlt_cnt = 0;
    cpu.executed_instr_count = 0;
    last_instr_count = -1;

    assert(SHOW_FREQ % DEV_CHECK_FREQ == 0);
#ifdef UI_FEATURE_NEXT_INSTRUCTION
    // n == 0 : 'ni' command
    // n == -1 : 'c' command
    // n == somenumber : 'si' command
    static uint32_t next_instr_addr = 0; // used by 'ni' command
    static uint32_t ni_stop_at = 0; // used by 'ni' command
    int ni_flag = 0;
    if (n == 0) {
        int next_opcode = instr_fetch(cpu.EIP, 1);
        switch (next_opcode) {
            case 0xe8: // call
                ni_flag = 1;
                n = -1;
                break;
            default:
                n = 1; // 'ni' is same as 'si' when other instructions
        }
    }
#endif

	if(nemu_state == END) {
		printf(c_red "  Program execution has ended. To restart the program, exit NEMU and run again.\n" c_normal);
		return;
	}
	nemu_state = RUNNING;

#ifdef DEBUG
	uint32_t n_temp = n;
	int le_flag = 0;
#endif

    cpu.executed_instr_count = last_instr_count = DEV_CHECK_FREQ - 1;



	volatile int jval = setjmp(jbuf);
    if (unlikely(jval != 0 && jval != 1)) { // if not caused by interrupt
        if (jval == 2) {
            // panic to ui
            printf("%s  NEMU panic%s\n", c_red, c_normal);
            nemu_state = END;
        } else if (jval == 3) {
            // hit nemu trap
            nemu_state = END;
        }
        return;
    }

    //if (unlikely(nemu_state != RUNNING)) { return; }

    cpu.protect_eip = 0;

#ifdef DEBUG
	for(; n > 0 && nemu_state == RUNNING; n --) {
#else
    while (1) {
#endif


#ifdef DEBUG
		swaddr_t eip_temp = cpu.eip;
		if((n & 0xffff) == 0) {
			/* Output some dots while executing the program. */
			fputc('.', stderr);
		}
#endif

#ifdef UI_FEATURE_NEXT_INSTRUCTION
        /* check for next_instr_addr */
        if (cpu.eip == ni_stop_at && ni_flag == 2) {
            //printf(c_blue "  stopped before executing instruction at " c_bold c_red "0x%08X" c_normal "\n", cpu.eip);
            ni_stop_at = next_instr_addr;
            
            break;
        }
#endif

#ifdef REG_DIFF_HIGHLIGHT
        /* save last cpustate */
        old_cpu = cpu;
#endif
        
		/* Execute one instruction, including instruction fetch,
		 * instruction decode, and the actual execution. */
#ifdef UI_FEATURE_NEXT_INSTRUCTION
		next_instr_addr = cpu.eip;
#endif


#ifdef UI_FEATURE_PERF
        // do perf counting
		void perf_record_eip();
		perf_record_eip();
#endif
		
		cpu.executed_instr_count++;
		
		assert(cpu.protect_eip == 0);
		
		int instr_len = exec(cpu.eip); // NOTE: this function might not return!
        
    	if (unlikely(cpu.protect_eip == 0)) {
    	    // normal
    	    cpu.eip += instr_len;
    	} else {
    	    if (!(cpu.protect_eip & 1)) {
    	        cpu.eip += instr_len;
    	    }
    	    if ((cpu.protect_eip & 2)) {
        	    extern void push_vm_stack_dword(uint32_t val);
        	    push_vm_stack_dword(cpu.orig_eip + instr_len);
    	    }
    	    cpu.protect_eip = 0;
    	}

    	

#ifdef UI_FEATURE_NEXT_INSTRUCTION
        next_instr_addr += instr_len;
    	if (ni_flag == 1) {
    	    ni_flag = 2;
            ni_stop_at = next_instr_addr;
        }
#endif

#ifdef UI_FEATURE_WATCHPOINT
        extern int eval_wp();
		int wp_flag = eval_wp();
		if (wp_flag) {
		    n = 1;
#ifdef DEBUG
		    n_temp = 1;
#endif
		}
#endif

#ifdef DEBUG
		print_bin_instr(eip_temp, instr_len);
		strcat(asm_buf, assembly);
		Log_write("%s\n", asm_buf);
		if(n_temp <= MAX_INSTR_TO_PRINT) {
			if (!le_flag) {
		        puts(c_green " latest execution:" c_normal);
		        le_flag = 1;
		    }
			printf("%s\n", asm_buf);
			/*if (n == 1) {
			    print_bin_instr(cpu.eip, instr_len); // BUG HERE
		        strcat(asm_buf, assembly);
		        //puts(c_red "NEXT INSTRUCTION:" c_normal );
		        printf("%s%s%s\n", c_red, asm_buf, c_normal);
		    }*/
		}
#endif
#ifdef UI_FEATURE_WATCHPOINT
        if (wp_flag) {
            printf(c_green  " watchpoint hit:\n" c_normal);
            extern void info_wp();
            info_wp();
        }
#endif

		if (unlikely(cpu.executed_instr_count % DEV_CHECK_FREQ == 0)) {
		    // combine this two compare for speed
			if (unlikely(cpu.executed_instr_count % SHOW_FREQ == 0)) {
	            clock_t cur_clock = clock();
	            if (last_instr_count >= 0) {
        	        double diff = (double) (cur_clock - last_clock) / CLOCKS_PER_SEC;
        	        long long instr_diff = cpu.executed_instr_count - last_instr_count;
        	        long long hlt_diff = hlt_cnt - last_hlt_cnt;
        	        extern void print_current_time();
		            printf("%s", c_green);
		            print_current_time();
		            printf("%s", c_normal);
		            
		            extern double tot_timer_jitter;
                    extern double tot_timer_cnt;
                    double avg_jitter = tot_timer_jitter / tot_timer_cnt;
                    double avg_jitter_percent = avg_jitter / (1000 / TIMER_HZ) * 100;
                    tot_timer_jitter = 0;
                    tot_timer_cnt = 0;
                    
		            extern char *get_func_name(uint32_t loc, uint32_t *st, uint32_t *off);
		            uint32_t off;
                    const char *fname = get_func_name(cpu.EIP, NULL, &off);
                    if (!fname) { off = 0; fname = "?"; }
                    //double devfreq = (instr_diff / diff) / DEV_CHECK_FREQ;
		            printf("speed: %s%.2f%s instrs/sec", c_purple, instr_diff / diff, c_normal);
		            printf(", %s%.2f%s hlts/sec", c_purple, hlt_diff / diff, c_normal);
		            printf(", jitter = %s%.2f%s ms (%s%.2f%%%s)", c_purple, avg_jitter, c_normal, c_purple, avg_jitter_percent, c_normal);
		            //printf("(devfreq = %s%.2f%s hz)", c_purple, devfreq, c_normal);
		            printf(", eip = %s%s+%04X%s\n", c_purple, fname, off, c_normal);
		            
		            // NOTE:
		            // DEV_CHECK_FREQ should be lower enough
		            // or the jitter of device will be too large
		            //if (avg_jitter_percent > 10) {
		            //    printf("%s%s  jitter is too high%s\n", c_red, c_bold, c_normal);
		            //}
		            
        	    }
        	    
        	    last_instr_count = cpu.executed_instr_count;
	            last_clock = cur_clock;
	            last_hlt_cnt = hlt_cnt;
	        }
	    
    		extern void check_device_update();
	    	check_device_update();
	    	
	    	if (cpu.executed_instr_count >= n) {
	    	    break;
	    	}
	    }
	    
		if (unlikely(cpu.INTR) && likely(cpu.EFLAGS.IF)) {
	        uint32_t intr_no = i8259_query_intr();
	        i8259_ack_intr();
	        
	        extern void raise_intr(uint8_t NO);
	        raise_intr(intr_no);
        }
	}

	if(nemu_state == RUNNING) { nemu_state = STOP; }
	
#ifndef DEBUG
    if (1) {
        if (txt_line_cnt != 0) {
            puts(c_green " next execution:" c_normal);
            show_as_code(0, 0);
        }
    }
#endif
}
