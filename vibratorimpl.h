#ifndef __VIBRATOR_IMPL_TELF__

#define __VIBRATOR_IMPL_TELF__

#include "vibrator.h"

// Play request
int vib_play_impl(int duration,PlayOptions* options);

// Initialization request
int vib_init();

int vib_cancel_all_impl();

int vib_cancel_impl(int reqId);

int vib_terminate();

#endif
