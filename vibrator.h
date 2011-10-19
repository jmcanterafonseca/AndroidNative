#ifndef __VIBRATOR_TELF__

#define __VIBRATOR_TELF__


#define ON_COMMAND "on"
#define OFF_COMMAND "off"
#define CANCEL_COMMAND "cnc"

struct PlayOptions {
	int magnitude;
	char* style;
};

typedef struct PlayOptions PlayOptions;


#endif
