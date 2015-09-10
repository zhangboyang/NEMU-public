#include "hal.h"

#define NR_KEYS 18

/* Only the following keys are used in NEMU-PAL. */
static const int keycode_array[] = {
	K_UP, K_DOWN, K_LEFT, K_RIGHT, K_ESCAPE,
	K_RETURN, K_SPACE, K_PAGEUP, K_PAGEDOWN, K_r,
	K_a, K_d, K_e, K_w, K_q,
	K_s, K_f, K_p
};

static int key_event[NR_KEYS]; // 0:press_event   1:release_event
static int key_state[NR_KEYS]; // 0:release       1:pressed
static int key_processed[NR_KEYS];

static int find_key_index(int code)
{
    int i;
    code &= 0x7f;
    for (i = 0; i < NR_KEYS; i++)
	    if (keycode_array[i] == code)
	        return i;
	printf("unknown key = %d\n", code);
	return -1;
}

#define I8042_DATA_PORT 0x60

void
keyboard_event(void) {
	/* Fetch the scancode and update the key states. */
	int code = (unsigned) (unsigned char) in_byte(I8042_DATA_PORT);
	
	// the code below is not fully safe
	// but it doesn't matter because you can't type that fast
	
	int index = find_key_index(code);
	
	if (code < 0x80) { // press_event
	    key_event[index] = 1;
	} else { // release_event
	    if (key_processed[index]) {
	        key_event[index] = 0;
	        key_processed[index] = 0;
	    }
	}
}

bool 
process_keys(void (*key_press_callback)(int), void (*key_release_callback)(int)) {
    //printf("processkeys\n");
	cli();
	/* Traverse the key states. Find a key just pressed or released.
	 * If a pressed key is found, call ``key_press_callback'' with the keycode.
	 * If a released key is found, call ``key_release_callback'' with the keycode.
	 * If any such key is found, the function return true.
	 * If no such key is found, the function return false.
	 * Remember to enable interrupts before returning from the function.
	 */
    //printf("ask for keys\n");
    bool ret = false;
	/*
	    the state machine: (maely machine)
	 
          [in]:press_event         [in]:press_event
          [out]:noevent            [out]:press
	            +>>>>>>>>   +------<<<<<<<<<<<<------+
	            |       v   |                        |
	            +<<<<<<<PRESSED                    RELEASED<<<<<<<+ [in]:release_event
	                        |                        |   v        | [out]:noevent
	                        +------>>>>>>>>>>>>------+   +>>>>>>>>^
	                               [in]:release_event
	                               [out]:release
	*/
	
	int index;
	for (index = 0; index < NR_KEYS; index++) { // process each key
	    int state = key_state[index]; // current machine state: 0: RELEASED  1: PRESSED
	    int event = key_event[index]; // the machine input: [in]:event  0:release_event  1:press_event
	    int out_event; // the machine output: [out]:event  0: noevent  1:press  2:release
	    int next_state; // the next state of machine
	    
	    if (state == 0) { // current state is RELEASED
	        if (event == 0) { // in:release_event
	            out_event = 0; // out:noevent
	            next_state = 0; // next:RELEASED
	        } else { // in: press_event
	            out_event = 1; // out:press
	            next_state = 1; // next:PRESSED
	        }
	    } else { // current state is PRESSED
	        if (event == 0) { // in:release_event
	            out_event = 2; // out:release
	            next_state = 0; // next:RELEASED
	        } else { // in: press_event
	            out_event = 0; // out:noevent
	            next_state = 1; // next:PRESSED
	        }
	    }
	    
	    key_state[index] = next_state;
	    key_processed[index] = 1;
	    
	    if (out_event != 0) {
	        if (out_event == 1) key_press_callback(keycode_array[index]);
	        if (out_event == 2) key_release_callback(keycode_array[index]);
	        printf("gen key event: index = %d, event = %d\n", index, out_event);
	        ret = true;
	        goto done;
	    }
	}
	
done:
	sti();
	return ret;
}
