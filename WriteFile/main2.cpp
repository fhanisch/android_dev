#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "android_native_app_glue.h"
#include "Person.h"
#include <dlfcn.h>

/**
* Dies ist der Haupteinsprungspunkt einer systemeigenen Anwendung, die
* android_native_app_glue verwendet. Sie wird im eigenen Thread ausgef�hrt, mit ihrer eigenen
* Ereignisschleife zum Empfangen von Eingabeereignissen und Ausf�hren anderer Dinge.
*/


extern "C"
{
	char* getStr();
}

char msg[] = "HalliHallo!!!\n";
char str[] = "Das ist das Haus vom Nikolaus und nebenan vom Weihnachtsmann.\n";
char newRow[] = "\n\n";

class App {
protected:
	const char* internalDataPath;
	FILE* file;
public:
	App(const char* dataPath) {
		internalDataPath = dataPath;
		file = fopen("/storage/emulated/0/Dokumente/log.txt", "a");
		if (file == NULL) exit(1);
		fwrite(msg, strlen(msg), 1, file);
		fwrite(str, strlen(str), 1, file);		
	}

	~App() {
		fwrite(newRow, strlen(newRow), 1, file);
		fclose(file);
	}

	void start() {
		Person* person1 = new Person((char*)"Heinz");
		Person* person2 = new Person((char*)"Emma");
		
		fwrite(internalDataPath, strlen(internalDataPath), 1, file);
		fwrite("\n", strlen("\n"), 1, file);
		fwrite(getStr() , strlen(getStr()), 1, file);
		fwrite(person1->getName(), strlen(person1->getName()), 1, file);
		fwrite("\n", strlen("\n"), 1, file);
		fwrite(person2->getName(), strlen(person2->getName()), 1, file);
	}
};

void android_main(struct android_app* state)
{
	App* app = new App(state->activity->internalDataPath);
	app->start();
	delete app;
	while (1);
}