#include <time.h>
#include <dlfcn.h>
#include "app.h"

PFN_vkEnumerateInstanceVersion vkEnumerateInstanceVersion;
PFN_vkCreateInstance vkCreateInstance;
PFN_vkEnumerateInstanceLayerProperties vkEnumerateInstanceLayerProperties;
PFN_vkEnumerateInstanceExtensionProperties vkEnumerateInstanceExtensionProperties;
PFN_vkCreateAndroidSurfaceKHR vkCreateAndroidSurfaceKHR;
PFN_vkEnumeratePhysicalDevices vkEnumeratePhysicalDevices;
PFN_vkEnumerateDeviceExtensionProperties vkEnumerateDeviceExtensionProperties;
PFN_vkGetPhysicalDeviceProperties vkGetPhysicalDeviceProperties;
PFN_vkGetPhysicalDeviceFeatures vkGetPhysicalDeviceFeatures;
PFN_vkGetPhysicalDeviceQueueFamilyProperties vkGetPhysicalDeviceQueueFamilyProperties;
PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR vkGetPhysicalDeviceSurfaceCapabilitiesKHR;
PFN_vkGetPhysicalDeviceSurfaceFormatsKHR vkGetPhysicalDeviceSurfaceFormatsKHR;
PFN_vkGetPhysicalDeviceSurfacePresentModesKHR vkGetPhysicalDeviceSurfacePresentModesKHR;
PFN_vkGetPhysicalDeviceSurfaceSupportKHR vkGetPhysicalDeviceSurfaceSupportKHR;

#define PRINT(...) \
sprintf(buf, ##__VA_ARGS__); \
fwrite(buf, strlen(buf), 1, file);

#define GET_FCN_PTR(fun) \
fun = (PFN_##fun)dlsym(libvulkan, #fun); \
if (!fun) \
{ \
    PRINT("Find Symbol '%s' failed!\n", #fun); \
    fclose(file); \
    return; \
}

int initialized_ = 0;
char buf[128];

void handle_cmd(struct android_app* app, int32_t cmd) {
  FILE* file = app->userData;
  switch (cmd) {
    case APP_CMD_INIT_WINDOW:
      // The window is being shown, get it ready.
      startApp(app, file);
      initialized_ = 1;
      fclose(file);
      break;
    case APP_CMD_TERM_WINDOW:
      // The window is being hidden or closed, clean it up.
      //terminate();
      break;
    default:
      PRINT("event not handled: %d", cmd);
  }
}

void android_main(struct android_app* state)
{
    time_t current_time;
    char* c_time_string;

    current_time = time(NULL);
    c_time_string = ctime(&current_time);

    FILE* file = fopen("/storage/emulated/0/Dokumente/VulkanApp.log.txt", "w");
	if (file == NULL) return;

    PRINT("\n==================\n*** Vulkan App ***\n==================\n\n")
    PRINT("%s\n", c_time_string)

    void* libvulkan = dlopen("libvulkan.so", RTLD_NOW | RTLD_LOCAL);
    if (!libvulkan)
    {
        PRINT("Loading libvulkan.so failed!\n")
        fclose(file);
        return;
    }

    GET_FCN_PTR(vkEnumerateInstanceVersion)
    GET_FCN_PTR(vkCreateInstance)
    GET_FCN_PTR(vkEnumerateInstanceLayerProperties)
    GET_FCN_PTR(vkEnumerateInstanceExtensionProperties)
    GET_FCN_PTR(vkCreateAndroidSurfaceKHR)
    GET_FCN_PTR(vkEnumeratePhysicalDevices)
    GET_FCN_PTR(vkEnumerateDeviceExtensionProperties)
    GET_FCN_PTR(vkGetPhysicalDeviceProperties)
    GET_FCN_PTR(vkGetPhysicalDeviceFeatures)
    GET_FCN_PTR(vkGetPhysicalDeviceQueueFamilyProperties)
    GET_FCN_PTR(vkGetPhysicalDeviceSurfaceCapabilitiesKHR)
    GET_FCN_PTR(vkGetPhysicalDeviceSurfaceFormatsKHR)
    GET_FCN_PTR(vkGetPhysicalDeviceSurfacePresentModesKHR)
    GET_FCN_PTR(vkGetPhysicalDeviceSurfaceSupportKHR)

    state->userData = file;
    state->onAppCmd = handle_cmd;

    int events;
    struct android_poll_source* source;
    do {
    if (ALooper_pollAll(initialized_ ? 1 : 0, NULL, &events,
                        (void**)&source) >= 0) {
        if (source != NULL) source->process(state, source);
    }
    } while (state->destroyRequested == 0);

    while(1);
}