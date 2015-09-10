#include "trap.h"

#define SZ 1024 * 1024 * 1
char empty[SZ];

int main() {
	
    int i;
    for (i = 0; i < SZ; i++)
        nemu_assert(empty[i] == 0);

	HIT_GOOD_TRAP;
	return 0;
}
