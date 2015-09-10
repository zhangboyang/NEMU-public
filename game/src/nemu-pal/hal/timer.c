#include "hal.h"


#define MS_PER_INTERRUPT (1000 / HZ)


static volatile uint32_t jiffy = 0;
static int fps = 0;
static int nr_draw = 0;

void
incr_nr_draw(void) {
	nr_draw ++;
}

int
get_fps() {
	return fps;
}


#define USE_HIGHRES_TIMER

#ifndef USE_HIGHRES_TIMER

/* ============= the timer interrupt based timer functions ============== */
void
timer_event(void) {
	jiffy ++;
	//printf("jiffy=%d, time=%d ms\n", jiffy, jiffy * 10);
	if(jiffy % (HZ / 2) == 0) {
		fps = nr_draw * 2 + 1;
		nr_draw = 0;
	}
	/*if (MS_PER_INTERRUPT * jiffy % 1000 == 0) {
	    printf("jiffy = %d\n", jiffy);
	}*/
}


uint32_t SDL_GetTicks() {
	/* DONE: Return the time in millisecond. */
	return MS_PER_INTERRUPT * jiffy;
}

void SDL_Delay(uint32_t ms) {
    //printf("delay=%d\n", ms);
	/* DONE: Return from this function after waiting for `ms' milliseconds. */
	uint32_t target_jiffy = jiffy + ms / MS_PER_INTERRUPT;
	while (jiffy < target_jiffy);
}
#else


/* ============= the timespec based timer functions ============== */

struct zby_timespec {
    int tv_sec, tv_nsec;
};

#define TIMER_FACTOR 1

static inline void ts_plus(struct zby_timespec *tsp2, const struct zby_timespec *tsp1, int ms)
{
    /* tsp2 = tsp1 + ms
     * note: tsp1 may equal to tsp2 */
    int nsec = tsp1->tv_nsec + (ms % 1000) * 1000000 / TIMER_FACTOR;
    tsp2->tv_sec = tsp1->tv_sec + ms / 1000 / TIMER_FACTOR + nsec / 1000000000;
    tsp2->tv_nsec = nsec % 1000000000;
}
static inline int ts_minus(const struct zby_timespec *tsp1, const struct zby_timespec *tsp2)
{
    /* tsp1 - tsp2 (in ms)
     * note: pay attation to rounding method */
    return (tsp1->tv_sec - tsp2->tv_sec) * 1000 +
           (tsp1->tv_nsec - tsp2->tv_nsec) / 1000000;
}
static inline int ts_less(const struct zby_timespec *tsp1, const struct zby_timespec *tsp2)
{
    /* use this function to check deadline:
     *   tsp1 -> deadline
     *   tsp2 -> current time
     * note: should not use ts_minus(tsp1, tsp2) < 0, since it's unit is ms */
    return tsp1->tv_sec < tsp2->tv_sec ||
           (tsp1->tv_sec == tsp2->tv_sec && tsp1->tv_nsec <= tsp2->tv_nsec);
}

static inline void zby_clock_gettime(struct zby_timespec *ts) // only the lower 32 bit is used
{
    int tv_nsec = 200, tv_sec;
    __asm__ __volatile__ (".byte 0xd5": "+a"(tv_nsec), "=d"(tv_sec));
    ts->tv_nsec = tv_nsec;
    ts->tv_sec = tv_sec;
    //printf("ts: %d %d\n", ts->tv_sec, ts->tv_nsec);
}

void
timer_event(void) {
    // nothing to do here
    jiffy++;
    if (jiffy % HZ == 0) {
		fps = nr_draw;
		nr_draw = 0;
	}
}


static struct zby_timespec initial_ts;
static int timer_init_flag = 0;

uint32_t SDL_GetTicks() {
    struct zby_timespec ts;
    zby_clock_gettime(&ts);
	if (!timer_init_flag) { initial_ts = ts; timer_init_flag = 1; }
	int ms_diff = ts_minus(&ts, &initial_ts);
	return ms_diff * TIMER_FACTOR;
}

void SDL_Delay(uint32_t ms) {
    if (ms < 10) return;
/*	uint32_t target_ms = SDL_GetTicks() + ms;
	while (SDL_GetTicks() < target_ms);
*/
    struct zby_timespec target_ts, ts;
    zby_clock_gettime(&target_ts);
    ts_plus(&target_ts, &target_ts, ms);
    //int cnt = 0;
    do {
        zby_clock_gettime(&ts);
        //cnt++;
    } while (ts_less(&ts, &target_ts));
    //if (cnt < 5) printf("%s%s  SDL_Delay is too slow, ms = %d, cnt = %d%s\n", c_bold, c_red, ms, cnt, c_normal);
}
#endif
