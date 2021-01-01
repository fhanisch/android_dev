#ifndef PERSON_H
#define PERSON_H

class Person {
protected:
	char* name;
public:
	Person(char* n);
	~Person();
	char* getName();
	void setName(char* n);
};

#endif // !PERSON_H