#include <jni.h>
#include <stdio.h>
#include <string.h>
#include <dlfcn.h>

JNIEXPORT jstring JNICALL Java_com_dltest_DLtest_stringFromJNI(JNIEnv* env, jobject thisObject, jstring jLibPath)
{
	char msg[] = "*** DL-Test ***\n\n";
	char dlPath[128];
	void* handle;
	const char* libPath = (*env)->GetStringUTFChars(env, jLibPath, NULL);
	sprintf(dlPath, "%s%s", libPath, "/libget_message.so");
	
	FILE* file = fopen("/storage/emulated/0/Dokumente/DLtest.log.txt", "a");
	if (file == NULL) return (*env)->NewStringUTF(env, "Failed to open file!");
	fwrite(msg, strlen(msg), 1, file);
	fwrite(dlPath, strlen(dlPath), 1, file);
	fwrite("\n", 1, 1, file);
	fclose(file);

	handle = dlopen(dlPath, RTLD_LAZY);
	if (handle == NULL) return (*env)->NewStringUTF(env, "dlopen failed!");

	char* (*getMessage)();
	getMessage = dlsym(handle, "getMessage");
	if (getMessage == NULL) return (*env)->NewStringUTF(env, "dlsym failed!");

	file = fopen("/storage/emulated/0/Dokumente/DLtest.log.txt", "a");
	fwrite(getMessage(), strlen(getMessage()), 1, file);
	fclose(file);

	return (*env)->NewStringUTF(env, getMessage());
}