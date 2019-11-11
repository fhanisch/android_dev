package com.example.classes;

import java.util.ArrayList;

public class Gruppe {
	ArrayList<Person> persons = new ArrayList<Person>();

	public void addPerson(Person p) {
		persons.add(p);
	}

	public void listPersons() {
		for (int i=0;i<persons.size();i++) {
			System.out.println(persons.get(i).getName() + ", " + persons.get(i).getAge());
		}
	}
}