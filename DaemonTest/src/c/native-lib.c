#include <jni.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>

JNIEXPORT jstring JNICALL Java_com_daemontest_DaemonTest_stringFromJNI(JNIEnv* env, jobject thisObject)
{
	char msg[] = "*** DaemonTest ***\n\n";
	char str[] = "Das ist das Haus vom Nikolaus.\n";
	char str2[] = "Create Daeon failed!\n";
	char str3[] = "Close Parent Process.\n";
	int result;
	pid_t pid, sid;

	FILE* file = fopen("/storage/emulated/0/Dokumente/DaemonTest.log.txt", "a");
	if (file == NULL) return (*env)->NewStringUTF(env, "Failed to open file!");
	fwrite(msg, strlen(msg), 1, file);
	fclose(file);

	pid = fork();
	if (pid < 0) {
		file = fopen("/storage/emulated/0/Dokumente/DaemonTest.log.txt", "a");
		fwrite(str2, strlen(str2), 1, file);
		fclose(file);
		exit(EXIT_FAILURE);
	}

	if (pid > 0) {
		file = fopen("/storage/emulated/0/Dokumente/DaemonTest.log.txt", "a");
		fwrite(str3, strlen(str3), 1, file);
		fclose(file);
		exit(EXIT_SUCCESS);
		return (*env)->NewStringUTF(env, "Child Process active.");
	}


	clock_t startTime = clock();
	while (1)
	{
		if ((double)(clock() - startTime) / CLOCKS_PER_SEC > 10)
		{
			file = fopen("/storage/emulated/0/Dokumente/DaemonTest.log.txt", "a");
			fwrite(str, strlen(str), 1, file);
			fclose(file);
			startTime = clock();
		}
	}

	return (*env)->NewStringUTF(env, "Fertsch!!!");
}