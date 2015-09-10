#include "trap.h"

int check(void *p1, unsigned int val, int val_size, int n)
{
    
    int i, cnt;
    unsigned char *ptr_char = p1;
    unsigned short *ptr_short = p1;
    unsigned int *ptr_int = p1;
    
    nemu_assert(val_size == 1 || val_size == 2 || val_size == 4);
    nemu_assert(n % val_size == 0);
    cnt = n / val_size;
    
    for (i = 0; i < cnt; i++) {
        switch (val_size) {
            case 1: if (*ptr_char++ != (val & 0xff)) return -1; break;
            case 2: if (*ptr_short++ != (val & 0xffff)) return -1; break;
            case 4: if (*ptr_int++ != val) return -1; break;
        }
    }
    
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
void test_ ## INSTR (void *edi, int eax, int ecx, int df) \
{ \
    volatile int new_eax, new_edi, new_ecx; \
    \
    __asm__ __volatile__ ("mov %6, %%eax\n\t" \
                          "test %%eax, %%eax\n\t" \
                          "jz 1f\n\t" \
                          "std\n\t" \
                          "jmp 2f\n\t" \
                          "1:cld\n\t" \
                          "2:\n\t" \
                          \
                          "mov %3, %%eax\n\t" \
                          "mov %4, %%edi\n\t" \
                          "mov %5, %%ecx\n\t" \
                          \
                          "rep " #INSTR "\n\t" \
                          \
                          "mov %%eax, %0\n\t" \
                          "mov %%edi, %1\n\t" \
                          "mov %%ecx, %2\n\t" \
                          "cld\n\t" \
                          \
                          :"=m"(new_eax), "=m"(new_edi), "=m"(new_ecx) \
                          :"m"(eax), "m"(edi), "m"(ecx), "m"(df) \
                          :"eax", "edi", "ecx"); \
    \
    int step = DATA_BYTE; \
    if (df) step = -step; \
    nemu_assert((int)edi + ecx * step == new_edi); \
    nemu_assert(new_ecx == 0); \
    nemu_assert(new_eax == eax); \
    if (df == 0) { \
        nemu_assert(check(edi, eax, DATA_BYTE, ecx * step) == 0); \
    } else { \
        nemu_assert(check((void *)(new_edi - step), eax, DATA_BYTE, ecx * (-step)) == 0); \
    } \
}


MAKE_TEST_FUNC(stosl, 4)
MAKE_TEST_FUNC(stosw, 2)
MAKE_TEST_FUNC(stosb, 1)


#define CANARY1 0xaabcccdd
#define CANARY2 0x11223344

int size = 8;

int dest[] = {
    CANARY1,
    0, 0, 0, 0, 0, 0, 0, 0,
    CANARY2,
};

int fill_data[] = {
    0x1a2b3c4d, 0x7f6e5d4c, 0x23457833, 0xffffffff, 0x00000000, 0x7f7f7f7f
};
int fill_data_size = sizeof(fill_data) / sizeof(fill_data[0]);

int main()
{
    int i;
    
    for (i = 0; i < fill_data_size; i++) {
        naive_memset(dest + 1, 0xcc, size * 4);
        test_stosl(dest + 1, fill_data[i], size, 0);
        nemu_assert(dest[0] == CANARY1);
        nemu_assert(dest[size + 1] == CANARY2);
        naive_memset(dest + 1, 0xcc, size * 4);
        test_stosl(dest + size, fill_data[i], size, 1);
        nemu_assert(dest[0] == CANARY1);
        nemu_assert(dest[size + 1] == CANARY2);
    }
    
    for (i = 0; i < fill_data_size; i++) {
        naive_memset(dest + 1, 0xcc, size * 4);
        test_stosw(dest + 1, fill_data[i], size * 2, 0);
        nemu_assert(dest[0] == CANARY1);
        nemu_assert(dest[size + 1] == CANARY2);
        naive_memset(dest + 1, 0xcc, size * 4);
        test_stosw((char *)dest + size * 4 + 4 - 2, fill_data[i], size * 2, 1);
        nemu_assert(dest[0] == CANARY1);
        nemu_assert(dest[size + 1] == CANARY2);
    }
    
    for (i = 0; i < fill_data_size; i++) {
        naive_memset(dest + 1, 0xcc, size * 4);
        test_stosb(dest + 1, fill_data[i], size * 4, 0);
        nemu_assert(dest[0] == CANARY1);
        nemu_assert(dest[size + 1] == CANARY2);
        naive_memset(dest + 1, 0xcc, size * 4);
        test_stosb((char *)dest + size * 4 + 4 - 1, fill_data[i], size * 4, 1);
        nemu_assert(dest[0] == CANARY1);
        nemu_assert(dest[size + 1] == CANARY2);
    }
    
    HIT_GOOD_TRAP;
    
    return 0;
}
