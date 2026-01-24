#ifndef RTER_VK_OBJECTCREATION_H_
#define RTER_VK_OBJECTCREATION_H_
#include <vulkan/vulkan.h>
struct rtERenderer;
struct rtER_VK_queueInfo;
struct rtER_VK_queueCapabilities;
struct rtER_VK_swapchainInfo;
struct rtER_VK_Buffer;

VkQueue* rtER_VK_getQueueWithCapabilities(
        struct rtER_VK_queueInfo queueInfo, 
        struct rtER_VK_queueCapabilities requiredCapabilities, 
        uint32_t* queueFamilyIndex
        );

enum VkResult rtER_VK_createVKInstance(
        struct rtERenderer* renderer, 
        const char** requiredInstanceExtensions, 
        uint32_t numRequiredInstanceExtensions, 
        const char** requiredLayers, 
        uint32_t numRequiredLayers);

enum VkResult rtER_VK_createDebugMessenger(
        struct rtERenderer* renderer,
        VkDebugUtilsMessengerCreateInfoEXT info);


enum VkResult rtER_VK_getSuitablePhysicalDevice(
        struct rtERenderer* renderer,
        VkQueueFlagBits requiredQueueFlags,
        const char** requiredExtensions,
        uint32_t requiredExtensionsCount
        );

enum VkResult rtER_VK_createLogicalDevice(
        struct rtERenderer* renderer,
        VkQueueFlagBits requiredQueueTypeFlags,
        const char** requiredExtensions,
        uint32_t requiredExtensionsCount,
        struct rtER_VK_queueInfo* queueInfo
        );

enum VkResult rtER_VK_createSwapchain(
        struct rtERenderer* renderer
        );

enum VkResult rtER_VK_createImageViews(
        struct rtERenderer* renderer
        );

enum VkResult rtER_VK_createRenderpass(
        struct rtERenderer* renderer
        );

enum VkResult rtER_VK_createFramebuffers(
        struct rtERenderer* renderer
        );

enum VkResult rtER_VK_createGraphicsPipeline(
        struct rtERenderer* renderer
        );

enum VkResult rtER_VK_createCommandPool(
        struct rtERenderer* renderer
        );

// The following creation functions will need more thought as the renderer needs to create multiple. Perhaps convert to create *s functions
enum VkResult rtER_VK_createCommandBuffer(
        VkCommandBuffer* dest,
        VkDevice logicalDevice,
        VkCommandPool commandPool
);

enum VkResult rtER_VK_createFence(
                VkFence* dest,
                VkDevice logicalDevice
        );

enum VkResult rtER_VK_createSemaphore(
                VkSemaphore *dest,
                VkDevice logicalDevice
        );

enum VkResult rtER_VK_createBuffer(
        struct rtER_VK_Buffer* dest,
        size_t size,
        VkDevice logicalDevice,
        VkPhysicalDevice physDevice,
        VkBufferCreateFlags flags,
        VkBufferUsageFlags usage,
        VkSharingMode sharingMode,
        size_t queueCount,
        uint32_t* qfi,
        VkMemoryPropertyFlags memoryProperties
        );

enum VkResult rtER_VK_bufferData(
        void* data,
        VkDevice logicalDevice,
        VkDeviceMemory deviceMemory,
        VkDeviceSize offset,
        VkDeviceSize sizeToMap,
        VkMemoryMapFlags flags
        );

enum VkResult rtER_VK_createDescriptorSetLayout(
                struct rtERenderer* renderer,
                uint32_t binding,
                uint32_t descriptorCount,
                enum VkDescriptorType descriptorType,
                enum VkShaderStageFlagBits stageFlags,
                const VkSampler* pImmutableSamplers);

enum VkResult rtER_VK_createDescriptorPool(
                VkDescriptorPool* dest, 
                VkDevice logicalDevice
                );

enum VkResult rtER_VK_allocateDescriptorSets(
                VkDescriptorSet* dest,
                VkDescriptorSetLayout layout,
                VkDescriptorPool descriptorPool,
                VkDevice logicalDevice
              );
#endif // RTER_VK_OBJECTCREATION_H_
