#include <stdio.h>
#include <stdlib.h>

#define c_normal         "\x1b[0m"
#define c_bold           "\x1b[1m"
#define c_red            "\x1b[31m"
#define c_green          "\x1b[32m"
#define c_yellow         "\x1b[33m"
#define c_blue           "\x1b[34m"
#define c_purple         "\x1b[35m"
#define c_cyan           "\x1b[36m"

void zby_hit_good_trap()
{
    puts(c_green "HIT GOOD TRAP" c_normal);
    exit(0);
}

void zby_hit_bad_trap()
{
    puts(c_bold c_red "HIT BAD TRAP" c_normal);
    exit(1);
}

void zby_realmachine_notsuitable()
{
    puts(c_blue "NOT SUITABLE FOR REALMACHINE" c_normal);
    exit(0);
}

void zby_assert(const char *file, const char *func, int line, int val)
{
    if (!val) {
        printf("assert failed at file: %s  func: %s  line: %d\n", file, func, line);
        exit(1);
    }
}

#if HAVEASM == 1
#include <sys/mman.h>
#include <errno.h>
#include <string.h>
void create_mmap(int base, int size)
{
    void *p = mmap((void *) base, size, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    if ((int)p != base) {
        printf(c_bold c_red "MMAP %08x FAILED: p=%p errno=%s\n" c_normal, base, p, strerror(errno));
        exit(1);
    }
}
void change_permission(void *base)
{
    int ret;
    base = (void *)((int)base & 0xfffff000); // assume page size is 4K
    ret = mprotect(base, 0x100, PROT_READ | PROT_WRITE | PROT_EXEC);
    if (ret != 0) {
        printf(c_bold c_red "MPROTECT %08x FAILED: errno=%s\n" c_normal, base, strerror(errno));
        exit(1);
    }
    while (ret == 0) ret = mprotect((base += 0x100), 0x100, PROT_READ | PROT_WRITE | PROT_EXEC);
    
}
void start();
int main()
{
    create_mmap(0x10000, 0x2000000);
    create_mmap(0x07000000, 0x01000000);
    change_permission(start);
    start();
    zby_hit_bad_trap();
    return 0;
}
#endif
