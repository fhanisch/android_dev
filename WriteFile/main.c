#include <stdio.h>
#include <string.h>
#include "android_native_app_glue.h"

/**
* Dies ist der Haupteinsprungspunkt einer systemeigenen Anwendung, die
* android_native_app_glue verwendet. Sie wird im eigenen Thread ausgef�hrt, mit ihrer eigenen
* Ereignisschleife zum Empfangen von Eingabeereignissen und Ausf�hren anderer Dinge.
*/
void android_main(struct android_app* state)
{
	char msg[] = "HalliHallo!!!\n";
	char str[] = "Das ist das Haus vom Nikolaus.\n";
	char newRow[] = "\n\n";
	const char* internalDataPath;

	internalDataPath = state->activity->internalDataPath;

	FILE* file = fopen("/storage/emulated/0/Dokumente/log.txt", "a");
	if (file == NULL) return;

	fwrite(msg, strlen(msg), 1, file);
	fwrite(str, strlen(str), 1, file);
	fwrite(internalDataPath, strlen(internalDataPath), 1, file);
	fwrite(newRow, strlen(newRow), 1, file);
	fclose(file);
	while (1);
}