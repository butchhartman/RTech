#ifndef RTER_VK_SUPPORT_H_
#define RTER_VK_SUPPORT_H_
#include <vulkan/vulkan.h>

VkBool32 rtER_VK_checkInstanceExtensionSupport(const char** requiredInstanceExtensions, uint32_t numRequiredInstanceExtensions);

unsigned char* rtER_VK_readShaderSource(const char* shaderPath, uint32_t* codeSize);

VkBool32 rtER_debug_checkValidationLayerSupport(const char** requiredValidationLayers, uint32_t numRequiredLayers);

VkBool32 VKAPI_PTR rtER_debug_debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageTypes,
        const struct VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData);

#endif // RTER_VK_SUPPORT_H_
