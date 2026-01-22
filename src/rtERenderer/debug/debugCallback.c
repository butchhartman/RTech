#include "rtELog/rtELog.h"
#include "rtERenderer/debug/debugCallback.h"

VkBool32 VKAPI_PTR rtER_debug_debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageTypes,
        const struct VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData) {

        (void)messageSeverity;
        (void)messageTypes;
        (void)pUserData;

        rtELog_logWarning("%s", pCallbackData->pMessage);
/*
This function should always return VK_FALSE as per the spec:
https://docs.vulkan.org/spec/latest/chapters/debugging.html#PFN_vkDebugUtilsMessengerCallbackEXT
*/
        return VK_FALSE;
}
