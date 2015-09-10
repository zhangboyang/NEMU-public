#include "trap.h"

int naive_memcmp(void *p1, void *p2, int n)
{
    unsigned char *s1 = p1, *s2 = p2;
    int i;
    for (i = 0; i < n; i++)
        if (s1[i] < s2[i])
            return -1;
        else if (s1[i] > s2[i])
            return 1;
    return 0;
}

void naive_memset(void *p, char c, int n)
{
    unsigned char *s = p;
    int i;
    for (i = 0; i < n; i++)
        s[i] = c;
}


#define MAKE_TEST_FUNC(INSTR, DATA_BYTE) \
void test_ ## INSTR (void *esi, void *edi, int ecx, int df) \
{ \
    volatile int new_esi, new_edi, new_ecx; \
    \
    __asm__ __volatile__ ("mov %6, %%eax\n\t" \
                          "test %%eax, %%eax\n\t" \
                          "jz 1f\n\t" \
                          "std\n\t" \
                          "jmp 2f\n\t" \
                          "1:cld\n\t" \
                          "2:\n\t" \
                          \
                          "mov %3, %%esi\n\t" \
                          "mov %4, %%edi\n\t" \
                          "mov %5, %%ecx\n\t" \
                          \
                          "rep " #INSTR "\n\t" \
                          \
                          "mov %%esi, %0\n\t" \
                          "mov %%edi, %1\n\t" \
                          "mov %%ecx, %2\n\t" \
                          "cld\n\t" \
                          \
                          :"=m"(new_esi), "=m"(new_edi), "=m"(new_ecx) \
                          :"m"(esi), "m"(edi), "m"(ecx), "m"(df) \
                          :"eax", "esi", "edi", "ecx"); \
    \
    int step = DATA_BYTE; \
    if (df) step = -step; \
    nemu_assert((int)esi + ecx * step == new_esi); \
    nemu_assert((int)edi + ecx * step == new_edi); \
    nemu_assert(new_ecx == 0); \
    if (df == 0) { \
        nemu_assert(naive_memcmp(esi, edi, ecx * step) == 0); \
    } else { \
        nemu_assert(naive_memcmp((void *)(new_esi - step), (void *)(new_edi - step), ecx * (-step)) == 0); \
    } \
}



MAKE_TEST_FUNC(movsl, 4)
MAKE_TEST_FUNC(movsw, 2)
MAKE_TEST_FUNC(movsb, 1)

#define CANARY1 0xaabcccdd
#define CANARY2 0x11223344

int size = 8;

int src[] = {
    0xdeadbeef, 0xbeefdead, 0x7fffffff, 0x80000000,
    0x98765432, 0x23456789, 0xffffffff, 0xa1b2c3d4,
};

int dest[] = {
    CANARY1,
    0, 0, 0, 0, 0, 0, 0, 0,
    CANARY2,
};


int main()
{
    naive_memset(dest + 1, 0, size * 4);
    test_movsl(src, dest + 1, size, 0);
    nemu_assert(dest[0] == CANARY1);
    nemu_assert(dest[size + 1] == CANARY2);
    
    naive_memset(dest + 1, 0, size * 4);
    test_movsl(src + size - 1, dest + size, size, 1);
    nemu_assert(dest[0] == CANARY1);
    nemu_assert(dest[size + 1] == CANARY2);
    
    
    naive_memset(dest + 1, 0, size * 4);
    test_movsw(src, dest + 1, size * 2, 0);
    nemu_assert(dest[0] == CANARY1);
    nemu_assert(dest[size + 1] == CANARY2);
    
    naive_memset(dest + 1, 0, size * 4);
    test_movsw((char *)src + size * 4 - 2, (char *)dest + 4 + size * 4 - 2, size * 2, 1);
    nemu_assert(dest[0] == CANARY1);
    nemu_assert(dest[size + 1] == CANARY2);
    
    
    naive_memset(dest + 1, 0, size * 4);
    test_movsb(src, dest + 1, size * 4, 0);
    nemu_assert(dest[0] == CANARY1);
    nemu_assert(dest[size + 1] == CANARY2);
    
    naive_memset(dest + 1, 0, size * 4);
    test_movsb((char *)src + size * 4 - 1, (char *)dest + 4 + size * 4 - 1, size * 4, 1);
    nemu_assert(dest[0] == CANARY1);
    nemu_assert(dest[size + 1] == CANARY2);
    
    HIT_GOOD_TRAP;
    
    return 0;
}
