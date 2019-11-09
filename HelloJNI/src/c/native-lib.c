#include <jni.h>
#include <stdio.h>
#include <string.h>

JNIEXPORT jstring JNICALL Java_com_hellojni_HelloJNI_stringFromJNI(JNIEnv* env, jobject thisObject)
{
	char msg[] = "HalliHallo JNI!!!\n";
	char str[] = "Das ist das Haus vom Nikolaus.\n";
	//FILE* file = fopen("/storage/emulated/0/Dokumente/HelloJNI.log", "a");
	//fwrite(msg, strlen(msg), 1, file);
	//fwrite(str, strlen(str), 1, file);
	//fclose(file);

	return (*env)->NewStringUTF(env, str);
}