
#include "vibrator.h"
#include <stdlib.h>


int main(int argc, char** argv) {

	if(argc < 2) {
		return -1;
	}

	int duration = atoi(argv[1]);

	LOGI("Duration Command Line: %d",duration);

	if(duration > 0) {
		simpleVibrate(duration);
	}
	else {
			vibrateOff();
	}
}
