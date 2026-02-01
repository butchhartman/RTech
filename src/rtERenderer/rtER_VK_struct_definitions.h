#ifndef RTER_VK_STRUCT_DEFINITIONS
#define RTER_VK_STRUCT_DEFINITIONS
#include <stdint.h>
#include <vulkan/vulkan.h>
constexpr size_t MAX_CONCURRENT_FRAMES = 2;

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

struct rtERenderer { 
        struct rtEngineWindow* window;
        uint32_t apiVersion; 
        VkInstance instance;
        VkDebugUtilsMessengerEXT debugMessenger;
        VkSurfaceKHR surface;
        VkPhysicalDevice physDevice;
        VkDevice logicalDevice;
        struct rtER_VK_queueInfo queueInfo;
        VkSwapchainKHR swapchain;
        struct rtER_VK_swapchainInfo swapchainInfo;
        VkImage* swapchainImages;
        uint32_t swapchainImageCount;
        VkImageView* swapchainImageViews;
        VkRenderPass renderPass;
        VkFramebuffer* framebuffers; // same length as image views, which is the same as swapchainimagecount
        VkPipeline graphicsPipeline;
        VkPipelineLayout pipelineLayout;

        VkCommandPool commandPool;
        VkCommandBuffer commandBuffer[MAX_CONCURRENT_FRAMES];
        VkFence queueExecuteFence[MAX_CONCURRENT_FRAMES];
        VkSemaphore imageAvaiableSemaphore[MAX_CONCURRENT_FRAMES];
        VkSemaphore renderingFinishedSemaphores[2]; // needs to be swapchain image count to have a semaphore fore each image. Hardcoded to 2 but could be less.
        size_t currentFrame;
        VkDescriptorSetLayout UBODescriptorSetLayout;
        VkDescriptorPool UBODescriptorPool;
        VkDescriptorSet UBODescriptorSet;

        uint32_t uniformBufferCount;
        uint32_t vertexBufferCount;

        struct rtER_VK_Buffer* uniformBuffers;
        struct rtER_VK_Buffer* vertexBuffers;

        // Size of AND number of bound vBuffers
        uint32_t boundVertexBuffersCount;
        VkBuffer* boundVertexBuffers;
};

#endif // RTER_VK_STRUCT_DEFINITIONS
