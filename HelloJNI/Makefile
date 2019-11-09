all: HelloJNI.apk

lib/arm64-v8a/libnative-lib.so: src\c\native-lib.c
	aarch64-linux-android28-clang -shared -o lib/arm64-v8a/libnative-lib.so src\c\native-lib.c -landroid -llog

obj\com\hellojni\HelloJNI.class: src/com/hellojni/HelloJNI.java
	javac -d obj -classpath src -bootclasspath C:\Users\Felix\AppData\Local\Android\Sdk\platforms\android-28\android.jar src/com/hellojni/HelloJNI.java

output.apk: AndroidManifest.xml
	aapt2 link -o output.apk -I C:\Users\Felix\AppData\Local\Android\Sdk\platforms\android-28\android.jar --manifest AndroidManifest.xml 

classes.dex: obj\com\hellojni\HelloJNI.class output.apk lib/arm64-v8a/libnative-lib.so
	d8 obj\com\hellojni\HelloJNI.class --lib C:\Users\Felix\AppData\Local\Android\Sdk\platforms\android-28\android.jar
	7z a output.apk classes.dex lib/

HelloJNI_unsigned-aligned.apk: classes.dex
	del HelloJNI_unsigned-aligned.apk
	zipalign -v -p 4 output.apk HelloJNI_unsigned-aligned.apk

HelloJNI.apk: HelloJNI_unsigned-aligned.apk
	apksigner sign --ks my-release-key.jks --out HelloJNI.apk HelloJNI_unsigned-aligned.apk

clean:
	del *.apk lib\arm64-v8a\*.so