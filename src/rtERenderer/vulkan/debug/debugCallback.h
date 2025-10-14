#ifndef DEBUGCALLBACK_H_
#define DEBUGCALLBACK_H_
#include <vulkan/vulkan.h>

VkBool32 VKAPI_PTR rtER_debug_debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageTypes,
        const struct VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData);

#endif // DEBUGCALLBACK_H_
