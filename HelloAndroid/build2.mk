all: HelloAndroid.apk

layout_activity_main.xml.flat values_strings.arsc.flat: res\layout\activity_main.xml res\values\strings.xml
	aapt2 compile res\layout\activity_main.xml res\values\strings.xml -o .

output.apk src\com\example\helloandroid\R.java: AndroidManifest.xml layout_activity_main.xml.flat values_strings.arsc.flat
	aapt2 link -o output.apk -I C:\Users\Felix\AppData\Local\Android\Sdk\platforms\android-28\android.jar --manifest AndroidManifest.xml layout_activity_main.xml.flat values_strings.arsc.flat --java src 

obj\com\example\helloandroid\MainActivity.class: src/com/example/helloandroid/MainActivity.java src\com\example\helloandroid\R.java
	javac -d obj -bootclasspath C:\Users\Felix\AppData\Local\Android\Sdk\platforms\android-28\android.jar src/com/example/helloandroid/*.java

classes.dex: obj\com\example\helloandroid\MainActivity.class output.apk
	d8 obj\com\example\helloandroid\*.class --lib C:\Users\Felix\AppData\Local\Android\Sdk\platforms\android-28\android.jar
	7z a output.apk classes.dex

HelloAndroid_unsigned-aligned.apk: classes.dex
	del HelloAndroid_unsigned-aligned.apk
	zipalign -v -p 4 output.apk HelloAndroid_unsigned-aligned.apk

HelloAndroid.apk: HelloAndroid_unsigned-aligned.apk
	apksigner sign --ks my-release-key.jks --out HelloAndroid.apk HelloAndroid_unsigned-aligned.apk

clean:
	del *.apk del obj\com\example\helloandroid\*.class *.flat *.dex src\com\example\helloandroid\R.java