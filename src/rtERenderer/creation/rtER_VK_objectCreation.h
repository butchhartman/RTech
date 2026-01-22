#ifndef RTER_VK_OBJECTCREATION_H_
#define RTER_VK_OBJECTCREATION_H_
#include <vulkan/vulkan.h>

struct vertex {
        float x, y, z;
        float u, v;
        float r, g, b;
};

struct rtER_VK_Buffer {
        VkBuffer buffer;
        VkDeviceMemory bufferDeviceMemory;
        size_t bufferSize;
};

struct rtER_VK_swapchainInfo {
        VkFormat imageFormat;
        VkExtent2D swapchianExtent;
};

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
        struct rtER_VK_swapchainInfo* infoDest,
        VkSurfaceKHR surface,
        VkPhysicalDevice physDevice,
        VkDevice logicalDevice,
        VkImage** swapchainImages,
        uint32_t* swapchainImageCount
        );

enum VkResult rtER_VK_createImageViews(
        VkImageView** dest,
        struct rtER_VK_swapchainInfo swapchainInfo,
        VkImage* images,
        uint32_t imageCount,
        VkDevice logicalDevice
        );

enum VkResult rtER_VK_createRenderpass(
        VkRenderPass* dest,
        VkDevice logicalDevice,
        struct rtER_VK_swapchainInfo swapchainInfo
        );

enum VkResult rtER_VK_createFramebuffers(
        VkFramebuffer** dest,
        VkDevice logicalDevice,
        VkRenderPass renderPass,
        VkImageView* imageViews,
        uint32_t numImageViews,
        struct rtER_VK_swapchainInfo swapchainInfo
        );

enum VkResult rtER_VK_createGraphicsPipeline(
        VkPipeline* dest,
        VkPipelineLayout* layoutDest,
        VkDevice logicalDevice,
        VkRenderPass renderpass,
        struct rtER_VK_swapchainInfo swapchainInfo,
        VkDescriptorSetLayout UBODescriptorSetLayout
        );

enum VkResult rtER_VK_createCommandPool(
        VkCommandPool* dest,
        VkDevice logicalDevice,
        struct rtER_VK_queueInfo queueInfo 
        );

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
                VkDescriptorSetLayout* dest, 
                VkDevice logicalDevice,
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
