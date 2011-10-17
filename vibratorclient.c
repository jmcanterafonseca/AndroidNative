#include <android/log.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "client-vibrator", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "client-vibrator", __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, "client-vibrator", __VA_ARGS__))

#define ON_COMMAND "on"
#define OFF_COMMAND "off"

int makeAddr(const char* name, struct sockaddr_un* pAddr, socklen_t* pSockLen) {
	int nameLen = strlen(name);
	if (nameLen >= (int) sizeof(pAddr->sun_path) - 1) /* too long? */
		return -1;
	pAddr->sun_path[0] = '\0'; /* abstract namespace */
	strcpy(pAddr->sun_path + 1, name);
	pAddr->sun_family = AF_LOCAL;
	*pSockLen = 1 + nameLen + offsetof(struct sockaddr_un, sun_path);
	return 0;
}

int main(int argc, char** argv) {

	struct sockaddr_un sockAddr;
	socklen_t sockLen;

	char message[10];

	bzero(message,sizeof(message));

	sprintf(message,"%s\n%d\n",ON_COMMAND,1000);

	if (makeAddr("com.telefonica.owd.hardware", &sockAddr, &sockLen) < 0)
		return 1;

	int fd = socket(AF_LOCAL, SOCK_STREAM, PF_UNIX);
	if (fd < 0) {
		LOGE("client socket()");
		return 1;
	}

	LOGI("CLIENT %s\n", sockAddr.sun_path+1);

	if (connect(fd, (const struct sockaddr*) &sockAddr, sockLen) < 0) {
		LOGE("client connect()");
		close(fd);
		return 1;
	}

	if (write(fd, message, strlen(message)) < 0) {
		LOGE("client write()");
		close(fd);
	}

	return 0;

}
