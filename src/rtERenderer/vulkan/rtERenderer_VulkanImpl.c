#include "rtERenderer/vulkan/rtERenderer_VulkanImpl.h"
#include "rtERenderer/vulkan/info/getDebugMessengerCreateInfo.h"
#include "rtERenderer/vulkan/macros/rtERendererVKMacros.h"
#include "rtERenderer/vulkan/debug/checkValidationLayerSupport.h"
#include "rtEErrorCodes/rtEErrorCodes.h"
#include "rtELog/rtELog.h"
#include "rtERenderer/vulkan/rtERenderer_VK_constants.h"
#include "rtERenderer/vulkan/support/checkInstanceExtensionSupport.h"
#include <assert.h>
#include <stdint.h>
#include <vulkan/vulkan.h>
#include <stdlib.h>

struct rtER_VulkanImpl { 
        uint32_t apiVersion; 
        VkInstance instance;
        VkDebugUtilsMessengerEXT debugMessenger;
};



static enum VkResult createVKInstance(VkInstance* dest, uint32_t* apiVersionDest, const char** requiredInstanceExtensions, uint32_t numRequiredInstanceExtensions, const char** requiredLayers, uint32_t numRequiredLayers) {
        assert(dest != nullptr);
        #ifndef NDEBUG
        if (!rtER_debug_checkValidationLayerSupport(requiredLayers, numRequiredLayers)) {
                rtELog_logError("One or more required validation layers not supported");
                return VK_ERROR_LAYER_NOT_PRESENT;
        }
        #endif

        if (!rtER_VK_checkInstanceExtensionSupport(requiredInstanceExtensions, numRequiredInstanceExtensions)) {
                rtELog_logError("One or more required instance extensions not supported");
                return VK_ERROR_EXTENSION_NOT_PRESENT;
        }

        VK_ERROR_LOG_AND_RETURN(vkEnumerateInstanceVersion(apiVersionDest), "Failed to enumerate instance version. This should not happen.");

        if (VK_API_VERSION_MINOR(*apiVersionDest) < 4) {
                rtELog_logError("Required Vulkan version 1.4 not supported");
                return VK_ERROR_INCOMPATIBLE_DRIVER;
        }

        struct VkApplicationInfo applicationInfo = {
                .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
                .pNext = nullptr,
                .pApplicationName = RTECH_APPLICATION_NAME,
                .applicationVersion = VK_MAKE_API_VERSION(0, RTECH_VERSION_MAJOR, RTECH_VERSION_MINOR, RTECH_VERSION_PATCH),
                .pEngineName = nullptr,
                .engineVersion = 0,
                .apiVersion = VK_API_VERSION_1_4
        };

        struct VkDebugUtilsMessengerCreateInfoEXT dbmsgCreateInfo = rtER_VK_getDebugMessengerCreateInfo();

        struct VkInstanceCreateInfo createInfo = {
                .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
                .pNext = &dbmsgCreateInfo,
                .flags = NO_VK_FLAGS,
                .pApplicationInfo = &applicationInfo,
                #ifndef NDEBUG
                        .enabledLayerCount = numRequiredLayers,
                        .ppEnabledLayerNames = requiredLayers,
                #else
                        .enabledLayerCount = 0,
                        .ppEnabledLayerNames = nullptr,
                #endif
                .enabledExtensionCount = numRequiredInstanceExtensions,
                .ppEnabledExtensionNames = requiredInstanceExtensions 
        };

        VK_ERROR_LOG_AND_RETURN(vkCreateInstance(&createInfo, nullptr, dest), "Failed to create Vulkan instance");
        
        rtELog_debug_logInfo("Created Vulkan Instance");
        return VK_SUCCESS;
}


static enum VkResult createDebugMessenger(VkDebugUtilsMessengerEXT* dest, VkInstance instance) {
        PFN_vkCreateDebugUtilsMessengerEXT pfnCreateDebugUtilsMessengerEXT = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");

        struct VkDebugUtilsMessengerCreateInfoEXT createInfo = rtER_VK_getDebugMessengerCreateInfo();

        VK_ERROR_LOG_AND_RETURN(pfnCreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, dest), "Failed to create debug messenger");

        rtELog_debug_logInfo("Created debug messenger");

        return VK_SUCCESS;
}
 
enum rtEErrorCode rtER_VK_initializeRenderer(struct rtER_VulkanImpl** dest) {
        assert(dest != nullptr);
        assert(*dest != nullptr);

        // TODO: Maybe read these from a file or something? here works fine for now
        *dest = malloc(sizeof(struct rtER_VulkanImpl));

        rtELog_debug_logInfo("Allocated memory for rtER_VulkanImpl in the impl pointer of rtERenderer");

        createVKInstance(&(*dest)->instance, &(*dest)->apiVersion, rtER_VK_requiredInstanceExtensions, ARRAY_SIZE(rtER_VK_requiredInstanceExtensions), rtER_VK_requiredValidationLayers, ARRAY_SIZE(rtER_VK_requiredValidationLayers));
        createDebugMessenger(&(*dest)->debugMessenger, (*dest)->instance);

        return rtEErrorCode_SUCCESS;
}

enum rtEErrorCode rtER_VK_cleanupRenderer(void** ptr) {
        struct rtER_VulkanImpl** implPtr = (struct rtER_VulkanImpl**)ptr; 
        vkDestroyInstance((*implPtr)->instance, nullptr);

        free(*implPtr);

        rtELog_debug_logInfo("Cleaned up Vulkan Renderer Resources");
        return rtEErrorCode_SUCCESS;
}
