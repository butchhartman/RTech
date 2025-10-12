#include "rtERenderer/vulkan/rtERenderer_VulkanImpl.h"
#include "rtEErrorCodes/rtEErrorCodes.h"
#include "rtELog/rtELog.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
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
#define ARRAY_SIZE(array) \
        (sizeof(array) / sizeof(array[0]))

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



static VkBool32 checkValidationLayerSupport(const char** requiredValidationLayers, uint32_t numRequiredLayers) {
        uint32_t numSupportedLayers;
        VK_ERROR_LOG_AND_RETURN(
                vkEnumerateInstanceLayerProperties(
                        &numSupportedLayers,
                        nullptr
                ),
                "Failed to enumerate instance layer properties"
        );

        VkLayerProperties* layerPropertiesArray = malloc(sizeof(VkLayerProperties) * numSupportedLayers);

        VK_ERROR_LOG_AND_RETURN(
                vkEnumerateInstanceLayerProperties(
                        &numSupportedLayers,
                        layerPropertiesArray 
                ),
                "Failed to enumerate instance layer properties"
        );


        for(size_t i = 0; i < numRequiredLayers; i++) {
                bool requiredLayerSupported = false;
                for(size_t j = 0; j < numSupportedLayers; j++) {
                        if (strcmp(requiredValidationLayers[i], layerPropertiesArray[j].layerName) == 0) {
                                requiredLayerSupported = true;
                        }
                                
                }

                if (!requiredLayerSupported) {
                        free(layerPropertiesArray);
                        return VK_FALSE;
                }

        }
        
        #ifndef NDEBUG
                rtELog_debug_logInfo("Required Validation Layers:");

               for(size_t i = 0; i < numRequiredLayers; i++) {
                       rtELog_debug_logInfo("\t%s", requiredValidationLayers[i]);
               }

               rtELog_debug_logInfo("Supported Validation Layers:");

                for(size_t j = 0; j < numSupportedLayers; j++) {
                       rtELog_debug_logInfo("\t%s", layerPropertiesArray[j].layerName);
                }

        #endif


        free(layerPropertiesArray);
        rtELog_debug_logInfo("All required validation layers supported");
        return VK_TRUE;
}

static VkBool32 checkInstanceExtensionSupport(const char** requiredInstanceExtensions, uint32_t numRequiredInstanceExtensions) {
        uint32_t numSupportedInstanceExtensions;
        if (vkEnumerateInstanceExtensionProperties(nullptr, &numSupportedInstanceExtensions, nullptr) < 0) {
                rtELog_logError("Failed to enumerate instance extension properties");
                return VK_FALSE;
        }

        struct VkExtensionProperties* supportedExtensions = malloc(sizeof(struct VkExtensionProperties) * numSupportedInstanceExtensions);

        if (vkEnumerateInstanceExtensionProperties(nullptr, &numSupportedInstanceExtensions, supportedExtensions) < 0) {
                rtELog_logError("Failed to enumerate instance extension properties");
                free(supportedExtensions);
                return VK_FALSE;
        }

        for(size_t i = 0; i < numRequiredInstanceExtensions; i++) {
                bool extensionSupported = false;
                for(size_t j = 0; j < numSupportedInstanceExtensions; j++) {
                        if (strcmp(requiredInstanceExtensions[i], supportedExtensions[j].extensionName) == 0) {
                                extensionSupported = true;
                        }
                                
                }

                if (!extensionSupported) {
                        free(supportedExtensions);
                        return VK_FALSE;
                }

        }

        #ifndef NDEBUG
                rtELog_debug_logInfo("Required Instance Extensions:");

               for(size_t i = 0; i < numRequiredInstanceExtensions; i++) {
                       rtELog_debug_logInfo("\t%s", requiredInstanceExtensions[i]);
               }

               rtELog_debug_logInfo("Supported Instance Extensions:");

                for(size_t j = 0; j < numSupportedInstanceExtensions; j++) {
                       rtELog_debug_logInfo("\t%s", supportedExtensions[j].extensionName);
                }

        #endif



        free(supportedExtensions);
        rtELog_debug_logInfo("All required instance extensions supported");
        return VK_TRUE;
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


static enum VkResult createVKInstance(VkInstance* dest, uint32_t* apiVersionDest, const char** requiredInstanceExtensions, uint32_t numRequiredInstanceExtensions, const char** requiredLayers, uint32_t numRequiredLayers) {
        assert(dest != nullptr);
        #ifndef NDEBUG
        if (!checkValidationLayerSupport(requiredLayers, numRequiredLayers)) {
                rtELog_logError("One or more required validation layers not supported");
                return VK_ERROR_LAYER_NOT_PRESENT;
        }
        #endif

        if (!checkInstanceExtensionSupport(requiredInstanceExtensions, numRequiredInstanceExtensions)) {
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

        struct VkDebugUtilsMessengerCreateInfoEXT dbmsgCreateInfo = getDebugMessengerCreateInfo();

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

        struct VkDebugUtilsMessengerCreateInfoEXT createInfo = getDebugMessengerCreateInfo();

        VK_ERROR_LOG_AND_RETURN(pfnCreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, dest), "Failed to create debug messenger");

        rtELog_debug_logInfo("Created debug messenger");

        return VK_SUCCESS;
}
 
enum rtEErrorCode rtER_VK_initializeRenderer(struct rtER_VulkanImpl** dest) {
        assert(dest != nullptr);
        assert(*dest != nullptr);

        // TODO: Maybe read these from a file or something? here works fine for now
        const char* requiredInstanceExtensions[] = { 
                VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
        }; 

        const char* requiredValidationLayers[] = {
                "VK_LAYER_KHRONOS_validation"
        };

        *dest = malloc(sizeof(struct rtER_VulkanImpl));

        rtELog_debug_logInfo("Allocated memory for rtER_VulkanImpl in the impl pointer of rtERenderer");

        createVKInstance(&(*dest)->instance, &(*dest)->apiVersion, requiredInstanceExtensions, ARRAY_SIZE(requiredInstanceExtensions), requiredValidationLayers, ARRAY_SIZE(requiredValidationLayers));
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
