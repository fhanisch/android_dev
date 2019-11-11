all:
	javac Addressbook.java
	jar cvfm classes.jar MANIFEST.MF com\example\classes\*.class Addressbook.class
	jar cvf testpack.jar com\example\classes\*.class
#javac Addressbook.java -cp testpack.jar

clean:
	del *.class com\example\classes\*.class