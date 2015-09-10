#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

typedef struct watchpoint {
	int NO;
	struct watchpoint *next;

	/* DONE: Add more members if necessary */

    int expr_id;
    int hit;
    int type; /* type == 0 means wp hit when expr value changes
                      == 1 means wp hit when expr value is non-zero */
} WP;

#endif
