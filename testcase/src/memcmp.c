#include "trap.h"
#include <string.h>

/* NOTE:
    it it **lucky** that memcmp() can work correctly
    since it checks ZF after SHR instruction

  8001d0:	75 1b                	jne    8001ed <BYTECMP> // **lucky** this make sure that ZF==1
  8001d2:	89 cb                	mov    %ecx,%ebx
  8001d4:	c1 e9 02             	shr    $0x2,%ecx // let we assume (ECX >> 2) == 0, i.e. ECX==0,1,2,3
  8001d7:	f3 a7                	repz cmpsl %es:(%edi),%ds:(%esi) // ECX == 0, no flag tested
  8001d9:	74 0d                	je     8001e8 <L5>  // check for invalid flag ZF    

    it is highly recommanded that: update ZF when executing SHR instruction
*/

char mychar[] __attribute__ ((aligned (16))) = {
    0xaa, 0xbb, 0xcc,
    0, 0, 0, 0, 0,
    2, 3, 4,
    2, 4, 3,
    3, 2, 4,
    3, 4, 2,
    4, 2, 3,
    4, 3, 2,
    0, 0, 0, 0,
    1, 1, 1, 1,
};

int size = sizeof(mychar);

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

int main()
{
    int i, j, k;
    int x, y;
    for (i = 0; i < size; i++)
        for (j = 0; j < size; j++)
            for (k = 0; i + k < size && j + k < size; k++) {
                x = memcmp(mychar + i, mychar + j, k);
                y = naive_memcmp(mychar + i, mychar + j, k);
                if (x == 0) nemu_assert(y == 0);
                if (x < 0) nemu_assert(y < 0);
                if (x > 0) nemu_assert(y > 0);
            }
    HIT_GOOD_TRAP;
    return 0;
}
