all: WriteFile.apk

lib/arm64-v8a/libWriteFile.so: main.c
	aarch64-linux-android28-clang -shared -o lib/arm64-v8a/libWriteFile.so main.c android_native_app_glue.c -landroid -llog

output.apk: lib/arm64-v8a/libWriteFile.so AndroidManifest.xml
	aapt2 link -o output.apk -I C:\Users\Felix\AppData\Local\Android\Sdk\platforms\android-28\android.jar --manifest AndroidManifest.xml
	7z a output.apk lib/

WriteFile_unsigned-aligned.apk: output.apk
	del WriteFile_unsigned-aligned.apk
	zipalign -v -p 4 output.apk WriteFile_unsigned-aligned.apk

WriteFile.apk: WriteFile_unsigned-aligned.apk
	apksigner sign --ks my-release-key.jks --out WriteFile.apk WriteFile_unsigned-aligned.apk

clean:
	del WriteFile.apk WriteFile_unsigned-aligned.apk output.apk lib\arm64-v8a\libWriteFile.so