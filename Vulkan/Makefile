ANDROID_JAR=/home/felix/Android/Sdk/platforms/android-28/android.jar

all: VulkanApp.apk

lib/arm64-v8a/libVulkanApp.so: main.c app.cpp app.h
	mkdir -p lib/arm64-v8a;
	aarch64-linux-android28-clang -c main.c android_native_app_glue.c -Wall
	aarch64-linux-android28-clang++ -c app.cpp -Wall
	aarch64-linux-android28-clang++ -shared -o lib/arm64-v8a/libVulkanApp.so main.o android_native_app_glue.o app.o -landroid -llog -ldl -Wall

output.apk: lib/arm64-v8a/libVulkanApp.so AndroidManifest.xml
	aapt2 link -o output.apk -I $(ANDROID_JAR) --manifest AndroidManifest.xml
	7z a output.apk lib/

VulkanApp_unsigned-aligned.apk: output.apk
	rm -f VulkanApp_unsigned-aligned.apk
	zipalign -v -p 4 output.apk VulkanApp_unsigned-aligned.apk

VulkanApp.apk: VulkanApp_unsigned-aligned.apk
	apksigner sign --ks my-release-key.jks --out VulkanApp.apk VulkanApp_unsigned-aligned.apk

clean:
	rm -f *.apk *.o lib/arm64-v8a/*.so