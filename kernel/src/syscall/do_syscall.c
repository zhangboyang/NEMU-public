#include "irq.h"

#include <string.h>
#include <sys/syscall.h>

void add_irq_handle(int, void (*)(void));
uint32_t mm_brk(uint32_t);

static void sys_brk(TrapFrame *tf) {
    tf->eax = 0;
#ifdef IA32_PAGE
    if (tf->ebx) {
    	mm_brk(tf->ebx);
    } else {
        tf->eax = mm_brk(0);
    }
#endif
    printk("brk(0x%08x) = 0x%08x\n", tf->ebx, tf->eax);
}


extern int fs_open(const char *pathname, int flags);
extern int fs_read(int fd, void *buf, int len);
extern int fs_write(int fd, void *buf, int len);
extern int fs_lseek(int fd, int offset, int whence);
extern int fs_close(int fd);

static void dump_tf(TrapFrame *tf)
{
    printk("=== trap frame dump ===\n");
    printk("EAX=%08x // syscall number\n", tf->eax);
    printk("EBX=%08x // syscall arg1\n", tf->ebx);
    printk("ECX=%08x // syscall arg2\n", tf->ecx);
    printk("EDX=%08x // stscall arg3\n", tf->edx);
    printk("ESI=%08x // syscall arg4\n", tf->esi);
    printk("EDI=%08x // syscall arg5\n", tf->edi);
    printk("EBP=%08x // syscall arg6\n", tf->ebp);
    printk("=== end ===\n");
}


void do_syscall(TrapFrame *tf) {
	switch(tf->eax) {
		/* The ``add_irq_handle'' system call is artificial. We use it to 
		 * let user program register its interrupt handlers. But this is 
		 * very dangerous in a real operating system. Therefore such a 
		 * system call never exists in GNU/Linux.
		 */
		case 0: 
            printk("add_irq_handle(0x%08x, 0x%08x)\n", tf->ebx, tf->ecx);
			cli();
			add_irq_handle(tf->ebx, (void*)tf->ecx);
			sti();
			break;

		case SYS_brk:
            sys_brk(tf);
            break;

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

        case 54: // ioctl
            if (tf->ecx == 0x5413) { // TIOCGWINSZ
                const static short data[] = {25, 80, 0, 0};
                memcpy((void *) tf->edx, data, sizeof(data));
                tf->eax = 0;
            } else {
                dump_tf(tf);
                panic("unknown ioctl()");
            }
            printk("ioctl(0x%08x, 0x%08x, 0x%08x, ...) = 0x%08x\n", tf->ebx, tf->ecx, tf->edx, tf->eax);
            break;
        
        case 140: // llseek, used by musl libc
            do {
                if (tf->ebx == 0) { // llseek stdin, fail with ESPIPE
                    tf->eax = -29;
                } else { // llseek system
                    if (tf->ecx == (((int) tf->edx) >> 31)) {
                        *(unsigned long long*) tf->esi = fs_lseek(tf->ebx, tf->edx, tf->edi);
                        tf->eax = 0;
                    } else {
                        dump_tf(tf);
                        panic("offset overflow!");
                    }
                }
            } while (0);
            break;

        case 145: // readv, used by musl libc
            do {
                int i;
                struct {
                    void *base;
                    size_t len;
                } *iov = (void *) tf->ecx;
                tf->eax = 0;
                for (i = 0; i < tf->edx; i++) {
                    if (tf->ebx == 0) { // read stdin
                        int x = 3;
                        asm volatile (".byte 0xd6" :"+a"(x) : "c"(iov[i].base), "d"(iov[i].len));
                        tf->eax += x;
                        if (x < iov[i].len) break;
                    } else { // read file system
                        tf->eax += fs_read(tf->ebx, iov[i].base, iov[i].len);
                    }
                }
            } while (0);
            //printk("readv() fd = %d, return %d\n", tf->ebx, tf->eax);
            break;

        case 146: // writev, used by musl libc
            if (tf->ebx == 1 || tf->ebx == 2) {
                int i;
                struct {
                    void *base;
                    size_t len;
                } *iov = (void *) tf->ecx;
                tf->eax = 0;
                for (i = 0; i < tf->edx; i++) {
		            asm volatile (".byte 0xd6" : : "a"(2), "c"(iov[i].base), "d"(iov[i].len));
                    tf->eax += iov[i].len;
                }
            } else {
                dump_tf(tf);
                panic("writev() to fd = %d is not supported", tf->ebx);
            }
            break;
        
        case 91: // munmap, ignored
            tf->eax = 0;
            printk("munmap() ignored, return %d\n", tf->eax);
            break;

        case 192: // mmap2
            //  addr == NULL && MAP_ANONYMOUS  && OFFSET == 0
            if ((tf->ebx == 0) && (tf->esi | 32) != 0 && (tf->ebp == 0)) {
                extern unsigned mmap_anonymous(unsigned length);
                tf->eax = mmap_anonymous(tf->ecx);
                printk("mmap() with size 0x%08x = 0x%08x\n", tf->ecx, tf->eax);
            } else {
                dump_tf(tf);
                panic("unsupported mmap2() call");
            }
            break;

        case 219: // madvise
            if (tf->edx == 4) { // MADV_DONTNEED
                assert((tf->ebx & (PAGE_SIZE - 1)) == 0); // check alignment
                memset((void *)tf->ebx, 0, tf->ecx);
                tf->eax = 0;
            } else {
                dump_tf(tf);
                panic("unsupported madvise() call");
            }
            printk("madvise(0x%08x, 0x%08x, 0x%08x) = 0x%08x\n", tf->ebx, tf->ecx, tf->edx, tf->eax);
            break;

        case 243: // set_thread_area
            do {
                struct {
                    unsigned int  entry_number;
                    unsigned long base_addr;
                    unsigned int  limit;
                    unsigned int  seg_32bit:1;
                    unsigned int  contents:2;
                    unsigned int  read_exec_only:1;
                    unsigned int  limit_in_pages:1;
                    unsigned int  seg_not_present:1;
                    unsigned int  useable:1;
                } *desc = (void *) tf->ebx;
                
                if (desc->entry_number == -1 && desc->seg_32bit == 1 &&
                    desc->contents == 0 && desc->read_exec_only == 0 &&
                    desc->limit_in_pages == 1 &&
                    desc->seg_not_present == 0 &&
                    desc->useable == 1) {
                    
                    // this is the magic number
                    // we will use this magic number in 'seg.c' in nemu
                    // musl will load gs with 'magicnumber * 8 + 3'
                    desc->entry_number = 12;

                    // use nemu-trap to save desc
                    asm volatile (".byte 0xd6" : : "a"(100), "b"(desc->base_addr), "c"(desc->limit * PAGE_SIZE + PAGE_SIZE - 1), "d"(desc->entry_number));

                    tf->eax = 0;
                    printk("special patch for set_thread_area(), entry_number set to 0x%08x\n", desc->entry_number);
                } else {
                    dump_tf(tf);
                    panic("unsupported set_thread_area() call");
                }
            } while (0);
            break;

        case 258: // set_tid_address
            tf->eax = 0; // return value is thread id
            printk("set_tid_address() ignored, return %d\n", tf->eax);
            break;


        case 252: // exit_group
            printk("exit_group() with status %d\n", tf->ebx);
            HIT_GOOD_TRAP;
            break;
            

		default: 
            dump_tf(tf);
            panic("Unhandled system call: id = %d", tf->eax);
	}
}

