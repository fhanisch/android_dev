#include "app.h"

extern PFN_vkEnumerateInstanceVersion vkEnumerateInstanceVersion;
extern PFN_vkCreateInstance vkCreateInstance;
extern PFN_vkEnumerateInstanceLayerProperties vkEnumerateInstanceLayerProperties;
extern PFN_vkEnumerateInstanceExtensionProperties vkEnumerateInstanceExtensionProperties;
extern PFN_vkCreateAndroidSurfaceKHR vkCreateAndroidSurfaceKHR;
extern PFN_vkEnumeratePhysicalDevices vkEnumeratePhysicalDevices;
extern PFN_vkEnumerateDeviceExtensionProperties vkEnumerateDeviceExtensionProperties;
extern PFN_vkGetPhysicalDeviceProperties vkGetPhysicalDeviceProperties;
extern PFN_vkGetPhysicalDeviceFeatures vkGetPhysicalDeviceFeatures;
extern PFN_vkGetPhysicalDeviceQueueFamilyProperties vkGetPhysicalDeviceQueueFamilyProperties;
extern PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR vkGetPhysicalDeviceSurfaceCapabilitiesKHR;
extern PFN_vkGetPhysicalDeviceSurfaceFormatsKHR vkGetPhysicalDeviceSurfaceFormatsKHR;
extern PFN_vkGetPhysicalDeviceSurfacePresentModesKHR vkGetPhysicalDeviceSurfacePresentModesKHR;
extern PFN_vkGetPhysicalDeviceSurfaceSupportKHR vkGetPhysicalDeviceSurfaceSupportKHR;

#define PRINT(...) \
sprintf(buf, ##__VA_ARGS__); \
fwrite(buf, strlen(buf), 1, file);

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
    sprintf(buf, "%s (%u)\n", "Available Instance Extensions: ", extensionCount);
    fwrite(buf, strlen(buf), 1, file);
	for (uint32_t i = 0; i < extensionCount; i++) {
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

	const unsigned int globalExtensionCount = 2;
	const char *globalExtensions[] = { VK_KHR_SURFACE_EXTENSION_NAME, VK_KHR_ANDROID_SURFACE_EXTENSION_NAME };
    VkInstanceCreateInfo inst_info = {};
    inst_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    inst_info.pNext = NULL;
    inst_info.flags = 0;
    inst_info.pApplicationInfo = &app_info;
    inst_info.enabledExtensionCount = globalExtensionCount;
    inst_info.ppEnabledExtensionNames = globalExtensions;
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

void App::printPhysicalDevices()
{
	if (instance == VK_NULL_HANDLE) {
		PRINT("%s\n","Create Instance first!");
		return;
	}
	uint32_t deviceCount;
	vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
	if (deviceCount == 0) {
		PRINT("%s\n","Failed to find GPUs with Vulkan support!");
		return;
	}
	VkPhysicalDevice *devices = new VkPhysicalDevice[deviceCount];
	vkEnumeratePhysicalDevices(instance, &deviceCount, devices);
    
	for (uint32_t i = 0; i < deviceCount; i++) {
		uint32_t extensionCount;
		vkEnumerateDeviceExtensionProperties(devices[i], nullptr, &extensionCount, nullptr);
		VkExtensionProperties *extensions = new VkExtensionProperties[extensionCount];
		vkEnumerateDeviceExtensionProperties(devices[i], nullptr, &extensionCount, extensions);
		PRINT("Device #%u Extensions:\n", i);
		for (uint32_t j = 0; j < extensionCount; j++) {
			PRINT("\t#%u\t%s\n", j, extensions[j].extensionName);
		}
		delete[] extensions;
        
		VkPhysicalDeviceProperties deviceProperties;
		vkGetPhysicalDeviceProperties(devices[i], &deviceProperties);
		PRINT("Device #%u Properties:\n", i);
		PRINT("\tApi Version:    %u\n", deviceProperties.apiVersion);
		PRINT("\tDevice ID:      %u\n", deviceProperties.deviceID);
		PRINT("\tDevice Name:    %s\n", deviceProperties.deviceName);
		PRINT("\tDevice Type:    %u\n", deviceProperties.deviceType);
		PRINT("\tDriver Version: %u\n", deviceProperties.driverVersion);
		PRINT("\tVendor ID:      %u\n", deviceProperties.vendorID);
		
		VkPhysicalDeviceFeatures deviceFeatures;
		vkGetPhysicalDeviceFeatures(devices[i], &deviceFeatures);
		PRINT("Device #%u Featers:\n", i);
		PRINT("\tTessellation Shader: %u\n", deviceFeatures.tessellationShader);
		PRINT("\tGeometry Shader:     %u\n", deviceFeatures.geometryShader);
		PRINT("\tfullDrawIndexUint32: %u\n", deviceFeatures.fullDrawIndexUint32);
		
		uint32_t queueFamilyCount;
		vkGetPhysicalDeviceQueueFamilyProperties(devices[i], &queueFamilyCount, nullptr);
		VkQueueFamilyProperties *queueFamilies = new VkQueueFamilyProperties[queueFamilyCount];
		vkGetPhysicalDeviceQueueFamilyProperties(devices[i], &queueFamilyCount, queueFamilies);
		PRINT("Device #%u Queue Families: (%u)\n", i, queueFamilyCount);
		for (uint32_t j = 0; j < queueFamilyCount; j++)
		{
			PRINT("\tQueue Family #%u:\n", j);
			PRINT("\t\tVK_QUEUE_GRAPHICS_BIT:           %u\n",  ((queueFamilies[j].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0));
			PRINT("\t\tVK_QUEUE_COMPUTE_BIT:            %u\n",  ((queueFamilies[j].queueFlags & VK_QUEUE_COMPUTE_BIT) != 0));
			PRINT("\t\tVK_QUEUE_TRANSFER_BIT:           %u\n",  ((queueFamilies[j].queueFlags & VK_QUEUE_TRANSFER_BIT) != 0));
			PRINT("\t\tVK_QUEUE_SPARSE_BINDING_BIT:     %u\n",  ((queueFamilies[j].queueFlags & VK_QUEUE_SPARSE_BINDING_BIT) != 0));
			PRINT("\t\tQueue Count:                     %u\n",  queueFamilies[j].queueCount);
			PRINT("\t\tTimestamp Valid Bits:            %u\n",  queueFamilies[j].timestampValidBits);
			uint32_t width = queueFamilies[j].minImageTransferGranularity.width;
			uint32_t height = queueFamilies[j].minImageTransferGranularity.height;
			uint32_t depth = queueFamilies[j].minImageTransferGranularity.depth;
			PRINT("\t\tMin Image Timestamp Granularity (width, height, depth): %u, %u, %u\n", width, height, depth);
		}
		delete[] queueFamilies;
		
		VkSurfaceCapabilitiesKHR capabilities;
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(devices[i], surface, &capabilities);
		PRINT("Device #%u Surface capabilities:\n", i);
		PRINT("\tminImageCount:           %u\n", capabilities.minImageCount);
		PRINT("\tmaxImageCount:           %u\n", capabilities.maxImageCount);
		PRINT("\tcurrentExtent:           %u, %u\n", capabilities.currentExtent.width, capabilities.currentExtent.height);
		PRINT("\tminImageExtent:          %u, %u\n", capabilities.minImageExtent.width, capabilities.minImageExtent.height);
		PRINT("\tmaxImageExtent:          %u, %u\n", capabilities.maxImageExtent.width, capabilities.maxImageExtent.height);
		PRINT("\tmaxImageArrayLayers:     %u\n", capabilities.maxImageArrayLayers);
		PRINT("\tsupportedTransforms:     %u\n", capabilities.supportedTransforms);
		PRINT("\tcurrentTransform:        %u\n", capabilities.currentTransform);
		PRINT("\tsupportedCompositeAlpha: %u\n", capabilities.supportedCompositeAlpha);
		PRINT("\tsupportedUsageFlags:     %u\n", capabilities.supportedUsageFlags);
		
		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(devices[i], surface, &formatCount, nullptr);
		VkSurfaceFormatKHR *formats = new VkSurfaceFormatKHR[formatCount];
		vkGetPhysicalDeviceSurfaceFormatsKHR(devices[i], surface, &formatCount, formats);
		PRINT("Device #%u Supported formats:\n", i);
		for (uint32_t j = 0; j < formatCount; j++)
		{
			PRINT("\tFormat #%u:\n", j);
			PRINT("\t\tformat:     %u\n", formats[j].format);
			PRINT("\t\tcolorSpace: %u\n", formats[j].colorSpace);
		}
		delete[] formats;
		
		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(devices[i], surface, &presentModeCount, nullptr);
		VkPresentModeKHR *presentModes = new VkPresentModeKHR[presentModeCount];
		vkGetPhysicalDeviceSurfacePresentModesKHR(devices[i], surface, &presentModeCount, presentModes);
		PRINT("Device #%u supported presentation modes:\n", i);
		for (uint32_t j = 0; j < presentModeCount; j++)
		{
			PRINT("\tPresent mode #%u: %u\n", j, presentModes[j]);
		}
		delete[] presentModes;
	}
	delete[] devices;
}

void App::choosePhysicalDevice()
{
	uint32_t deviceCount;
	vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
	if (deviceCount == 0) {
		PRINT("Failed to find GPUs with Vulkan support!\n");
		exit(1);
	}
	VkPhysicalDevice *devices = new VkPhysicalDevice[deviceCount];
	vkEnumeratePhysicalDevices(instance, &deviceCount, devices);
	for (unsigned int i = 0; i < deviceCount; i++) {
		if (isDeviceSuitable(devices[i])) {
			physicalDevice = devices[i];
			break;
		}
	}
	if (physicalDevice == VK_NULL_HANDLE) {
		PRINT("Failed to find a suitable GPU!\n");
		exit(1);
	}
	PRINT("Physical Device selected.\n");
	delete[] devices;
}

/* Helper Functions */

int App::findQueueFamilies(VkPhysicalDevice physDevice)
{
	int index = -1;
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(physDevice, &queueFamilyCount, nullptr);
	VkQueueFamilyProperties *queueFamilies = new VkQueueFamilyProperties[queueFamilyCount];
	vkGetPhysicalDeviceQueueFamilyProperties(physDevice, &queueFamilyCount, queueFamilies);

	for (unsigned int i = 0; i < queueFamilyCount; i++) {
		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(physDevice, i, surface, &presentSupport);
		if (queueFamilies[i].queueCount > 0 && queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT && presentSupport) {
			index = i;
		}
		if (index >= 0) break;
	}
	delete[] queueFamilies;
	return index;
}

bool App::checkDeviceExtensionSupport(VkPhysicalDevice physDevice)
{
	bool extensionsSupported = false;

	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties(physDevice, nullptr, &extensionCount, nullptr);
	VkExtensionProperties *availableExtensions = new VkExtensionProperties[extensionCount];
	vkEnumerateDeviceExtensionProperties(physDevice, nullptr, &extensionCount, availableExtensions);

	for (uint32_t i = 0; i < extensionCount; i++) {
		if (strcmp(availableExtensions[i].extensionName, VK_KHR_SWAPCHAIN_EXTENSION_NAME) == 0) {
			extensionsSupported = true;
			break;
		}
	}
	delete[] availableExtensions;
	return extensionsSupported;
}

App::SwapChainSupportDetails App::querySwapChainSupport(VkPhysicalDevice physDevice)
{
	SwapChainSupportDetails details;

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physDevice, surface, &details.capabilities);

	vkGetPhysicalDeviceSurfaceFormatsKHR(physDevice, surface, &details.formatCount, nullptr);
	details.formats = new VkSurfaceFormatKHR[details.formatCount];
	vkGetPhysicalDeviceSurfaceFormatsKHR(physDevice, surface, &details.formatCount, details.formats);

	vkGetPhysicalDeviceSurfacePresentModesKHR(physDevice, surface, &details.presentModeCount, nullptr);
	details.presentModes = new VkPresentModeKHR[details.presentModeCount];
	vkGetPhysicalDeviceSurfacePresentModesKHR(physDevice, surface, &details.presentModeCount, details.presentModes);

	return details;
}

bool App::isDeviceSuitable(VkPhysicalDevice physDevice)
{
	int index = findQueueFamilies(physDevice);
	bool extensionsSupported = checkDeviceExtensionSupport(physDevice);
	bool swapChainAdequate = false;

	if (extensionsSupported) {
		SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physDevice);
		swapChainAdequate = swapChainSupport.formatCount && swapChainSupport.presentModeCount;
	}

	VkSurfaceCapabilitiesKHR capabilities;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physDevice, surface, &capabilities);

	return index >= 0 && extensionsSupported && swapChainAdequate && (capabilities.maxImageCount>2);
}

void App::run()
{
    printSupportedApiVersion();
    printLayers();
    printExtensions();
    createInstance();
    createSurface();
    printPhysicalDevices();
	choosePhysicalDevice();
}

void startApp(struct android_app* state, FILE* file)
{
    App app(state, file);
    app.run();
}