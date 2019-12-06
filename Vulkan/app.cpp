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
extern PFN_vkCreateDevice vkCreateDevice;
extern PFN_vkGetDeviceQueue vkGetDeviceQueue;
extern PFN_vkCreateSwapchainKHR vkCreateSwapchainKHR;
extern PFN_vkCreateCommandPool vkCreateCommandPool;
extern PFN_vkCreateImage vkCreateImage;
extern PFN_vkGetImageMemoryRequirements vkGetImageMemoryRequirements;
extern PFN_vkAllocateMemory vkAllocateMemory;
extern PFN_vkBindImageMemory vkBindImageMemory;
extern PFN_vkCreateImageView vkCreateImageView;
extern PFN_vkCmdPipelineBarrier vkCmdPipelineBarrier;
extern PFN_vkCmdCopyBufferToImage vkCmdCopyBufferToImage;
extern PFN_vkGetSwapchainImagesKHR vkGetSwapchainImagesKHR;
extern PFN_vkGetPhysicalDeviceMemoryProperties vkGetPhysicalDeviceMemoryProperties;
extern PFN_vkCreateBuffer vkCreateBuffer;
extern PFN_vkGetBufferMemoryRequirements vkGetBufferMemoryRequirements;
extern PFN_vkBindBufferMemory vkBindBufferMemory;
extern PFN_vkCmdCopyBuffer vkCmdCopyBuffer;
extern PFN_vkMapMemory vkMapMemory;
extern PFN_vkUnmapMemory vkUnmapMemory;
extern PFN_vkDestroyBuffer vkDestroyBuffer;
extern PFN_vkFreeMemory vkFreeMemory;
extern PFN_vkAllocateCommandBuffers vkAllocateCommandBuffers;
extern PFN_vkBeginCommandBuffer vkBeginCommandBuffer;
extern PFN_vkEndCommandBuffer vkEndCommandBuffer;
extern PFN_vkQueueSubmit vkQueueSubmit;
extern PFN_vkQueueWaitIdle vkQueueWaitIdle;
extern PFN_vkFreeCommandBuffers vkFreeCommandBuffers;
extern PFN_vkCreateRenderPass vkCreateRenderPass;
extern PFN_vkCreateDescriptorSetLayout vkCreateDescriptorSetLayout;
extern PFN_vkGetPhysicalDeviceFormatProperties vkGetPhysicalDeviceFormatProperties;
extern PFN_vkCreateFramebuffer vkCreateFramebuffer;
extern PFN_vkCreateSemaphore vkCreateSemaphore;

#define PRINT(...) \
sprintf(buf, ##__VA_ARGS__); \
fwrite(buf, strlen(buf), 1, file);

static char buf[128];
static FILE* file;

Buffer::Buffer(VkPhysicalDevice _physicalDevice, VkDevice _device, VkCommandPool _commandPool, VkQueue _queue)
{
	physicalDevice = _physicalDevice;
	device = _device;
	commandPool = _commandPool;
	queue = _queue;
}

Buffer::~Buffer() {}

uint32_t Buffer::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
		if (typeFilter & (1 << i) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
			return i;
	}

	PRINT("Failed to find suitable memory type!\n")
	exit(1);
}

void Buffer::createBuffer(VkDeviceSize _size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties)
{
	size = _size;
	VkBufferCreateInfo bufferInfo = {};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateBuffer(device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
		PRINT("Failed to create vertex buffer!\n")
		exit(1);
	}

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

	if (vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
		PRINT("Failed to allocate vertex buffer memory!\n")
		exit(1);
	}

	vkBindBufferMemory(device, buffer, bufferMemory, 0);
}

void Buffer::copyBuffer(VkBuffer srcBuffer, VkDeviceSize _size)
{

	beginSingleTimeCommands();

	VkBufferCopy copyRegion = {};
	copyRegion.srcOffset = 0;
	copyRegion.dstOffset = 0;
	copyRegion.size = _size;
	vkCmdCopyBuffer(commandBuffer, srcBuffer, buffer, 1, &copyRegion);

	endSingleTimeCommands();
}

void Buffer::createDeviceLocalBuffer(const void *srcData, VkDeviceSize _size, VkBufferUsageFlags bufferUsageFlags)
{
	void *data; // --> pr�fen ob zu deleten
	Buffer stagingBuffer = Buffer(physicalDevice, device, commandPool, queue);
	stagingBuffer.createBuffer(_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	vkMapMemory(device, stagingBuffer.getBufferMemory(), 0, _size, 0, &data);
		memcpy(data, srcData, _size);
	vkUnmapMemory(device, stagingBuffer.getBufferMemory());
	createBuffer(_size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | bufferUsageFlags, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	copyBuffer(stagingBuffer.getBuffer(), _size);
	vkDestroyBuffer(device, stagingBuffer.getBuffer(), nullptr);
	vkFreeMemory(device, stagingBuffer.getBufferMemory(), nullptr);
}

void Buffer::beginSingleTimeCommands()
{
	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = 1;

	vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(commandBuffer, &beginInfo);
}

void Buffer::endSingleTimeCommands()
{
	vkEndCommandBuffer(commandBuffer);

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(queue);

	vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
}

VkBuffer Buffer::getBuffer() { return buffer; }

VkCommandBuffer Buffer::getCommandBuffer() { return commandBuffer; }

VkDeviceMemory Buffer::getBufferMemory() { return bufferMemory; }

Image::Image(VkPhysicalDevice _physicalDevice, VkDevice _device, VkCommandPool _commandPool, VkQueue _queue)
{
	physicalDevice = _physicalDevice;
	device = _device;
	commandPool = _commandPool;
	queue = _queue;
	commandBuffer = new Buffer(physicalDevice, device, commandPool, queue);
}

Image::~Image() {}

bool Image::hasStencilComponent(VkFormat format)
{
	return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
}

void Image::createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties)
{
	VkImageCreateInfo imageInfo = {};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.extent.width = width;
	imageInfo.extent.height = height;
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = 1;
	imageInfo.arrayLayers = 1;
	imageInfo.format = format;
	imageInfo.tiling = tiling;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageInfo.usage = usage;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageInfo.flags = 0; // Optional

	if (vkCreateImage(device, &imageInfo, nullptr, &image) != VK_SUCCESS) {
		PRINT("Failed to create image!\n")
		exit(1);
	}

	VkMemoryRequirements memRequirements;
	vkGetImageMemoryRequirements(device, image, &memRequirements);

	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = commandBuffer->findMemoryType(memRequirements.memoryTypeBits, properties);

	if (vkAllocateMemory(device, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS) {
		PRINT("Failed to allocate image memory!\n")
		exit(1);
	}

	vkBindImageMemory(device, image, imageMemory, 0);
}

void Image::createImageView(VkFormat format, VkImageAspectFlags aspectFlags)
{
	VkImageViewCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	createInfo.pNext = nullptr;
	createInfo.flags = 0;
	createInfo.image = image;
	createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	createInfo.format = format;
	createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
	createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
	createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
	createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
	createInfo.subresourceRange.aspectMask = aspectFlags;
	createInfo.subresourceRange.baseMipLevel = 0;
	createInfo.subresourceRange.levelCount = 1;
	createInfo.subresourceRange.baseArrayLayer = 0;
	createInfo.subresourceRange.layerCount = 1;

	if (vkCreateImageView(device, &createInfo, nullptr, &imageView) != VK_SUCCESS) {
		PRINT("Failed to create image views!\n")
		exit(1);
	}
}

void Image::transitionImageLayout(VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout)
{
	commandBuffer->beginSingleTimeCommands();

	VkImageMemoryBarrier barrier = {};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout = oldLayout;
	barrier.newLayout = newLayout;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = image;
	if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
		if (hasStencilComponent(format)) {
			barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
		}
	}
	else {
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	}
	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = 1;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;

	VkPipelineStageFlags sourceStage = {};
	VkPipelineStageFlags destinationStage = {};

	if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	}
	else {
		PRINT("Unsupported layout transition!\n")
	}

	vkCmdPipelineBarrier(commandBuffer->getCommandBuffer(), sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);

	commandBuffer->endSingleTimeCommands();
}

void Image::copyBufferToImage(VkBuffer buffer, uint32_t width, uint32_t height)
{
	commandBuffer->beginSingleTimeCommands();

	VkBufferImageCopy region = {};
	region.bufferOffset = 0;
	region.bufferRowLength = 0;
	region.bufferImageHeight = 0;

	region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.imageSubresource.mipLevel = 0;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.layerCount = 1;

	region.imageOffset = { 0, 0, 0 };
	region.imageExtent = { width,height,1 };

	vkCmdCopyBufferToImage(commandBuffer->getCommandBuffer(), buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

	commandBuffer->endSingleTimeCommands();
}

void Image::setImage(VkImage _image) { image = _image; }

VkImageView Image::getImageView() { return imageView; }

App::App(struct android_app* s)
{
    state = s;
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
		PRINT("%s\n","Create Instance first!")
		return;
	}
	uint32_t deviceCount;
	vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
	if (deviceCount == 0) {
		PRINT("%s\n","Failed to find GPUs with Vulkan support!")
		return;
	}
	VkPhysicalDevice *devices = new VkPhysicalDevice[deviceCount];
	vkEnumeratePhysicalDevices(instance, &deviceCount, devices);
    
	for (uint32_t i = 0; i < deviceCount; i++) {
		uint32_t extensionCount;
		vkEnumerateDeviceExtensionProperties(devices[i], nullptr, &extensionCount, nullptr);
		VkExtensionProperties *extensions = new VkExtensionProperties[extensionCount];
		vkEnumerateDeviceExtensionProperties(devices[i], nullptr, &extensionCount, extensions);
		PRINT("Device #%u Extensions:\n", i)
		for (uint32_t j = 0; j < extensionCount; j++) {
			PRINT("\t#%u\t%s\n", j, extensions[j].extensionName)
		}
		delete[] extensions;
        
		VkPhysicalDeviceProperties deviceProperties;
		vkGetPhysicalDeviceProperties(devices[i], &deviceProperties);
		PRINT("Device #%u Properties:\n", i)
		PRINT("\tApi Version:    %u\n", deviceProperties.apiVersion)
		PRINT("\tDevice ID:      %u\n", deviceProperties.deviceID)
		PRINT("\tDevice Name:    %s\n", deviceProperties.deviceName)
		PRINT("\tDevice Type:    %u\n", deviceProperties.deviceType)
		PRINT("\tDriver Version: %u\n", deviceProperties.driverVersion)
		PRINT("\tVendor ID:      %u\n", deviceProperties.vendorID)
		
		VkPhysicalDeviceFeatures deviceFeatures;
		vkGetPhysicalDeviceFeatures(devices[i], &deviceFeatures);
		PRINT("Device #%u Featers:\n", i)
		PRINT("\tTessellation Shader: %u\n", deviceFeatures.tessellationShader)
		PRINT("\tGeometry Shader:     %u\n", deviceFeatures.geometryShader)
		PRINT("\tfullDrawIndexUint32: %u\n", deviceFeatures.fullDrawIndexUint32)
		PRINT("\tsamplerAnisotropy: %u\n", deviceFeatures.samplerAnisotropy)
		PRINT("\twideLines: %u\n", deviceFeatures.wideLines)
		
		uint32_t queueFamilyCount;
		vkGetPhysicalDeviceQueueFamilyProperties(devices[i], &queueFamilyCount, nullptr);
		VkQueueFamilyProperties *queueFamilies = new VkQueueFamilyProperties[queueFamilyCount];
		vkGetPhysicalDeviceQueueFamilyProperties(devices[i], &queueFamilyCount, queueFamilies);
		PRINT("Device #%u Queue Families: (%u)\n", i, queueFamilyCount)
		for (uint32_t j = 0; j < queueFamilyCount; j++)
		{
			PRINT("\tQueue Family #%u:\n", j)
			PRINT("\t\tVK_QUEUE_GRAPHICS_BIT:           %u\n",  ((queueFamilies[j].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0))
			PRINT("\t\tVK_QUEUE_COMPUTE_BIT:            %u\n",  ((queueFamilies[j].queueFlags & VK_QUEUE_COMPUTE_BIT) != 0))
			PRINT("\t\tVK_QUEUE_TRANSFER_BIT:           %u\n",  ((queueFamilies[j].queueFlags & VK_QUEUE_TRANSFER_BIT) != 0))
			PRINT("\t\tVK_QUEUE_SPARSE_BINDING_BIT:     %u\n",  ((queueFamilies[j].queueFlags & VK_QUEUE_SPARSE_BINDING_BIT) != 0))
			PRINT("\t\tQueue Count:                     %u\n",  queueFamilies[j].queueCount)
			PRINT("\t\tTimestamp Valid Bits:            %u\n",  queueFamilies[j].timestampValidBits)
			uint32_t width = queueFamilies[j].minImageTransferGranularity.width;
			uint32_t height = queueFamilies[j].minImageTransferGranularity.height;
			uint32_t depth = queueFamilies[j].minImageTransferGranularity.depth;
			PRINT("\t\tMin Image Timestamp Granularity (width, height, depth): %u, %u, %u\n", width, height, depth)
		}
		delete[] queueFamilies;
		
		VkSurfaceCapabilitiesKHR capabilities;
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(devices[i], surface, &capabilities);
		PRINT("Device #%u Surface capabilities:\n", i)
		PRINT("\tminImageCount:           %u\n", capabilities.minImageCount)
		PRINT("\tmaxImageCount:           %u\n", capabilities.maxImageCount)
		PRINT("\tcurrentExtent:           %u, %u\n", capabilities.currentExtent.width, capabilities.currentExtent.height)
		PRINT("\tminImageExtent:          %u, %u\n", capabilities.minImageExtent.width, capabilities.minImageExtent.height)
		PRINT("\tmaxImageExtent:          %u, %u\n", capabilities.maxImageExtent.width, capabilities.maxImageExtent.height)
		PRINT("\tmaxImageArrayLayers:     %u\n", capabilities.maxImageArrayLayers)
		PRINT("\tsupportedTransforms:     %u\n", capabilities.supportedTransforms)
		PRINT("\tcurrentTransform:        %u\n", capabilities.currentTransform)
		PRINT("\tsupportedCompositeAlpha: %u\n", capabilities.supportedCompositeAlpha)
		PRINT("\tsupportedUsageFlags:     %u\n", capabilities.supportedUsageFlags)
		
		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(devices[i], surface, &formatCount, nullptr);
		VkSurfaceFormatKHR *formats = new VkSurfaceFormatKHR[formatCount];
		vkGetPhysicalDeviceSurfaceFormatsKHR(devices[i], surface, &formatCount, formats);
		PRINT("Device #%u Supported formats:\n", i)
		for (uint32_t j = 0; j < formatCount; j++)
		{
			PRINT("\tFormat #%u:\n", j)
			PRINT("\t\tformat:     %u\n", formats[j].format)
			PRINT("\t\tcolorSpace: %u\n", formats[j].colorSpace)
		}
		delete[] formats;
		
		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(devices[i], surface, &presentModeCount, nullptr);
		VkPresentModeKHR *presentModes = new VkPresentModeKHR[presentModeCount];
		vkGetPhysicalDeviceSurfacePresentModesKHR(devices[i], surface, &presentModeCount, presentModes);
		PRINT("Device #%u supported presentation modes:\n", i)
		for (uint32_t j = 0; j < presentModeCount; j++)
		{
			PRINT("\tPresent mode #%u: %u\n", j, presentModes[j])
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
		PRINT("Failed to find GPUs with Vulkan support!\n")
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
		PRINT("Failed to find a suitable GPU!\n")
		exit(1);
	}
	PRINT("Physical Device selected.\n")
	delete[] devices;
}

void App::createLogicalDevice()
{
	uint32_t queueFamilyIndex = findQueueFamilies(physicalDevice);
	float queuePriority = 1.0f;

	VkDeviceQueueCreateInfo queueCreateInfo = {};
	queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueCreateInfo.pNext = NULL;
	queueCreateInfo.flags = 0;
	queueCreateInfo.queueFamilyIndex = queueFamilyIndex;
	queueCreateInfo.queueCount = 1;
	queueCreateInfo.pQueuePriorities = &queuePriority;

	VkPhysicalDeviceFeatures deviceFeatures = {};
	deviceFeatures.tessellationShader = VK_TRUE;
	deviceFeatures.samplerAnisotropy = VK_TRUE;
	//deviceFeatures.wideLines = VK_TRUE;

	const unsigned int deviceExtensionCount = 1;
	const char *deviceExtensions[] = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
	VkDeviceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.pNext = NULL;
	createInfo.flags = 0;
	createInfo.queueCreateInfoCount = 1;
	createInfo.pQueueCreateInfos = &queueCreateInfo;
	createInfo.enabledLayerCount = 0;
	createInfo.ppEnabledLayerNames = NULL; // evtl. auch hier den validationLayer eintragen
	createInfo.enabledExtensionCount = deviceExtensionCount;
	createInfo.ppEnabledExtensionNames = deviceExtensions;
	createInfo.pEnabledFeatures = &deviceFeatures;

	if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS) {
		PRINT("Failed to create logical device!\n")
		exit(1);
	}
	PRINT("Device created.\n")
	vkGetDeviceQueue(device, queueFamilyIndex, 0, &queue);
}

void App::createSwapChain()
{
	
	SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice); // --> ToDo: delete des Inhalts beim Beenden 
	VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formatCount, swapChainSupport.formats);
	VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModeCount, swapChainSupport.presentModes);
	VkExtent2D extent = swapChainSupport.capabilities.currentExtent;
	uint32_t imageCount = swapChainSupport.capabilities.minImageCount;// + 1;
	if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
		imageCount = swapChainSupport.capabilities.maxImageCount;
	
	VkSwapchainCreateInfoKHR createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.pNext = nullptr;
	createInfo.flags = 0;
	createInfo.surface = surface;
	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	createInfo.queueFamilyIndexCount = 0;
	createInfo.pQueueFamilyIndices = nullptr;
	createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;
	createInfo.oldSwapchain = VK_NULL_HANDLE;
	
	if (vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapChain) != VK_SUCCESS) {
		PRINT("Failed to create swap chain!\n")
		exit(1);
	}
	PRINT("imageCount: %u\n", imageCount)
	PRINT("Swapchain created.\n")
	swapChainImageFormat = surfaceFormat.format;
	swapChainExtent = extent;
}

void App::createCommandPool()
{
	int queueFamilyIndex = findQueueFamilies(physicalDevice);

	VkCommandPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.flags = 0; // Optional
	poolInfo.queueFamilyIndex = queueFamilyIndex;

	if (vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool) != VK_SUCCESS) {
		PRINT("Failed to create command pool!\n")
		exit(1);
	}
	PRINT("Command Pool created.\n")
}

void App::createSwapChainImages()
{
	vkGetSwapchainImagesKHR(device, swapChain, &swapChainImagesCount, nullptr);
	VkImage *tmp = new VkImage[swapChainImagesCount];
	swapChainImages = new Image*[swapChainImagesCount];
	vkGetSwapchainImagesKHR(device, swapChain, &swapChainImagesCount, tmp);

	for (uint32_t i = 0; i < swapChainImagesCount; i++) {
		swapChainImages[i] = new Image(physicalDevice, device, commandPool, queue);
		swapChainImages[i]->setImage(tmp[i]);
		swapChainImages[i]->createImageView(swapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT);
	}
	PRINT("%u image views created.\n", swapChainImagesCount)
	delete[] tmp;
}

void App::createRenderPass()
{
	VkAttachmentDescription colorAttachment = {};
	colorAttachment.flags = 0;
	colorAttachment.format = swapChainImageFormat;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentDescription depthAttachment = {};
	depthAttachment.format = VK_FORMAT_D32_SFLOAT;
	depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentReference colorAttachmentRef = {};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentReference depthAttachmentRef = {};
	depthAttachmentRef.attachment = 1;
	depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass = {};
	subpass.flags = 0;
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;
	subpass.pDepthStencilAttachment = &depthAttachmentRef;

	VkSubpassDependency dependency = {};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	VkAttachmentDescription attachments[] = { colorAttachment, depthAttachment };

	VkRenderPassCreateInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = 2;
	renderPassInfo.pAttachments = attachments;
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;
	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies = &dependency;

	if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS) {
		PRINT("Failed to create render pass!\n")
		exit(1);
	}
	PRINT("Renderpass created.\n")
}

void App::createDescriptorSetLayout()
{
	VkDescriptorSetLayoutBinding uboLayoutBinding = {};
	uboLayoutBinding.binding = 0;
	uboLayoutBinding.descriptorCount = 1;
	uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uboLayoutBinding.stageFlags = VK_SHADER_STAGE_ALL_GRAPHICS;
	uboLayoutBinding.pImmutableSamplers = nullptr;

	VkDescriptorSetLayoutBinding samplerLayoutBinding = {};
	samplerLayoutBinding.binding = 1;
	samplerLayoutBinding.descriptorCount = 1;
	samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	samplerLayoutBinding.pImmutableSamplers = nullptr;

	VkDescriptorSetLayoutBinding uboMaterialBinding = {};
	uboMaterialBinding.binding = 2;
	uboMaterialBinding.descriptorCount = 1;
	uboMaterialBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uboMaterialBinding.stageFlags = VK_SHADER_STAGE_ALL_GRAPHICS;
	uboMaterialBinding.pImmutableSamplers = nullptr;

	VkDescriptorSetLayoutBinding bindings[] = { uboLayoutBinding, samplerLayoutBinding, uboMaterialBinding };

	VkDescriptorSetLayoutCreateInfo layoutInfo = {};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = 3;
	layoutInfo.pBindings = bindings;

	if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) {
		PRINT("Failed to create descriptor set layout!\n")
		exit(1);
	}
	PRINT("DescriptorSetLayout created.\n")
}

void App::createDepthImage()
{
	VkFormat depthFormat = findDepthFormat();
	depthImage = new Image(physicalDevice, device, commandPool, queue);
	depthImage->createImage(swapChainExtent.width, swapChainExtent.height, depthFormat, VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	depthImage->createImageView(depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);
	depthImage->transitionImageLayout(depthFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
	PRINT("Depth Image created.\n")
}

void App::createFramebuffers()
{
	swapChainFramebuffers = new VkFramebuffer[swapChainImagesCount];

	for (uint32_t i = 0; i < swapChainImagesCount; i++) {
		VkImageView attachments[] = { swapChainImages[i]->getImageView(), depthImage->getImageView() };
		VkFramebufferCreateInfo framebufferInfo = {};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = renderPass;
		framebufferInfo.attachmentCount = 2;
		framebufferInfo.pAttachments = attachments;
		framebufferInfo.width = swapChainExtent.width;
		framebufferInfo.height = swapChainExtent.height;
		framebufferInfo.layers = 1;

		if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &swapChainFramebuffers[i]) != VK_SUCCESS) {
			PRINT("failed to create framebuffer!\n")
			exit(1);
		}
	}
	PRINT("Framebuffers created.\n")
}

void App::createSemaphores()
{
	VkSemaphoreCreateInfo semaphoreInfo = {};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &imageAvailableSemaphore) != VK_SUCCESS ||
		vkCreateSemaphore(device, &semaphoreInfo, nullptr, &renderFinishedSemaphore) != VK_SUCCESS)
	{
		PRINT("failed to create semaphores!\n")
		exit(1);
	}
	PRINT("Semaphores created.\n")
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

VkSurfaceFormatKHR App::chooseSwapSurfaceFormat(uint32_t formatCount, VkSurfaceFormatKHR *availableFormats)
{
	if (formatCount == 1 && availableFormats[0].format == VK_FORMAT_UNDEFINED) {
		return { VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
	}
	for (uint32_t i = 0; i < formatCount; i++) {
		if (availableFormats[i].format == VK_FORMAT_B8G8R8A8_UNORM && availableFormats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			return availableFormats[i];
	}
	return availableFormats[0];
}

VkPresentModeKHR App::chooseSwapPresentMode(uint32_t presentModeCount, VkPresentModeKHR *availablePresentModes)
{
	VkPresentModeKHR bestMode = VK_PRESENT_MODE_FIFO_KHR;

	for (uint32_t i = 0; i < presentModeCount; i++) {
		if (availablePresentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR)
			return availablePresentModes[i];
		else if (availablePresentModes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR)
			bestMode = availablePresentModes[i];
	}

	return bestMode;
}

VkFormat App::findSupportedFormat(uint32_t candidatesCount, VkFormat *candidates, VkImageTiling tiling, VkFormatFeatureFlags features)
{
	for (uint32_t i = 0; i < candidatesCount; i++) {
		VkFormatProperties properties;
		vkGetPhysicalDeviceFormatProperties(physicalDevice, candidates[i], &properties);
		if (tiling == VK_IMAGE_TILING_LINEAR && (properties.linearTilingFeatures & features) == features) {
			return candidates[i];
		}
		else if (tiling == VK_IMAGE_TILING_OPTIMAL && (properties.optimalTilingFeatures & features) == features) {
			return candidates[i];
		}
	}
	PRINT("Failed to find supported format!\n")
	exit(1);
}

VkFormat App::findDepthFormat()
{
	VkFormat formats[] = { VK_FORMAT_D32_SFLOAT };
	return findSupportedFormat(1, formats, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}

/*
VkExtent2D App::chooseSwapExtent(VkSurfaceCapabilitiesKHR *capabilities)
{
	if (capabilities->currentExtent.width != std::numeric_limits<uint32_t>::max())
		return capabilities->currentExtent;
	else {
		VkExtent2D actualExtent = { (uint32_t)window->getWidth(), (uint32_t)window->getHeight() };
		actualExtent.width = std::max(capabilities->minImageExtent.width, std::min(capabilities->maxImageExtent.width, actualExtent.width));
		actualExtent.height = std::max(capabilities->minImageExtent.height, std::min(capabilities->maxImageExtent.height, actualExtent.height));

		return actualExtent;
	}
}
*/

void App::run()
{
    printSupportedApiVersion();
    printLayers();
    printExtensions();
    createInstance();
    createSurface();
    printPhysicalDevices();
	choosePhysicalDevice();
	createLogicalDevice();
	createSwapChain();
	createCommandPool();
	createSwapChainImages();
	createRenderPass();
	createDescriptorSetLayout();
	createDepthImage();
	createFramebuffers();
	createSemaphores();
}

void startApp(struct android_app* state, FILE* f)
{
	file = f;
    App app(state);
    app.run();
}