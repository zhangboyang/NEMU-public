#ifndef __TRAP_H__
#define __TRAP_H__

#define concat_temp(x, y) x ## y
#define concat(x, y) concat_temp(x, y)

#ifndef REALMACHINE

#ifndef __ASSEMBLER__

#define HIT_GOOD_TRAP \
	asm volatile(".byte 0xd6" : : "a" (0))

#define HIT_BAD_TRAP \
	asm volatile(".byte 0xd6" : : "a" (1))

#define nemu_assert(cond) \
	do { \
		if( !(cond) ) HIT_BAD_TRAP; \
	} while(0)

static __attribute__((always_inline)) inline void
set_bp(void) {
	asm volatile ("int3");
}

#define REALMACHINE_NOTSUITABLE do { } while (0)


#else

#define HIT_GOOD_TRAP \
	movl $0, %eax; \
	.byte 0xd6

#define HIT_BAD_TRAP \
	movl $1, %eax; \
	.byte 0xd6

#define nemu_assert(reg, val) \
	cmp $val, %reg; \
	je concat(label,__LINE__); HIT_BAD_TRAP; concat(label,__LINE__):

#define REALMACHINE_NOTSUITABLE nop

#endif

#else // run test on real machine

#ifndef __ASSEMBLER__

void zby_hit_good_trap();
void zby_hit_bad_trap();
void zby_realmachine_notsuitable();
void zby_assert(const char *file, const char *func, int line, int val);
#define HIT_GOOD_TRAP zby_hit_good_trap()
#define HIT_BAD_TRAP zby_hit_bad_trap()
#define REALMACHINE_NOTSUITABLE zby_realmachine_notsuitable()
#define nemu_assert(cond) zby_assert(__FILE__, __FUNCTION__, __LINE__, !!(cond))

#else

#define HIT_GOOD_TRAP call zby_hit_good_trap
#define HIT_BAD_TRAP call zby_hit_bad_trap
#define REALMACHINE_NOTSUITABLE call zby_realmachine_notsuitable

#endif
#endif

#endif
