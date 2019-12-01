#include <stdio.h>
#include <string.h>
#include <time.h>
#include <dlfcn.h>

#include "android_native_app_glue.h"

#define VK_NO_PROTOTYPES
#include <vulkan/vulkan.h>

PFN_vkEnumerateInstanceVersion vkEnumerateInstanceVersion;
PFN_vkCreateInstance vkCreateInstance;

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

    uint32_t apiVersion;
	vkEnumerateInstanceVersion(&apiVersion);
	sprintf(buf, "Supported Api Version: %u.%u.%u\n", VK_VERSION_MAJOR(apiVersion), VK_VERSION_MINOR(apiVersion), VK_VERSION_PATCH(apiVersion));
    fwrite(buf, strlen(buf), 1, file);

    VkApplicationInfo app_info = {};
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pNext = NULL;
    app_info.pApplicationName = "VulkanApp";
    app_info.applicationVersion = 1;
    app_info.pEngineName = "VulkanEngine";
    app_info.engineVersion = 1;
    app_info.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo inst_info = {};
    inst_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    inst_info.pNext = NULL;
    inst_info.flags = 0;
    inst_info.pApplicationInfo = &app_info;
    inst_info.enabledExtensionCount = 0;
    inst_info.ppEnabledExtensionNames = NULL;
    inst_info.enabledLayerCount = 0;
    inst_info.ppEnabledLayerNames = NULL;

    VkInstance instance;
    VkResult result;

    result = vkCreateInstance(&inst_info, NULL, &instance);
    if (result == VK_ERROR_INCOMPATIBLE_DRIVER) {
        strcpy(buf, "cannot find a compatible Vulkan ICD\n");
        fwrite(buf, strlen(buf), 1, file);
        fclose(file);
        return;
    } else if (result) {
        strcpy(buf, "unknown error\n");
        fwrite(buf, strlen(buf), 1, file);
        fclose(file);
        return;
    }

    strcpy(buf, "Intance created.\n");
    fwrite(buf, strlen(buf), 1, file);
	fclose(file);

    while(1);
}