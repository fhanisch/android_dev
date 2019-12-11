#include <jni.h>
#include <stdio.h>
#include <string.h>

char title[] = "./node";
char option[] = "-e";
char str[] = "var x=1";

char buf[] = "./node\0-e\0var x=1";

int argC = 3;
char *argV[3];

extern "C" JNIEXPORT jstring JNICALL Java_com_nodetest_NODEtest_nodeStart(JNIEnv* env, jobject thisObject)
{
	char msg[] = "*** node-Test!!! ***\n";
	char outStr[] = "node-Test erfolgreich.\n";

  argV[0] = buf;
  argV[1] = buf+strlen(title)+1;
  argV[2] = argV[1]+strlen(option)+1;

	FILE* file = fopen("/storage/emulated/0/Dokumente/nodeTest.log.txt", "a");
	if (file == NULL) return env->NewStringUTF("Failed to open file!");
	
	fwrite(msg, strlen(msg), 1, file);
	fwrite(outStr, strlen(outStr), 1, file);

  for (int i=0;i<argC;i++)
  {
      fwrite(argV[i], strlen(argV[i])+1, 1, file);
  }

	fclose(file);

  node::Start(argC, argV);

	return env->NewStringUTF(outStr);
}