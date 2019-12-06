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
//#include <algorithm> /* Wird für min/max benötigt */

class Buffer
{
protected:
	size_t size;
	VkBuffer buffer;
	VkDeviceMemory bufferMemory;
	VkCommandBuffer commandBuffer;
	VkPhysicalDevice physicalDevice;
	VkDevice device;
	VkCommandPool commandPool;
	VkQueue queue;
public:
	Buffer(VkPhysicalDevice _physicalDevice, VkDevice _device, VkCommandPool _commandPool, VkQueue _queue);
	~Buffer();
	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
	void copyBuffer(VkBuffer srcBuffer, VkDeviceSize size);
	void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);
	void createDeviceLocalBuffer(const void *srcData, VkDeviceSize _size, VkBufferUsageFlags bufferUsageFlags);
	void beginSingleTimeCommands();
	void endSingleTimeCommands();
	VkBuffer getBuffer();
	VkCommandBuffer getCommandBuffer();
	VkDeviceMemory getBufferMemory();
};

class Image
{
protected:
	VkImage image;
	VkImageView imageView;
	VkDeviceMemory imageMemory;
	VkPhysicalDevice physicalDevice;
	VkDevice device;
	VkCommandPool commandPool;
	VkQueue queue;
	Buffer *commandBuffer;
    uint32_t swapChainImagesCount;
	Image **swapChainImages;
public:
	Image(VkPhysicalDevice _physicalDevice, VkDevice _device, VkCommandPool _commandPool, VkQueue _queue);
	~Image();
	bool hasStencilComponent(VkFormat format);
	void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties);
	void createImageView(VkFormat format, VkImageAspectFlags aspectFlags);
	void transitionImageLayout(VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
	void copyBufferToImage(VkBuffer buffer, uint32_t width, uint32_t height);
	void setImage(VkImage _image);
	VkImageView getImageView();
};

class App {
    struct android_app* state;

    struct SwapChainSupportDetails {
		VkSurfaceCapabilitiesKHR capabilities;
		uint32_t formatCount;
		VkSurfaceFormatKHR *formats;
		uint32_t presentModeCount;
		VkPresentModeKHR *presentModes;
	};

    VkInstance instance;
    VkSurfaceKHR surface;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDevice device;
	VkQueue queue;
    VkSwapchainKHR swapChain;
    VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;
    VkCommandPool commandPool;
    uint32_t swapChainImagesCount;
	Image **swapChainImages;
	VkRenderPass renderPass;
	VkDescriptorSetLayout descriptorSetLayout;
	Image *depthImage;
	VkFramebuffer *swapChainFramebuffers;
	VkSemaphore imageAvailableSemaphore;
	VkSemaphore renderFinishedSemaphore;
    
    void printSupportedApiVersion();
	void printLayers();
	void printExtensions();
	void createInstance();
    void createSurface();
    void printPhysicalDevices();
    void choosePhysicalDevice();
    void createLogicalDevice();
    void createSwapChain();
    void createCommandPool();
    void createSwapChainImages();
	void createRenderPass();
	void createDescriptorSetLayout();
	void createDepthImage();
	void createFramebuffers();
	void createSemaphores();

    /* Helper Functions */

    int findQueueFamilies(VkPhysicalDevice physDevice);
    bool checkDeviceExtensionSupport(VkPhysicalDevice physDevice);
    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice physDevice);
    bool isDeviceSuitable(VkPhysicalDevice physDevice);
    VkSurfaceFormatKHR chooseSwapSurfaceFormat(uint32_t formatCount, VkSurfaceFormatKHR *availableFormats);
    VkPresentModeKHR chooseSwapPresentMode(uint32_t presentModeCount, VkPresentModeKHR *availablePresentModes);
    VkExtent2D chooseSwapExtent(VkSurfaceCapabilitiesKHR *capabilities);
	VkFormat findSupportedFormat(uint32_t candidatesCount, VkFormat *candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
	VkFormat findDepthFormat();
public:
    App(struct android_app* s);
    ~App();
    void run();
};

/*
class RenderScene
{
protected:
	struct Camera {
		float xPos, yPos, zPos;
		float xAngle, yAngle, zAngle;
		mat4 M;
	} cam;
	bool *key;
	mat4 mView, mView2;
	VulkanSetup *vulkanSetup;
	uint32_t objectCount;
	RenderObject **obj;
	RenderObject *txtObj;
	VertexData *vertexData;
	IndexData *indexData;
	Buffer *vertexBuffer;
	Buffer *indexBuffer;
	//TextOverlay *textOverlay;
	VkDescriptorPool descriptorPool;
	VkCommandBuffer *commandBuffers;
	void printMatrix(mat4 M, float x, float y);
	void createVertexBuffer();
	void createIndexBuffer();
	void createDescriptorPool();
	void createCommandBuffers();
public:
	RenderScene(VulkanSetup *_vulkanSetup, bool *_key);
	~RenderScene();
	void updateUniformBuffers();
	void camMotion();
	void updateTextOverlay(uint32_t fps);
	void drawFrame();
};
*/
#endif

#ifdef __cplusplus
extern "C" {
#endif

void startApp(struct android_app* state, FILE* f);

#ifdef __cplusplus
}
#endif

#endif  // APP_H