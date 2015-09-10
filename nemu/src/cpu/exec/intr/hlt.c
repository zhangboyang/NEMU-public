#include "cpu/exec/helper.h"

make_helper(hlt) {
    extern void check_device_update();
    extern long long hlt_cnt;
    while (cpu.INTR == 0) {
        check_device_update();
        hlt_cnt++;
    }
	return 1;
}
