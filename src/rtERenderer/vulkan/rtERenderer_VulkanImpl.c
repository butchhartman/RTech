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
        rtELog_logError(message, __VA_ARGS__); \
        return (vkfunc); \
} \
} while(0) \

#define VK_NO_FLAGS 0

struct rtER_VulkanImpl {
        uint32_t apiVersion;
        VkInstance instance;
};


static enum VkResult createVKInstance(VkInstance* dest, uint32_t* apiVersionDest) {
        assert(dest != nullptr);
        assert(apiVersionDest != nullptr);

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

        struct VkInstanceCreateInfo createInfo = {
                .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
                .pNext = nullptr,
                .flags = VK_NO_FLAGS,
                .pApplicationInfo = &applicationInfo,
                .enabledLayerCount = 0, // temp
                .ppEnabledLayerNames = nullptr,
                .enabledExtensionCount = 0,
                .ppEnabledExtensionNames = nullptr
        };

        VK_ERROR_LOG_AND_RETURN(vkCreateInstance(&createInfo, nullptr, dest), "Failed to create Vulkan instance");
        
        rtELog_debug_logInfo("Created Vulkan Instance");
        return VK_SUCCESS;
}
 
enum rtEErrorCode rtER_VK_initializeRenderer(struct rtER_VulkanImpl** dest) {
        assert(dest != nullptr);
        assert(*dest != nullptr);

        *dest = malloc(sizeof(struct rtER_VulkanImpl));

        rtELog_debug_logInfo("Allocated memory for rtER_VulkanImpl in the impl pointer of rtERenderer");

        createVKInstance(&(*dest)->instance, &(*dest)->apiVersion);

        return rtEErrorCode_SUCCESS;
}
