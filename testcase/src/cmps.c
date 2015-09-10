#include "trap.h"

#include <stdio.h>
int naive_strlen(const char *s)
{
    const char *p = s;
    while (*p) p++;
    return p - s;
}

int check(void *p1, void *p2, int val_size, int n, int df)
{
    
    int i;
    unsigned int a = 0, b = 0;
    unsigned char *p1_char = p1, *p2_char = p2;
    unsigned short *p1_short = p1, *p2_short = p2;
    unsigned int *p1_int = p1, *p2_int = p2;
    
    nemu_assert(val_size == 1 || val_size == 2 || val_size == 4);
    
    // direction = 0 : i = [0, n - 1]
    //           = 1 : i = [n - 1, 0]
    
    int st, ed, step;
    if (df) { st = n - 1; ed = 0; step = -1; }
    else { st = 0; ed = n - 1; step = 1; }
    
    //printf("%s %s\n", p1, p2);
    for (i = st; (step == 1 && i <= ed) || (step == -1 && i >= ed); i += step) {
        switch (val_size) {
            case 1: a = *p1_char; b = *p2_char; p1_char += step; p2_char += step; break;
            case 2: a = *p1_short; b = *p2_short; p1_short += step; p2_short += step; break;
            case 4: a = *p1_int; b = *p2_int; p1_int += step; p2_int += step; break;
        }
        //printf("%c %c\n", (char)a, (char)b);
        if (a != b) {
            return i; // return address of mismatch
        }
    }
    
    return -1; // ret < 0 means equal
}


#define MAKE_TEST_FUNC(INSTR, DATA_BYTE, DATA_TYPE) \
void test_ ## INSTR (void *esi, void *edi, int ecx, int df) \
{ \
    volatile int new_esi, new_edi, new_ecx, new_sf, new_zf; \
    \
    __asm__ __volatile__ ("mov %8, %%eax\n\t" \
                          "test %%eax, %%eax\n\t" \
                          "jz 1f\n\t" \
                          "std\n\t" \
                          "jmp 2f\n\t" \
                          "1:cld\n\t" \
                          "2:\n\t" \
                          \
                          "mov %5, %%esi\n\t" \
                          "mov %6, %%edi\n\t" \
                          "mov %7, %%ecx\n\t" \
                          \
                          "rep " #INSTR "\n\t" \
                          \
                          "mov %%esi, %0\n\t" \
                          "mov %%edi, %1\n\t" \
                          "mov %%ecx, %2\n\t" \
                          \
                          "movl $1, %3\n\t" \
                          "js 1f\n\t" \
                          "movl $0, %3\n\t" \
                          "1:\n\t" \
                          \
                          "movl $1, %4\n\t" \
                          "jz 1f\n\t" \
                          "movl $0, %4\n\t" \
                          "1:\n\t" \
                          \
                          "cld\n\t" \
                          \
                          :"=m"(new_esi), "=m"(new_edi), "=m"(new_ecx), "=m"(new_sf), "=m"(new_zf) \
                          :"m"(esi), "m"(edi), "m"(ecx), "m"(df) \
                          :"eax", "esi", "edi", "ecx"); \
    \
    int step = DATA_BYTE; \
    if (df) step = -step; \
    nemu_assert((int)esi + (ecx - new_ecx) * step == new_esi); \
    nemu_assert((int)edi + (ecx - new_ecx) * step == new_edi); \
    \
    int diff_pos = check(esi, edi, DATA_BYTE, ecx, df); \
    DATA_TYPE *esi_array = esi, *edi_array = edi; \
    if (diff_pos == -1) { \
        nemu_assert(new_ecx == 0); \
        nemu_assert(new_zf == 1); \
        nemu_assert(new_sf == 0); \
    } else { \
        nemu_assert(new_zf == 0); \
        if (df == 0) { \
            nemu_assert(new_sf == ((esi_array[diff_pos] - edi_array[diff_pos]) < 0)); \
            nemu_assert(diff_pos == ecx - new_ecx - 1); \
        } else { \
            nemu_assert(new_sf == ((esi_array[-ecx + diff_pos + 1] - edi_array[-ecx + diff_pos + 1]) < 0)); \
            nemu_assert(diff_pos == new_ecx); \
        } \
    } \
}



MAKE_TEST_FUNC(cmpsl, 4, int)
MAKE_TEST_FUNC(cmpsw, 2, short)
MAKE_TEST_FUNC(cmpsb, 1, signed char)

#define len 16
char data[][len + 2] = {
    "aaaaaaabaaaaaaaa",
    "aaaaaaaaaaaaaaaa",
    "aaaaacaaaaaaaaaa",
    "aaaaaaaaaadaaaaa",
    "aabaaabaaaaabaaa",
    "baaaaaaaaaaaaaaa",
    "aaaaaaaaaaaaaaab",
    "baaaaaabaaaaaaab",
    "bbbbbbbbbbbbbbbb",
    "bbbbbbbbbbbbbbbc",
    "helloworldhahaha",
    "woshijiangyou=w=",
    "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0",
    "\0\0\0\0\0\0a\0\0\0\0\0\0\0\0\0",
    "a\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0",
    "\0\0\0\0\0\0\0\0\0\0\0\0\0a\0\0",
    "\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff",
    "a" "\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff",
    "\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff" "a",
    "\xff\xff\xff\xff\xff\xff\xff" "a" "\xff\xff\xff\xff\xff\xff\xff\xff",
};

int size = sizeof(data) / sizeof(data[0]);

int main()
{
    int i, j;
    nemu_assert(len % 4 == 0);

    for (i = 0; i < size; i++)
        for (j = 0; j < size; j++) {
            test_cmpsb(data[i], data[j], len, 0);
            test_cmpsw(data[i], data[j], len / 2, 0);
            test_cmpsl(data[i], data[j], len / 2, 0);
            test_cmpsb(data[i] + len - 1, data[j] + len - 1, len, 1);
            test_cmpsw(data[i] + len - 2, data[j] + len - 2, len / 2, 1);
            test_cmpsl(data[i] + len - 4, data[j] + len - 4, len / 4, 1);
        }
    
    HIT_GOOD_TRAP;
    
    return 0;
}
