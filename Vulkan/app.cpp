#include "app.h"

extern PFN_vkEnumerateInstanceVersion vkEnumerateInstanceVersion;
extern PFN_vkCreateInstance vkCreateInstance;
extern PFN_vkEnumerateInstanceLayerProperties vkEnumerateInstanceLayerProperties;
extern PFN_vkEnumerateInstanceExtensionProperties vkEnumerateInstanceExtensionProperties;
extern PFN_vkCreateAndroidSurfaceKHR vkCreateAndroidSurfaceKHR;

App::App(struct android_app* s, FILE* f)
{
    state = s;
    file = f;
}
App::~App()
{

}

void App::printSupportedApiVersion()
{
    uint32_t apiVersion;
	vkEnumerateInstanceVersion(&apiVersion);
	sprintf(buf, "Supported Api Version: %u.%u.%u\n", VK_VERSION_MAJOR(apiVersion), VK_VERSION_MINOR(apiVersion), VK_VERSION_PATCH(apiVersion));
    fwrite(buf, strlen(buf), 1, file);
}

void App::printLayers()
{
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
	VkLayerProperties *availableLayers = new VkLayerProperties[layerCount];
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers);
	sprintf(buf, "%s (%u)\n", "Available Instance Layers: ", layerCount);
    fwrite(buf, strlen(buf), 1, file);
	for (uint32_t i = 0; i < layerCount; i++) {
        sprintf(buf, "\t#%u\t%s\n", i, availableLayers[i].layerName);
        fwrite(buf, strlen(buf), 1, file);
	}
	delete[] availableLayers;
}

void App::printExtensions()
{
	uint32_t extensionCount;
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
	VkExtensionProperties *extensions = new VkExtensionProperties[extensionCount];
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions);
	//std::cout << "Available Intance Extensions:" << std::endl;
    sprintf(buf, "%s (%u)\n", "Available Instance Extensions: ", extensionCount);
    fwrite(buf, strlen(buf), 1, file);
	for (uint32_t i = 0; i < extensionCount; i++) {
		//std::cout << "\t#" << i << "\t" << extensions[i].extensionName << std::endl;
        sprintf(buf, "\t#%u\t%s\n", i, extensions[i].extensionName);
        fwrite(buf, strlen(buf), 1, file);
	}
	delete[] extensions;
}

void App::createInstance()
{
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

    VkResult result = vkCreateInstance(&inst_info, nullptr, &instance);
    if (result == VK_ERROR_INCOMPATIBLE_DRIVER) {
        strcpy(buf, "cannot find a compatible Vulkan ICD\n");
        fwrite(buf, strlen(buf), 1, file);
        fclose(file);
        return; //TODO: terminate app
    } else if (result) {
        strcpy(buf, "unknown error\n");
        fwrite(buf, strlen(buf), 1, file);
        fclose(file);
        return; //TODO: terminate app
    }

    strcpy(buf, "Instance created.\n");
    fwrite(buf, strlen(buf), 1, file);
}

void App::createSurface()
{
    VkAndroidSurfaceCreateInfoKHR createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR;
    createInfo.pNext = NULL;
    createInfo.flags = 0;
    createInfo.window = (ANativeWindow*)(state->window);

    VkResult result = vkCreateAndroidSurfaceKHR(instance, &createInfo, nullptr, &surface);
    if (result != VK_SUCCESS)
    {
        strcpy(buf, "Failed to create window surface!\n");
        fwrite(buf, strlen(buf), 1, file);
        exit(1);
    }

    strcpy(buf, "Surface created.\n");
    fwrite(buf, strlen(buf), 1, file);
}

void App::run()
{
    printSupportedApiVersion();
    printLayers();
    printExtensions();
    createInstance();
    createSurface();
}

void startApp(struct android_app* state, FILE* file)
{
    App app(state, file);
    app.run();
}