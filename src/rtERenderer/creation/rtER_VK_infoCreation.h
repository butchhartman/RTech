#ifndef RTER_VK_INFOCREATION_H_ 
#define RTER_VK_INFOCREATION_H_ 
#include <vulkan/vulkan.h>

inline VkDebugUtilsMessengerCreateInfoEXT rtER_VK_getDebugMessengerCreateInfo(
        VkDebugUtilsMessageSeverityFlagsEXT severity,
        VkDebugUtilsMessageTypeFlagsEXT type,
        PFN_vkDebugUtilsMessengerCallbackEXT callback) {

        VkDebugUtilsMessengerCreateInfoEXT createInfo = {
                .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
                .pNext = nullptr,
                .messageSeverity = severity, 
                .messageType = type, 
                .pfnUserCallback = callback,
                .flags = 0,
                .pUserData = nullptr 
                };
        return createInfo;
}

#endif // RTER_VK_INFOCREATION_H_  
