#include "rtERenderer/vulkan/creation/rtER_VK_infoCreation.h"

// If the compiler declines to inline this function, it will look here for the regular definition
extern inline VkDebugUtilsMessengerCreateInfoEXT rtER_VK_getDebugMessengerCreateInfo(
        VkDebugUtilsMessageSeverityFlagsEXT severity,
        VkDebugUtilsMessageTypeFlagsEXT type,
        PFN_vkDebugUtilsMessengerCallbackEXT callback
        ); 

