#include <jni.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <time.h>

void myThread(FILE* file)
{
	char str[] = "Das ist das Haus vom Nikolaus.\n";
	clock_t startTime = clock();
	while (1)
	{
		if ((double)(clock() - startTime) / CLOCKS_PER_SEC > 10)
		{
			file = fopen("/storage/emulated/0/Dokumente/ThreadTest.log.txt", "a");
			fwrite(str, strlen(str), 1, file);
			fclose(file);
			startTime = clock();
		}
	}
}

JNIEXPORT jstring JNICALL Java_com_threadtest_ThreadTest_stringFromJNI(JNIEnv* env, jobject thisObject)
{
	pthread_t threadID;
	char msg[] = "*** ThreadTest ***\n\n";
	int result;

	FILE* file = fopen("/storage/emulated/0/Dokumente/ThreadTest.log.txt", "a");
	if (file == NULL) return (*env)->NewStringUTF(env, "Failed to open file!");
	fwrite(msg, strlen(msg), 1, file);
	fclose(file);

	result = pthread_create(&threadID, NULL, (void*)myThread, file);
	if (result) return (*env)->NewStringUTF(env, "Can't creat thread!");

	return (*env)->NewStringUTF(env, "Fertsch!!!");
}