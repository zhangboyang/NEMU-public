#include "trap.h"

#define SZ 1024 * 1024 * 1
char mem[SZ];

int main() {
	
    int i;
    for (i = 0; i < SZ; i++)
        mem[i] = 0xaa;

	HIT_GOOD_TRAP;
	return 0;
}
