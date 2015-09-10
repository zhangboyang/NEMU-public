#include "trap.h"
#include <stdlib.h>
int cmp(const void *a, const void *b)
{
    return *(int *)a - *(int *)b;
}
int main()
{
    int i;
    int n = 8;
    int a[] = {5, 2, 7, 4, 1, 3, 8, 6};
    qsort(a, n, sizeof(int), cmp);
    for (i = 0; i < n; i++)
        nemu_assert(a[i] == i + 1);
    HIT_GOOD_TRAP;
    return 0;
}

