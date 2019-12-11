ANDROID_JAR=/home/felix/Android/Sdk/platforms/android-28/android.jar

all: NODEtest.apk

#lib/arm64-v8a/libnode.so: src/c/node.cpp
#	mkdir -p lib/arm64-v8a
#	aarch64-linux-android28-clang++ -shared -o lib/arm64-v8a/libnode.so src/c/node.cpp

obj/com/nodetest/NODEtest.class: src/com/nodetest/NODEtest.java
	mkdir -p obj/nodetest
	javac -d obj -bootclasspath $(ANDROID_JAR) src/com/nodetest/NODEtest.java

output.apk: AndroidManifest.xml
	aapt2 link -o output.apk -I $(ANDROID_JAR) --manifest AndroidManifest.xml 

classes.dex: obj/com/nodetest/NODEtest.class output.apk lib/arm64-v8a/libnode.so
	d8 obj/com/nodetest/NODEtest.class --lib $(ANDROID_JAR)
	cp ../../node_android/out/Release/lib.target/libnode.so.81 lib/arm64-v8a/libnode.so
	7z a output.apk classes.dex lib/

NODEtest_unsigned-aligned.apk: classes.dex
	rm -f NODEtest_unsigned-aligned.apk
	zipalign -v -p 4 output.apk NODEtest_unsigned-aligned.apk

NODEtest.apk: NODEtest_unsigned-aligned.apk
	apksigner sign --ks my-release-key.jks --out NODEtest.apk NODEtest_unsigned-aligned.apk

clean:
	rm -f *.apk lib/arm64-v8a/*.so obj/com/nodetest/*.class *.dex