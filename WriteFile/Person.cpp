#include "Person.h"

Person::Person(char* n) {
	name = n;
}

Person::~Person() {

}

void Person::setName(char* n) {
	name = n;
}

char* Person::getName() {
	return name;
}