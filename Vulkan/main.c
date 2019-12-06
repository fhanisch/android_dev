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
PFN_vkCreateDevice vkCreateDevice;
PFN_vkGetDeviceQueue vkGetDeviceQueue;
PFN_vkCreateSwapchainKHR vkCreateSwapchainKHR;
PFN_vkCreateCommandPool vkCreateCommandPool;
PFN_vkCreateImage vkCreateImage;
PFN_vkGetImageMemoryRequirements vkGetImageMemoryRequirements;
PFN_vkAllocateMemory vkAllocateMemory;
PFN_vkBindImageMemory vkBindImageMemory;
PFN_vkCreateImageView vkCreateImageView;
PFN_vkCmdPipelineBarrier vkCmdPipelineBarrier;
PFN_vkCmdCopyBufferToImage vkCmdCopyBufferToImage;
PFN_vkGetSwapchainImagesKHR vkGetSwapchainImagesKHR;
PFN_vkGetPhysicalDeviceMemoryProperties vkGetPhysicalDeviceMemoryProperties;
PFN_vkCreateBuffer vkCreateBuffer;
PFN_vkGetBufferMemoryRequirements vkGetBufferMemoryRequirements;
PFN_vkBindBufferMemory vkBindBufferMemory;
PFN_vkCmdCopyBuffer vkCmdCopyBuffer;
PFN_vkMapMemory vkMapMemory;
PFN_vkUnmapMemory vkUnmapMemory;
PFN_vkDestroyBuffer vkDestroyBuffer;
PFN_vkFreeMemory vkFreeMemory;
PFN_vkAllocateCommandBuffers vkAllocateCommandBuffers;
PFN_vkBeginCommandBuffer vkBeginCommandBuffer;
PFN_vkEndCommandBuffer vkEndCommandBuffer;
PFN_vkQueueSubmit vkQueueSubmit;
PFN_vkQueueWaitIdle vkQueueWaitIdle;
PFN_vkFreeCommandBuffers vkFreeCommandBuffers;
PFN_vkCreateRenderPass vkCreateRenderPass;
PFN_vkCreateDescriptorSetLayout vkCreateDescriptorSetLayout;
PFN_vkGetPhysicalDeviceFormatProperties vkGetPhysicalDeviceFormatProperties;
PFN_vkCreateFramebuffer vkCreateFramebuffer;
PFN_vkCreateSemaphore vkCreateSemaphore;

#define PRINT(...) \
sprintf(buf, ##__VA_ARGS__); \
fwrite(buf, strlen(buf), 1, file);

#define GET_FCN_PTR(fun) \
fun = (PFN_##fun)dlsym(libvulkan, #fun); \
if (!fun) \
{ \
    PRINT("Find Symbol '%s' failed!\n", #fun) \
    fclose(file); \
    return; \
}

static int initialized_ = 0;
static char buf[128];

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
      PRINT("Event not handled: %d\n", cmd)
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
    GET_FCN_PTR(vkCreateDevice)
    GET_FCN_PTR(vkGetDeviceQueue)
    GET_FCN_PTR(vkCreateSwapchainKHR)
    GET_FCN_PTR(vkCreateCommandPool)
    GET_FCN_PTR(vkCreateImage)
    GET_FCN_PTR(vkGetImageMemoryRequirements)
    GET_FCN_PTR(vkAllocateMemory)
    GET_FCN_PTR(vkBindImageMemory)
    GET_FCN_PTR(vkCreateImageView)
    GET_FCN_PTR(vkCmdPipelineBarrier)
    GET_FCN_PTR(vkCmdCopyBufferToImage)
    GET_FCN_PTR(vkGetSwapchainImagesKHR)
    GET_FCN_PTR(vkGetPhysicalDeviceMemoryProperties)
    GET_FCN_PTR(vkCreateBuffer)
    GET_FCN_PTR(vkGetBufferMemoryRequirements)
    GET_FCN_PTR(vkBindBufferMemory)
    GET_FCN_PTR(vkCmdCopyBuffer)
    GET_FCN_PTR(vkMapMemory)
    GET_FCN_PTR(vkUnmapMemory)
    GET_FCN_PTR(vkDestroyBuffer)
    GET_FCN_PTR(vkFreeMemory)
    GET_FCN_PTR(vkAllocateCommandBuffers)
    GET_FCN_PTR(vkBeginCommandBuffer)
    GET_FCN_PTR(vkEndCommandBuffer)
    GET_FCN_PTR(vkQueueSubmit)
    GET_FCN_PTR(vkQueueWaitIdle)
    GET_FCN_PTR(vkFreeCommandBuffers)
    GET_FCN_PTR(vkCreateRenderPass)
    GET_FCN_PTR(vkCreateDescriptorSetLayout)
    GET_FCN_PTR(vkGetPhysicalDeviceFormatProperties)
    GET_FCN_PTR(vkCreateFramebuffer)
    GET_FCN_PTR(vkCreateSemaphore)

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