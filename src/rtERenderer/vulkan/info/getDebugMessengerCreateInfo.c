#include "rtERenderer/vulkan/debug/debugCallback.h"
#include "rtERenderer/vulkan/macros/rtERendererVKMacros.h"
#include "rtERenderer/vulkan/info/getDebugMessengerCreateInfo.h"

VkDebugUtilsMessengerCreateInfoEXT rtER_VK_getDebugMessengerCreateInfo() {
        VkDebugUtilsMessengerCreateInfoEXT createInfo = {
                .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
                .pNext = nullptr,
                .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT, 
                .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT, 
                .pfnUserCallback = rtER_debug_debugCallback,
                .flags = NO_VK_FLAGS,
                .pUserData = nullptr 
                };
        return createInfo;
}
