#include "trap.h"

#define CHAR_DATA {0, -1, -0x1f, 0x1f, -0x11, 0x11, 0x7f, -0x7f, -0x80}
unsigned char mychar[] = CHAR_DATA;
unsigned short mychar_to_short[] = CHAR_DATA;
unsigned int mychar_to_int[] = CHAR_DATA;
int mychar_len = sizeof(mychar) / sizeof(mychar[0]);

#define SHORT_DATA {0, -1, -0x11ff, 0x11ff, -0x11, 0x11, 0x7f, -0x7f, -0x80}
unsigned short myshort[] = SHORT_DATA;
unsigned int myshort_to_int[] = SHORT_DATA;
int myshort_len = sizeof(myshort) / sizeof(myshort[0]);

int main()
{
    int i;
    
    /* char to int/short */
    for (i = 0; i < mychar_len; i++) {
        signed char a = mychar[i];
        int b = a;
        short c = a;
        nemu_assert(b == mychar_to_int[i]);
        nemu_assert(c == mychar_to_int[i]);
    }
    
    /* short to int */
    for (i = 0; i < myshort_len; i++) {
        short a = myshort[i];
        int b = a;
        nemu_assert(b == myshort_to_int[i]);
    }
    
    HIT_GOOD_TRAP;
    return 0;
}
