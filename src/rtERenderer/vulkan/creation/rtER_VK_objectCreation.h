#ifndef RTER_VK_OBJECTCREATION_H_
#define RTER_VK_OBJECTCREATION_H_
#include <vulkan/vulkan.h>

struct rtER_VK_queueCapabilities {
        enum VkQueueFlagBits queueFlags;
        VkBool32 presentationSupport;
};

struct rtER_VK_queueInfo {
        VkQueue* queues;
        uint32_t* queueFamilyIndices;
        struct rtER_VK_queueCapabilities* queueFlags;
        uint32_t queueCount;
};

VkQueue* rtER_VK_getQueueWithCapabilities(
        struct rtER_VK_queueInfo queueInfo, 
        struct rtER_VK_queueCapabilities requiredCapabilities, 
        uint32_t* queueFamilyIndex
        );

enum VkResult rtER_VK_createVKInstance(
        VkInstance* dest, 
        uint32_t* apiVersionDest, 
        const char** requiredInstanceExtensions, 
        uint32_t numRequiredInstanceExtensions, 
        const char** requiredLayers, 
        uint32_t numRequiredLayers);

enum VkResult rtER_VK_createDebugMessenger(
        VkDebugUtilsMessengerEXT* dest, 
        VkInstance instance, 
        VkDebugUtilsMessengerCreateInfoEXT info);


enum VkResult rtER_VK_getSuitablePhysicalDevice(
        VkPhysicalDevice* dest,
        VkInstance instance,
        VkSurfaceKHR surface,
        VkQueueFlagBits requiredQueueFlags,
        const char** requiredExtensions,
        uint32_t requiredExtensionsCount
        );

enum VkResult rtER_VK_createLogicalDevice(
        VkDevice* dest,
        VkPhysicalDevice physDevice,
        VkSurfaceKHR* surface,
        VkQueueFlagBits requiredQueueTypeFlags,
        const char** requiredExtensions,
        uint32_t requiredExtensionsCount,
        struct rtER_VK_queueInfo* queueInfo
        );

enum VkResult rtER_VK_createSwapchain(
        VkSwapchainKHR* dest,
        VkSurfaceKHR surface,
        VkPhysicalDevice physDevice,
        VkDevice logicalDevice,
        VkImage** swapchainImages,
        uint32_t* swapchainImageCount
        );

#endif // RTER_VK_OBJECTCREATION_H_
