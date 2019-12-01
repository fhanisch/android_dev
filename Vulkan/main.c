#include <time.h>
#include <dlfcn.h>

#include "android_native_app_glue.h"

#include "app.h"

PFN_vkEnumerateInstanceVersion vkEnumerateInstanceVersion;
PFN_vkCreateInstance vkCreateInstance;
PFN_vkEnumerateInstanceLayerProperties vkEnumerateInstanceLayerProperties;
PFN_vkEnumerateInstanceExtensionProperties vkEnumerateInstanceExtensionProperties;

void android_main(struct android_app* state)
{
    char buf[128];
    time_t current_time;
    char* c_time_string;

    current_time = time(NULL);
    c_time_string = ctime(&current_time);

    FILE* file = fopen("/storage/emulated/0/Dokumente/VulkanApp.log.txt", "a");
	if (file == NULL) return;

    strcpy(buf, "*** Vulkan App ***\n==================\n\n");
    fwrite(buf, strlen(buf), 1, file);
    fwrite(c_time_string, strlen(c_time_string), 1, file);
    strcpy(buf, "\n");
    fwrite(buf, strlen(buf), 1, file);

    void* libvulkan = dlopen("libvulkan.so", RTLD_NOW | RTLD_LOCAL);
    if (!libvulkan)
    {
        strcpy(buf, "Loading libvulkan.so failed!\n");
        fwrite(buf, strlen(buf), 1, file);
        fclose(file);
        return;
    }

    vkEnumerateInstanceVersion = (PFN_vkEnumerateInstanceVersion)dlsym(libvulkan, "vkEnumerateInstanceVersion");
    if (!vkEnumerateInstanceVersion)
    {
        strcpy(buf, "Find Symbol 'vkEnumerateInstanceVersion' failed!\n");
        fwrite(buf, strlen(buf), 1, file);
        fclose(file);
        return;
    }

    vkCreateInstance = (PFN_vkCreateInstance)dlsym(libvulkan, "vkCreateInstance");
    if (!vkCreateInstance)
    {
        strcpy(buf, "Find Symbol 'vkCreateInstance' failed!\n");
        fwrite(buf, strlen(buf), 1, file);
        fclose(file);
        return;
    }

    vkEnumerateInstanceLayerProperties = (PFN_vkEnumerateInstanceLayerProperties)dlsym(libvulkan, "vkEnumerateInstanceLayerProperties");
    if (!vkEnumerateInstanceLayerProperties)
    {
        strcpy(buf, "Find Symbol 'vkEnumerateInstanceLayerProperties' failed!\n");
        fwrite(buf, strlen(buf), 1, file);
        fclose(file);
        return;
    }

    vkEnumerateInstanceExtensionProperties = (PFN_vkEnumerateInstanceExtensionProperties)dlsym(libvulkan, "vkEnumerateInstanceExtensionProperties");
    if (!vkEnumerateInstanceExtensionProperties)
    {
        strcpy(buf, "Find Symbol 'vkEnumerateInstanceExtensionProperties' failed!\n");
        fwrite(buf, strlen(buf), 1, file);
        fclose(file);
        return;
    }

    startApp(file);

	fclose(file);

    while(1);
}