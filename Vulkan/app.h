#ifndef APP_H
#define APP_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "android_native_app_glue.h"

#define VK_NO_PROTOTYPES
#define VK_USE_PLATFORM_ANDROID_KHR
#include <vulkan/vulkan.h>

#ifdef __cplusplus
class App {
    char buf[128];
    struct android_app* state;
    FILE* file;
    VkInstance instance;
    VkSurfaceKHR surface;
    

    void printSupportedApiVersion();
	void printLayers();
	void printExtensions();
	void createInstance();
    void createSurface();
    void printPhysicalDevices();
public:
    App(struct android_app* s, FILE* f);
    ~App();
    void run();
};
#endif

#ifdef __cplusplus
extern "C" {
#endif

void startApp(struct android_app* state, FILE* file);

#ifdef __cplusplus
}
#endif


#endif  // APP_H