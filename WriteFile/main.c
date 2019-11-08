#include <stdio.h>
#include <string.h>
#include "android_native_app_glue.h"

/**
* Dies ist der Haupteinsprungspunkt einer systemeigenen Anwendung, die
* android_native_app_glue verwendet. Sie wird im eigenen Thread ausgeführt, mit ihrer eigenen
* Ereignisschleife zum Empfangen von Eingabeereignissen und Ausführen anderer Dinge.
*/
void android_main(struct android_app* state)
{
	char msg[] = "HalliHallo!!!\n";
	char str[] = "Das ist das Haus vom Nikolaus.\n";
	FILE* file = fopen("/storage/emulated/0/Dokumente/log.txt", "a");
	fwrite(msg, strlen(msg), 1, file);
	fwrite(str, strlen(str), 1, file);
	fclose(file);
	while (1);
}