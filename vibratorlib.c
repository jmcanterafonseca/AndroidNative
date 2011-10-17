#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <strings.h>
#include "vibrator.h"

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

static int sendCommand(char* command,char** response) {
	struct sockaddr_un sockAddr;
	socklen_t sockLen;

	if (makeAddr("com.telefonica.owd.hardware", &sockAddr, &sockLen) < 0)
		return -1;

	int fd = socket(AF_LOCAL, SOCK_STREAM, PF_UNIX);
	if (fd < 0) {
		LOGE("client socket()");
		return -1;
	}

	LOGI("CLIENT %s\n", sockAddr.sun_path + 1);

	if (connect(fd, (const struct sockaddr*) &sockAddr, sockLen) < 0) {
		LOGE("client connect()");
		close(fd);
		return -1;
	}

	if (write(fd, command, strlen(command)) < 0) {
		LOGE("client write()");
		close(fd);

		return -1;
	}

	// We read the request Id in the response
	char* sreqId = malloc(10);
	bzero(sreqId, sizeof(sreqId));

	FILE *socket_stream_in = fdopen(fd, "r");

	// Response from the server is read
	fgets(sreqId, sizeof(sreqId) - 1, socket_stream_in);

	LOGI("Response returned by the server: %s", sreqId);

	sreqId[strlen(sreqId) - 1] = '\0';

	fclose(socket_stream_in);
	close(fd);

	*response = sreqId;

	return 0;
}


static int sendOnCommand(char* command) {
	char* response;

	if (sendCommand(command,&response) == 0) {
		int reqId = atoi(response);
		free(response);

		LOGI("Request Id returned by the server: %d", reqId);

		return reqId;
	} else {
		return -1;
	}

}


static int requestVibration(int duration) {
	char message[10];

	bzero(message, sizeof(message));

	sprintf(message, "%s\n%d\n", ON_COMMAND, duration);

	return sendOnCommand(message);
}

static int requestPatternVibration(char* pattern) {
	char message[256];

	bzero(message, sizeof(message));

	sprintf(message, "%s\n%s", ON_COMMAND, pattern);

	return sendOnCommand(message);
}

/**
 *   Methods exported by the library
 *
 *
 */

int multipleVibrate(int* pattern, int repeat) {
	char serialized[256];
	bzero(serialized, sizeof(serialized));

	char number[7];
	int j = 0;
	int len = sizeof(pattern) / sizeof(int);

	for (j = 0; j < len; j++) {
		bzero(number, sizeof(number));

		sprintf(number, "%d\n", pattern[j]);
		strcat(serialized, number);
	}

	return requestPatternVibration(serialized);
}

int simpleVibrate(int duration) {
	return requestVibration(duration);
}

void cancelVibrate(int requestId) {
	char command[15];

	sprintf(command,"%s\n%d\n",CANCEL_COMMAND,requestId);

	char* response;

	sendCommand(command,&response);

	free(response);
}

void vibrateOff() {
	char command[5];

	sprintf(command,"%s\n",OFF_COMMAND);

	char* response;

	sendCommand(command,&response);

	free(response);
}
