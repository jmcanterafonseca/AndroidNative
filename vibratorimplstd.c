/**
 *
 *  Simple vibrator implementation
 *
 *
 */

#include "vibratorimpl.h"
#include <hardware_legacy/vibrator.h>

int vib_play_impl(int duration,PlayOptions* options) {
	return vibrator_on(duration);
}

int vib_init() {
	return vibrator_off();
}

int vib_cancel_all_impl() {
	return vibrator_off();
}

int vib_cancel_impl(int reqId) {
	return 0;
}

int vib_terminate() {
	return 0;
}
