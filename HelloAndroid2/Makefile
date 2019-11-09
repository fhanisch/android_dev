all: HelloAndroid2.apk

obj\com\helloandroid\HelloAndroid2.class: src/com/helloandroid/HelloAndroid2.java
	javac -d obj -classpath src -bootclasspath C:\Users\Felix\AppData\Local\Android\Sdk\platforms\android-28\android.jar src/com/helloandroid/HelloAndroid2.java

output.apk: AndroidManifest.xml
	aapt2 link -o output.apk -I C:\Users\Felix\AppData\Local\Android\Sdk\platforms\android-28\android.jar --manifest AndroidManifest.xml 

classes.dex: obj\com\helloandroid\HelloAndroid2.class output.apk
	d8 obj\com\helloandroid\HelloAndroid2.class --lib C:\Users\Felix\AppData\Local\Android\Sdk\platforms\android-28\android.jar
	7z a output.apk classes.dex

HelloAndroid2_unsigned-aligned.apk: classes.dex
	del HelloAndroid2_unsigned-aligned.apk
	zipalign -v -p 4 output.apk HelloAndroid2_unsigned-aligned.apk

HelloAndroid2.apk: HelloAndroid2_unsigned-aligned.apk
	apksigner sign --ks my-release-key.jks --out HelloAndroid2.apk HelloAndroid2_unsigned-aligned.apk

clean:
	del *.apk