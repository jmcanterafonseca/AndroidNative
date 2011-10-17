#include <hardware_legacy/vibrator.h>
#include <android/log.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "server-vibrator", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "server-vibrator", __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, "server-vibrator", __VA_ARGS__))

#define ON_COMMAND "on"
#define OFF_COMMAND "off"
#define CANCEL_COMMAND "cnc"

static int request_counter = 0;
pthread_mutex_t m_counter;

/**
 *  Returns the next request id
 *
 */
static int nextRequestId() {
	int ret;

	pthread_mutex_lock(&m_counter);
	ret = request_counter++;
	pthread_mutex_unlock(&m_counter);

	return ret;
}

/*
 * Create a UNIX-domain socket address in the Linux "abstract namespace".
 *
 * The socket code doesn't require null termination on the filename, but
 * we do it anyway so string functions work.
 */
static int makeAddr(const char* name, struct sockaddr_un* pAddr,
		socklen_t* pSockLen) {
	int nameLen = strlen(name);
	if (nameLen >= (int) sizeof(pAddr->sun_path) - 1) /* too long? */
		return -1;
	pAddr->sun_path[0] = '\0'; /* abstract namespace */
	strcpy(pAddr->sun_path + 1, name);
	pAddr->sun_family = AF_LOCAL;
	*pSockLen = 1 + nameLen + offsetof(struct sockaddr_un, sun_path);
	return 0;
}

static int sendResponse(int sock,int code) {
	char response[10];
	sprintf(response, "%d\n", code);
	return write(sock, response, strlen(response));
}

/**
 *  Thread that actually performs a vibration
 *
 *
 */
void* do_vibrate(void* param) {
	int clientSock = (int) param;

	// The command can be "ON" or "OFF"
	char command[6];
	// up to 6 digits for duration
	char sduration[8];

	bzero(command, sizeof(command));
	bzero(sduration, sizeof(sduration));

	FILE *socket_stream_in = fdopen(clientSock, "r");

	// Command can be "on/off"
	fgets(command, sizeof(command) - 1, socket_stream_in);
	LOGI("Command: %s", command);

	if (strncmp(command, ON_COMMAND, 2) == 0) {
		fgets(sduration, sizeof(sduration) - 1, socket_stream_in);
		sduration[strlen(sduration) - 1] = '\0';
		LOGI("Duration as string: %s", sduration);

		int duration = atoi(sduration);
		LOGI("On Command. Duration: %d", duration);

		if (duration > 0) {

			sendResponse(clientSock,nextRequestId());

			int result = vibrator_on(duration);

			LOGI("Vibration request sent from thread!!: Result: %d", result);

			usleep(duration * 1000);
		}
	} else if (strncmp(command, OFF_COMMAND, 3) == 0) {
		LOGI("Off Command!");

		vibrator_off();
	} else if (strncmp(command, CANCEL_COMMAND, 3) == 0) {
		LOGI("Cancel Command");

		// We read the request Id of what has to be canceled
	} else {
		sendResponse(clientSock,-1);
	}

	fclose(socket_stream_in);
	close(clientSock);

	pthread_exit(NULL);
}

/**
 *  Makes the process a daemon
 *
 */
static void daemonize() {
	/* Our process ID and Session ID */
		pid_t pid, sid;

		/* Fork off the parent process */
		pid = fork();
		if (pid < 0) {
			exit(EXIT_FAILURE);
		}
		/* If we got a good PID, then
		 we can exit the parent process. */
		if (pid > 0) {
			exit(EXIT_SUCCESS);
		}

		/* Change the file mode mask */
		umask(0);

		/* Create a new SID for the child process */
		sid = setsid();
		if (sid < 0) {
			/* Log the failure */
			exit(EXIT_FAILURE);
		}

		/* Change the current working directory */
		if ((chdir("/")) < 0) {
			/* Log the failure */
			exit(EXIT_FAILURE);
		}

		/* Close out the standard file descriptors */
		close(STDIN_FILENO);
		close(STDOUT_FILENO);
		close(STDERR_FILENO);

}

int main(int argc, char** argv) {
	daemonize();

	struct sockaddr_un sockAddr;
	socklen_t sockLen;

	if (makeAddr("com.telefonica.owd.hardware", &sockAddr, &sockLen) < 0)
		return 1;

	int fd = socket(AF_LOCAL, SOCK_STREAM, PF_UNIX);
	if (fd < 0) {
		LOGE("server socket()");
		return 1;
	}

	if (bind(fd, (const struct sockaddr*) &sockAddr, sockLen) < 0) {
		LOGE("server bind()");
		pthread_exit(NULL);
	}

	if (listen(fd, 5) < 0) {
		LOGE("server listen()");
		pthread_exit(NULL);
	}

	pthread_mutex_init(&m_counter, NULL);

	vibrator_off();

	while (1) {
		int clientSock = accept(fd, NULL, NULL);
		if (clientSock < 0) {
			LOGE("server accept");
		} else {

			LOGI("Request arrived at the server");

			pthread_t theThread;

			struct sched_param param;
			pthread_attr_t tattr;
			pthread_attr_init(&tattr);
			pthread_attr_getschedparam(&tattr, &param);

			// pthread_attr_setschedpolicy(&tattr, SCHED_RR);
			param.sched_priority = 1;

			int rc = pthread_create(&theThread, &tattr, do_vibrate,
					(void *) clientSock);
			if (rc) {
				LOGE("return code from pthread_create() is %d\n", rc);
				pthread_exit(NULL);
			}
		}
	} // while

	pthread_mutex_destroy(&m_counter);

	return 0;
} //main

