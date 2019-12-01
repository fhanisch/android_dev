#include "app.h"

extern PFN_vkEnumerateInstanceVersion vkEnumerateInstanceVersion;
extern PFN_vkCreateInstance vkCreateInstance;
extern PFN_vkEnumerateInstanceLayerProperties vkEnumerateInstanceLayerProperties;
extern PFN_vkEnumerateInstanceExtensionProperties vkEnumerateInstanceExtensionProperties;

App::App()
{

}
App::~App()
{

}

char buf[128];

void printLayers(FILE* file)
{
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
	VkLayerProperties *availableLayers = new VkLayerProperties[layerCount];
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers);
	sprintf(buf, "%s (%u)\n", "Available Intance Layers: ", layerCount);
    fwrite(buf, strlen(buf), 1, file);
	for (uint32_t i = 0; i < layerCount; i++) {
        sprintf(buf, "\t#%u\t%s\n", i, availableLayers[i].layerName);
        fwrite(buf, strlen(buf), 1, file);
	}
	delete[] availableLayers;
}

void printExtensions(FILE* file)
{
	uint32_t extensionCount;
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
	VkExtensionProperties *extensions = new VkExtensionProperties[extensionCount];
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions);
	//std::cout << "Available Intance Extensions:" << std::endl;
    sprintf(buf, "%s (%u)\n", "Available Intance Extensions: ", extensionCount);
    fwrite(buf, strlen(buf), 1, file);
	for (uint32_t i = 0; i < extensionCount; i++) {
		//std::cout << "\t#" << i << "\t" << extensions[i].extensionName << std::endl;
        sprintf(buf, "\t#%u\t%s\n", i, extensions[i].extensionName);
        fwrite(buf, strlen(buf), 1, file);
	}
	delete[] extensions;
}

void startApp(FILE* file)
{
    uint32_t apiVersion;
	vkEnumerateInstanceVersion(&apiVersion);
	sprintf(buf, "Supported Api Version: %u.%u.%u\n", VK_VERSION_MAJOR(apiVersion), VK_VERSION_MINOR(apiVersion), VK_VERSION_PATCH(apiVersion));
    fwrite(buf, strlen(buf), 1, file);

    printLayers(file);
    printExtensions(file);

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
}