ANDROID_JAR=C:/Users/Felix/AppData/Local/Android/Sdk/platforms/android-28/android.jar

all: DLtest.apk

lib/arm64-v8a/libnative-lib.so: src/c/native-lib.c
	aarch64-linux-android28-clang -shared -o lib/arm64-v8a/libnative-lib.so src/c/native-lib.c -ldl
	aarch64-linux-android28-clang -shared -o lib/arm64-v8a/libget_message.so src/c/get_message.c
	armv7a-linux-androideabi19-clang -shared -o lib/armeabi-v7a/libnative-lib.so src/c/native-lib.c -ldl
	armv7a-linux-androideabi19-clang -shared -o lib/armeabi-v7a/libget_message.so src/c/get_message.c

obj/com/dltest/DLtest.class: src/com/dltest/DLtest.java
	javac -d obj -bootclasspath $(ANDROID_JAR) src/com/dltest/DLtest.java

output.apk: AndroidManifest.xml
	aapt2 link -o output.apk -I $(ANDROID_JAR) --manifest AndroidManifest.xml 

classes.dex: obj/com/dltest/DLtest.class output.apk lib/arm64-v8a/libnative-lib.so
	d8 obj/com/dltest/DLtest.class --lib $(ANDROID_JAR)
	7z a output.apk classes.dex lib/

DLtest_unsigned-aligned.apk: classes.dex
	del DLtest_unsigned-aligned.apk
	zipalign -v -p 4 output.apk DLtest_unsigned-aligned.apk

DLtest.apk: DLtest_unsigned-aligned.apk
	apksigner sign --ks my-release-key.jks --out DLtest.apk DLtest_unsigned-aligned.apk

clean:
	rm -f *.apk lib/arm64-v8a/*.so lib/armeabi-v7a/*.so obj/com/dltest/*.class *.dex