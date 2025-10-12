#include "rtERenderer/vulkan/rtERenderer_VulkanImpl.h"
#include "rtEErrorCodes/rtEErrorCodes.h"
#include "rtELog/rtELog.h"
#include <assert.h>
#include <stdint.h>
#include <vulkan/vulkan.h>
#include <stdlib.h>

#define VK_ERROR_LOG_AND_RETURN(vkfunc, message, ...) \
do { \
        if ((vkfunc) < 0) {\
                rtELog_logError(message" VK_ERROR CODE: %d", vkfunc, __VA_ARGS__); \
                return (vkfunc); \
        } \
} while(0) \

#define NO_VK_FLAGS 0

static const char* instanceExtensions[] = { 
        VK_EXT_DEBUG_UTILS_EXTENSION_NAME 
}; 
static_assert(sizeof(instanceExtensions) / sizeof(const char*) == 1);
#define NUM_INSTANCE_EXTENSIONS (sizeof(instanceExtensions) / sizeof(const char*))
struct rtER_VulkanImpl { 
        uint32_t apiVersion; 
        VkInstance instance;
        VkDebugUtilsMessengerEXT debugMessenger;
};

VkBool32 VKAPI_PTR debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageTypes,
        const struct VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData) {

        // The error is in here. Whenever it tries to report an error, this breaks.
        (void)messageSeverity;
        (void)messageTypes;
        (void)pUserData;
        rtELog_log(pCallbackData->pMessage);
/*
This function should always return VK_FALSE as per the spec:
https://docs.vulkan.org/spec/latest/chapters/debugging.html#PFN_vkDebugUtilsMessengerCallbackEXT
*/
        return VK_FALSE;
}

static VkDebugUtilsMessengerCreateInfoEXT getDebugMessengerCreateInfo() {
        VkDebugUtilsMessengerCreateInfoEXT createInfo = {
                .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
                .pNext = nullptr,
                .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT, 
                .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT, 
                .pfnUserCallback = debugCallback,
                .flags = NO_VK_FLAGS,
                .pUserData = nullptr 
                };
        return createInfo;
}


static enum VkResult createVKInstance(VkInstance* dest, uint32_t* apiVersionDest) {
        assert(dest != nullptr);

        VK_ERROR_LOG_AND_RETURN(vkEnumerateInstanceVersion(apiVersionDest), "Failed to enumerate instance version. This should not happen.");

        if (VK_API_VERSION_MINOR(apiVersionDest) < 4) {
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

        struct VkDebugUtilsMessengerCreateInfoEXT dbmsgCreateInfo = getDebugMessengerCreateInfo();

        struct VkInstanceCreateInfo createInfo = {
                .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
                .pNext = &dbmsgCreateInfo,
                .flags = NO_VK_FLAGS,
                .pApplicationInfo = &applicationInfo,
                .enabledLayerCount = 0, // temp
                .ppEnabledLayerNames = nullptr,
                .enabledExtensionCount = NUM_INSTANCE_EXTENSIONS,
                .ppEnabledExtensionNames = instanceExtensions 
        };

        VK_ERROR_LOG_AND_RETURN(vkCreateInstance(&createInfo, nullptr, dest), "Failed to create Vulkan instance");
        
        rtELog_debug_logInfo("Created Vulkan Instance");
        return VK_SUCCESS;
}


static enum VkResult createDebugMessenger(VkDebugUtilsMessengerEXT* dest, VkInstance instance) {
        PFN_vkCreateDebugUtilsMessengerEXT pfnCreateDebugUtilsMessengerEXT = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");

        PFN_vkSubmitDebugUtilsMessageEXT pfnSubmitDebugUtilsMessageEXT = (PFN_vkSubmitDebugUtilsMessageEXT)vkGetInstanceProcAddr(instance, "vkSubmitDebugUtilsMessageEXT");
        
        struct VkDebugUtilsMessengerCreateInfoEXT createInfo = getDebugMessengerCreateInfo();

        VK_ERROR_LOG_AND_RETURN(pfnCreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, dest), "Failed to create debug messenger");

        rtELog_debug_logInfo("Created debug messenger");

        struct VkDebugUtilsMessengerCallbackDataEXT poop = {
                .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CALLBACK_DATA_EXT,
                .pNext = nullptr,
                .flags = 0,        
                .pMessageIdName = nullptr,
                .messageIdNumber = 0,
                .pMessage = "Debug Callback Test",
                .queueLabelCount = 0,
                .pQueueLabels = nullptr,
                .cmdBufLabelCount = 0,
                .pCmdBufLabels = nullptr,
                .objectCount = 0,
                .pObjects = nullptr
         };

        pfnSubmitDebugUtilsMessageEXT(instance,VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT, VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT, &poop);

        return VK_SUCCESS;
}
 
enum rtEErrorCode rtER_VK_initializeRenderer(struct rtER_VulkanImpl** dest) {
        assert(dest != nullptr);
        assert(*dest != nullptr);

        *dest = malloc(sizeof(struct rtER_VulkanImpl));

        rtELog_debug_logInfo("Allocated memory for rtER_VulkanImpl in the impl pointer of rtERenderer");

        createVKInstance(&(*dest)->instance, &(*dest)->apiVersion);
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
