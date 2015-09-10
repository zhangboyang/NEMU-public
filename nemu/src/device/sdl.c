#include "common.h"

#ifdef HAS_DEVICE

#include "sdl.h"
#include "vga.h"

#include <time.h>
//#include <sys/time.h>
#include <signal.h>

extern uint8_t fontdata_8x16[128][16];
SDL_Surface *real_screen;
SDL_Surface *screen;
uint8_t (*pixel_buf) [SCREEN_COL];

static uint64_t jiffy = 0;
//static struct itimerval it;
extern void timer_intr();
extern void keyboard_intr();
extern void update_screen();
static int sdl_event_pending = 0;
static void device_update() {
	jiffy ++;
	timer_intr();
	assert(TIMER_HZ % VGA_HZ == 0);
	if(jiffy % (TIMER_HZ / VGA_HZ) == 0) {
		update_screen();
	}

	SDL_Event event;
	while ((sdl_event_pending = SDL_PollEvent(&event))) {
		// If a key was pressed

		uint32_t sym = event.key.keysym.sym;
		if( event.type == SDL_KEYDOWN ) {
			keyboard_intr(sym2scancode[sym >> 8][sym & 0xff]);
		}
		else if( event.type == SDL_KEYUP ) {
			keyboard_intr(sym2scancode[sym >> 8][sym & 0xff] | 0x80);
		}

		// If the user has Xed out the window
		if( event.type == SDL_QUIT ) {
			//Quit the program
			exit(0);
		}
		break; // only process one key each time, since we can't send keyboard interrupt that fast
	}

	//int ret = setitimer(ITIMER_VIRTUAL, &it, NULL);
	//Assert(ret == 0, "Can not set timer");
}


static double ts_minus(const struct timespec *tsp1, const struct timespec *tsp2)
{
    /* tsp1 - tsp2 (in ms)
     * note: pay attation to rounding method */
    return (tsp1->tv_sec - tsp2->tv_sec) * 1000 +
           (tsp1->tv_nsec - tsp2->tv_nsec) / 1000000.0;
}

static struct timespec last_device_update_ts;

double tot_timer_jitter = 0;
double tot_timer_cnt = 0;
#define TS_DIFF (1000 / TIMER_HZ)
void check_device_update()
{
    static double diff_target;
    static int init_flag = 0;
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    
    if (!init_flag) { // should init
        last_device_update_ts = ts;
        diff_target = TS_DIFF;
        init_flag = 1;
        return;
    }
    
    double diff = ts_minus(&ts, &last_device_update_ts);
    if (sdl_event_pending) {
        device_update();
    } else if (diff >= diff_target) {
        double jitter = (diff - diff_target);
        diff_target = TS_DIFF - jitter;
        tot_timer_jitter += jitter;
        tot_timer_cnt++;
        last_device_update_ts = ts;
        device_update();
    }
}

void sdl_clear_event_queue() {
	SDL_Event event;
	while(SDL_PollEvent(&event));
}

void init_sdl() {
	int ret = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_NOPARACHUTE);
	Assert(ret == 0, "SDL_Init failed");

    int z = 3;
    int w = 320 * z, h = 200 * z;
	real_screen = SDL_SetVideoMode(w, h, 8, 
			SDL_HWSURFACE | SDL_HWPALETTE | SDL_HWACCEL | SDL_ASYNCBLIT);

	screen = SDL_CreateRGBSurface(SDL_SWSURFACE, w, h, 8,
			real_screen->format->Rmask, real_screen->format->Gmask,
			real_screen->format->Bmask, real_screen->format->Amask);
	pixel_buf = screen->pixels;

	SDL_SetPalette(real_screen, SDL_LOGPAL | SDL_PHYSPAL, (void *)&palette, 0, 256);
	SDL_SetPalette(screen, SDL_LOGPAL, (void *)&palette, 0, 256);

	SDL_WM_SetCaption("NEMU", NULL);

	SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);

//	struct sigaction s;
//	memset(&s, 0, sizeof(s));
//	s.sa_handler = device_update;
//	ret = sigaction(SIGVTALRM, &s, NULL);
//	Assert(ret == 0, "Can not set signal handler");

//	it.it_value.tv_sec = 0;
//	it.it_value.tv_usec = 1000000 / TIMER_HZ;
//	ret = setitimer(ITIMER_VIRTUAL, &it, NULL);
//	Assert(ret == 0, "Can not set timer");
    
    // catch Ctrl-C, dirty hack
    signal(SIGINT, SIG_DFL);
}
#endif	/* HAS_DEVICE */
