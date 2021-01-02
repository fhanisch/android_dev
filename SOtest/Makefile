ANDROID_JAR=C:/Users/Felix/AppData/Local/Android/Sdk/platforms/android-28/android.jar

all: SOtest.apk

lib/arm64-v8a/libnative-lib.so: src/c/native-lib.c
	aarch64-linux-android28-clang -shared -o lib/arm64-v8a/libtest.so src/c/sl.c
	aarch64-linux-android28-clang -shared -o lib/arm64-v8a/libnative-lib.so src/c/native-lib.c -ltest -L./lib/arm64-v8a
	armv7a-linux-androideabi19-clang -shared -o lib/armeabi-v7a/libtest.so src/c/sl.c
	armv7a-linux-androideabi19-clang -shared -o lib/armeabi-v7a/libnative-lib.so src/c/native-lib.c -ltest -L./lib/armeabi-v7a

obj/com/sotest/SOtest.class: src/com/sotest/SOtest.java
	javac -d obj -bootclasspath $(ANDROID_JAR) src/com/sotest/SOtest.java

output.apk: AndroidManifest.xml
	aapt2 link -o output.apk -I $(ANDROID_JAR) --manifest AndroidManifest.xml 

classes.dex: obj/com/sotest/SOtest.class output.apk lib/arm64-v8a/libnative-lib.so
	d8 obj/com/sotest/SOtest.class --lib $(ANDROID_JAR)
	7z a output.apk classes.dex lib/

SOtest_unsigned-aligned.apk: classes.dex
	del SOtest_unsigned-aligned.apk
	zipalign -v -p 4 output.apk SOtest_unsigned-aligned.apk

SOtest.apk: SOtest_unsigned-aligned.apk
	apksigner sign --ks my-release-key.jks --out SOtest.apk SOtest_unsigned-aligned.apk

clean:
	rm -f *.apk lib/arm64-v8a/*.so lib/armeabi-v7a/*.so obj/com/sotest/*.class *.dex