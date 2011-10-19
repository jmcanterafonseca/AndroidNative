
#include "vibratorlib.h"
#include <stdlib.h>

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "client-vibrator", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "client-vibrator", __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, "client-vibrator", __VA_ARGS__))


int main(int argc, char** argv) {

	if(argc < 2) {
		return -1;
	}

	int duration = atoi(argv[1]);

	LOGI("Duration Command Line: %d",duration);

	if(duration > 0) {
		vib_play(duration,NULL);
	}
	else {
			vib_cancel_all();
	}
}
