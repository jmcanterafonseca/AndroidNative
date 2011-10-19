#ifndef __VIBRATOR_LIB_TELF__

#define __VIBRATOR_LIB_TELF__

#include "vibrator.h"

int vib_play(int miliseconds,PlayOptions* options);
void vib_cancel(int requestId);
int multipleVibrate(int* pattern,int repeat);

#endif
