all: HelloAndroid.apk

src\com\example\helloandroid\R.java: res\layout\activity_main.xml AndroidManifest.xml
	aapt package -f -m -J src -M AndroidManifest.xml -S res -I C:\Users\Felix\AppData\Local\Android\Sdk\platforms\android-28\android.jar

obj/com/example/helloandroid/MainActivity.class: src/com/example/helloandroid/MainActivity.java src/com/example/helloandroid/R.java
	javac -d obj -classpath src -bootclasspath C:\Users\Felix\AppData\Local\Android\Sdk\platforms\android-28\android.jar src/com/example/helloandroid/*.java

classes.dex: obj/com/example/helloandroid/MainActivity.class
	dx --dex --output=classes.dex obj

hello.unaligned.apk: classes.dex
	aapt package -f -m -F hello.unaligned.apk -M AndroidManifest.xml -S res -I C:\Users\Felix\AppData\Local\Android\Sdk\platforms\android-28\android.jar
	aapt add hello.unaligned.apk classes.dex

HelloAndroid_unsigned-aligned.apk: hello.unaligned.apk
	del HelloAndroid_unsigned-aligned.apk
	zipalign -v -p 4 hello.unaligned.apk HelloAndroid_unsigned-aligned.apk

HelloAndroid.apk: HelloAndroid_unsigned-aligned.apk
	apksigner sign --ks my-release-key.jks --out HelloAndroid.apk HelloAndroid_unsigned-aligned.apk