#include "vibratorimpl.h"
#include "ImmVibe.h"
#include <android/log.h>

#define VIBE_LICENSE_KEY_GALAXYS "DH75NDX26JR75GCZ5CKKTD25M9JP62TG"
// 3.3.13.1
#define VIBE_GALAXYS_VERSION_NUMBER 0x0303010D

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "immvibe-test", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "immvibe-test", __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, "immvibe-test", __VA_ARGS__))

static VibeInt32 devHandle = VIBE_INVALID_DEVICE_HANDLE_VALUE;

int vib_init() {
	int ret = 1;

	VibeStatus vs = ImmVibeInitialize(VIBE_GALAXYS_VERSION_NUMBER);

	if (VIBE_FAILED(vs)) {
		LOGE("Initialization Failed");

		if (vs == VIBE_E_INVALID_ARGUMENT) {
			LOGE("Version number do not match");
		}
	} else {
		LOGI("Initialized ok");

		vs = ImmVibeOpenDevice(0, &devHandle);

		if (VIBE_FAILED(vs)) {
			LOGE("Open device Failed");;
		} else {
			LOGI("Open Device worked!!!");

			ImmVibeSetDevicePropertyString(devHandle,
					VIBE_DEVPROPTYPE_LICENSE_KEY, VIBE_LICENSE_KEY_GALAXYS);

			if (VIBE_FAILED(vs)) {
				LOGE("License key not accepted");
			} else {
				LOGI("License key has been accepted");

				ret = 0;
			}
		}
	}

	return ret;
}

int vib_play_impl(int duration,PlayOptions* options) {

	VibeInt32 g_nDuration = duration; /* play for 1000 ms (1 s) */
	VibeInt32 g_nMagnitude = VIBE_MAX_MAGNITUDE;
	VibeInt32 g_nStyle = VIBE_STYLE_STRONG;
	VibeInt32 g_nAttackTime = 0; /* no envelope */
	VibeInt32 g_nAttackLevel = 0; /* no envelope */
	VibeInt32 g_nFadeTime = 0; /* no envelope */
	VibeInt32 g_nFadeLevel = 0; /* no envelope */

	VibeInt32 g_hEffect;

	VibeStatus vs = ImmVibePlayMagSweepEffect(devHandle,g_nDuration,g_nMagnitude,VIBE_STYLE_SHARP,
								 g_nAttackTime, g_nAttackLevel, g_nFadeTime, g_nFadeLevel, &g_hEffect);

	return g_hEffect;
}

int vib_cancel_all_impl() {
	return ImmVibeStopAllPlayingEffects(devHandle);
}

int vib_cancel_impl(int reqId) {
	return ImmVibeStopPlayingEffect(devHandle,reqId);
}

int vib_terminate() {
	ImmVibeCloseDevice(devHandle);
	ImmVibeTerminate();
}
