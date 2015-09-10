#include <stdio.h>
#include <time.h>

void print_current_time()
{
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    printf("[%010u.%09u] ", (unsigned) ts.tv_sec, (unsigned) ts.tv_nsec);
}
