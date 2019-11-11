/*	
	Compile: javac Addressbook.java
	Ausführen aus jar: java -jar classes.jar
*/

import com.example.classes.*;

public class Addressbook {
	public static void main(String[] args) {
		System.out.println("*** Addressbook ***");

		Person p  = new Person("Person1", 30);
		Person p2 = new Person("Person2");

		Gruppe g = new Gruppe();
		g.addPerson(p);
		g.addPerson(p2);
		g.listPersons();
	}
}