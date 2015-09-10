#include "irq.h"

#include <sys/syscall.h>

void add_irq_handle(int, void (*)(void));
void mm_brk(uint32_t);

static void sys_brk(TrapFrame *tf) {
#ifdef IA32_PAGE
	mm_brk(tf->ebx);
#endif
	tf->eax = 0;
}


extern int fs_open(const char *pathname, int flags);
extern int fs_read(int fd, void *buf, int len);
extern int fs_write(int fd, void *buf, int len);
extern int fs_lseek(int fd, int offset, int whence);
extern int fs_close(int fd);


void do_syscall(TrapFrame *tf) {
	switch(tf->eax) {
		/* The ``add_irq_handle'' system call is artificial. We use it to 
		 * let user program register its interrupt handlers. But this is 
		 * very dangerous in a real operating system. Therefore such a 
		 * system call never exists in GNU/Linux.
		 */
		case 0: 
			cli();
			add_irq_handle(tf->ebx, (void*)tf->ecx);
			sti();
			break;

		case SYS_brk: sys_brk(tf); break;

		/* TODO: Add more system calls. */
		
		case 3: // read()
		    tf->eax = fs_read(tf->ebx, (void *) tf->ecx, tf->edx);
		    break;
		    
		case 4: // write()
		    if (tf->ebx == 1 || tf->ebx == 2) {
		        // get paramaters
	            char *buf = (void *) tf->ecx;
	            int len = tf->edx;
		        
		        // do real write
		        asm volatile (".byte 0xd6" : : "a"(2), "c"(buf), "d"(len));
		        
		        // set return value (number of bytes written)
		        tf->eax = len;
		    } else {
		        tf->eax = fs_write(tf->ebx, (void *) tf->ecx, tf->edx);
		    }
		    break;
        
        case 5: // open()
            tf->eax = fs_open((void *) tf->ebx, tf->ecx);
            break;
            
        case 6: // close()
            tf->eax = fs_close(tf->ebx);
            break;
        
        case 19: // lseek()
            tf->eax = fs_lseek(tf->ebx, tf->ecx, tf->edx);
            break;
        
		default: panic("Unhandled system call: id = %d", tf->eax);
	}
}

