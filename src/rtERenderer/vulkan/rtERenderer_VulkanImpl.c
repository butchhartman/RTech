#include "rtERenderer/vulkan/rtERenderer_VulkanImpl.h"
#include "rtERenderer/vulkan/creation/rtER_VK_infoCreation.h"
#include "rtERenderer/vulkan/creation/rtER_VK_objectCreation.h"
#include "rtERenderer/vulkan/debug/debugCallback.h"
#include "rtERenderer/vulkan/macros/rtERendererVKMacros.h"
#include "rtEErrorCodes/rtEErrorCodes.h"
#include "rtELog/rtELog.h"
#include "rtERenderer/vulkan/rtERenderer_VK_constants.h"
#include "rtEW/rtenginewindow.h"
#include "rtEW/vulkan/rtEW_VK_createSurface.h"
#include <assert.h>
#include <stdint.h>
#include <vulkan/vulkan.h>
#include <stdlib.h>

struct rtER_VulkanImpl { 
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
};

 
enum rtEErrorCode rtER_VK_initializeRenderer(struct rtER_VulkanImpl** dest, struct rtEngineWindow* window) {
        assert(dest != nullptr);

        *dest = malloc(sizeof(struct rtER_VulkanImpl));

        rtELog_debug_logInfo("Allocated memory for rtER_VulkanImpl in the impl pointer of rtERenderer");

        (*dest)->window = window;

        rtER_VK_createVKInstance(
                &(*dest)->instance, 
                &(*dest)->apiVersion, 
                rtER_VK_requiredInstanceExtensions, 
                ARRAY_SIZE(rtER_VK_requiredInstanceExtensions), 
                rtER_VK_requiredValidationLayers, 
                ARRAY_SIZE(rtER_VK_requiredValidationLayers)
                );

        rtER_VK_createDebugMessenger(
                &(*dest)->debugMessenger, 
                (*dest)->instance,
                rtER_VK_getDebugMessengerCreateInfo(
                        VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
                        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                        VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,

                        VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                        VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT |
                        VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT,

                        rtER_debug_debugCallback
                        )
                );

        rtEW_VK_createSurface(
                &(*dest)->surface, 
                (*dest)->instance, 
                (*dest)->window
                );

        rtER_VK_getSuitablePhysicalDevice(
                &(*dest)->physDevice,
                (*dest)->instance,
                (*dest)->surface,
                VK_QUEUE_GRAPHICS_BIT,
                rtER_VK_requiredDeviceExtensions,
                ARRAY_SIZE(rtER_VK_requiredDeviceExtensions)
                );

        rtER_VK_createLogicalDevice(
                &(*dest)->logicalDevice,
                (*dest)->physDevice,
                &(*dest)->surface,
                VK_QUEUE_GRAPHICS_BIT,
                rtER_VK_requiredDeviceExtensions,
                ARRAY_SIZE(rtER_VK_requiredDeviceExtensions),
                &(*dest)->queueInfo
        );

        rtER_VK_createSwapchain(
        &(*dest)->swapchain,
        &(*dest)->swapchainInfo,
        (*dest)->surface,
        (*dest)->physDevice,
        (*dest)->logicalDevice,
        &(*dest)->swapchainImages,
        &(*dest)->swapchainImageCount
        );

        rtER_VK_createImageViews(
        &(*dest)->swapchainImageViews,
        (*dest)->swapchainInfo,
        (*dest)->swapchainImages,
        (*dest)->swapchainImageCount,
        (*dest)->logicalDevice
        );

        rtER_VK_createRenderpass(
        &(*dest)->renderPass,
        (*dest)->logicalDevice,
        (*dest)->swapchainInfo
        );

        rtER_VK_createFramebuffers(
                &(*dest)->framebuffers,
                (*dest)->logicalDevice,
                (*dest)->renderPass,
                (*dest)->swapchainImageViews,
                (*dest)->swapchainImageCount,
                (*dest)->swapchainInfo
        );

        rtER_VK_createGraphicsPipeline(
                &(*dest)->graphicsPipeline,
                (*dest)->logicalDevice,
                (*dest)->renderPass,
                (*dest)->swapchainInfo
        );


        return rtEErrorCode_SUCCESS;
}

enum rtEErrorCode rtER_VK_cleanupRenderer(void** ptr) {
        struct rtER_VulkanImpl** implPtr = (struct rtER_VulkanImpl**)ptr; 
        vkDestroyInstance((*implPtr)->instance, nullptr);

        free(*implPtr);

        rtELog_debug_logInfo("Cleaned up Vulkan Renderer Resources");
        return rtEErrorCode_SUCCESS;
}
