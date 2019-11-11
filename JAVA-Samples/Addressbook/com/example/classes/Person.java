package com.example.classes;

public class Person {
	private String name;
	private int age;

	public Person(String n) {
		name = n;
		age = 0;
	}

	public Person(String n, int a) {
		name = n;
		age = a;
	}

	public String getName() {
		return name;
	}

	public int getAge() {
		return age;
	}
}