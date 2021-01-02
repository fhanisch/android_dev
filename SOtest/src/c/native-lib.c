#include <jni.h>
#include <stdio.h>
#include <string.h>

char* getStr();

JNIEXPORT jstring JNICALL Java_com_sotest_SOtest_stringFromJNI(JNIEnv* env, jobject thisObject, jstring jLibPath)
{
	char msg[] = "*** SharedLibrary-Test ***\n\n";
	const char* libPath = (*env)->GetStringUTFChars(env, jLibPath, NULL);
	
	FILE* file = fopen("/storage/emulated/0/Dokumente/SOtest.log.txt", "a");
	if (file == NULL) return (*env)->NewStringUTF(env, "Failed to open file!");

	fwrite(msg, strlen(msg), 1, file);
	fwrite(libPath, strlen(libPath), 1, file);
	fwrite("\n", 1, 1, file);
	fclose(file);

	file = fopen("/storage/emulated/0/Dokumente/SOtest.log.txt", "a");
	fwrite(getStr(), strlen(getStr()), 1, file);
	fclose(file);

	return (*env)->NewStringUTF(env, getStr());
}