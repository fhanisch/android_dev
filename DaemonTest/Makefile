all: DaemonTest.apk

lib/arm64-v8a/libnative-lib.so: src\c\native-lib.c
	aarch64-linux-android28-clang -shared -o lib/arm64-v8a/libnative-lib.so src\c\native-lib.c

obj\com\daemontest\DaemonTest.class: src/com/daemontest/DaemonTest.java
	javac -d obj -bootclasspath C:\Users\Felix\AppData\Local\Android\Sdk\platforms\android-28\android.jar src/com/daemontest/DaemonTest.java

output.apk: AndroidManifest.xml
	aapt2 link -o output.apk -I C:\Users\Felix\AppData\Local\Android\Sdk\platforms\android-28\android.jar --manifest AndroidManifest.xml 

classes.dex: obj\com\daemontest\DaemonTest.class output.apk lib/arm64-v8a/libnative-lib.so
	d8 obj\com\daemontest\DaemonTest.class --lib C:\Users\Felix\AppData\Local\Android\Sdk\platforms\android-28\android.jar
	7z a output.apk classes.dex lib/

DaemonTest_unsigned-aligned.apk: classes.dex
	del DaemonTest_unsigned-aligned.apk
	zipalign -v -p 4 output.apk DaemonTest_unsigned-aligned.apk

DaemonTest.apk: DaemonTest_unsigned-aligned.apk
	apksigner sign --ks my-release-key.jks --out DaemonTest.apk DaemonTest_unsigned-aligned.apk

clean:
	del *.apk lib\arm64-v8a\*.so obj\com\daemontest\*.class *.dex