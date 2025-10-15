#include <stdlib.h>
#include <assert.h>
#include <stdint.h>
#include "rtELog/rtELog.h"
#include "rtERenderer/vulkan/creation/rtER_VK_infoCreation.h"
#include "rtERenderer/vulkan/debug/checkValidationLayerSupport.h"
#include "rtERenderer/vulkan/debug/debugCallback.h"
#include "rtERenderer/vulkan/support/checkInstanceExtensionSupport.h"
#include "rtERenderer/vulkan/creation/rtER_VK_objectCreation.h"
#include "rtERenderer/vulkan/macros/rtERendererVKMacros.h"


enum VkResult rtER_VK_createVKInstance(
        VkInstance* dest, 
        uint32_t* apiVersionDest, 
        const char** requiredInstanceExtensions, 
        uint32_t numRequiredInstanceExtensions, 
        const char** requiredLayers, 
        uint32_t numRequiredLayers) {

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

        VK_ERROR_LOG_AND_RETURN(
                vkEnumerateInstanceVersion(apiVersionDest), 
                "Failed to enumerate instance version. This should not happen."
                );

        if (VK_API_VERSION_MINOR(*apiVersionDest) < 4) {
                rtELog_logError("Required Vulkan version 1.4 not supported");
                return VK_ERROR_INCOMPATIBLE_DRIVER;
        }

        // I dont think this needs a separate function. Maybe to make this code cleaner but this will always be the same value
        struct VkApplicationInfo applicationInfo = {
                .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
                .pNext = nullptr,
                .pApplicationName = RTECH_APPLICATION_NAME,
                .applicationVersion = VK_MAKE_API_VERSION(0, RTECH_VERSION_MAJOR, RTECH_VERSION_MINOR, RTECH_VERSION_PATCH),
                .pEngineName = nullptr,
                .engineVersion = 0,
                .apiVersion = VK_API_VERSION_1_4
        };

        struct VkDebugUtilsMessengerCreateInfoEXT dbmsgCreateInfo = 
                rtER_VK_getDebugMessengerCreateInfo(
                        VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT, 
                        VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT, 
                        rtER_debug_debugCallback
                        );

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

        VK_ERROR_LOG_AND_RETURN(
                vkCreateInstance(&createInfo, nullptr, dest), 
                "Failed to create Vulkan instance"
                );
        
        rtELog_debug_logInfo("Created Vulkan Instance");
        return VK_SUCCESS;
}

enum VkResult rtER_VK_createDebugMessenger(
        VkDebugUtilsMessengerEXT* dest, 
        VkInstance instance, 
        VkDebugUtilsMessengerCreateInfoEXT info) {

        PFN_vkCreateDebugUtilsMessengerEXT pfnCreateDebugUtilsMessengerEXT = 
        (PFN_vkCreateDebugUtilsMessengerEXT)
        vkGetInstanceProcAddr(
                instance, 
                "vkCreateDebugUtilsMessengerEXT");

        VK_ERROR_LOG_AND_RETURN(
                pfnCreateDebugUtilsMessengerEXT(
                        instance, 
                        &info, 
                        nullptr, 
                        dest), 
                "Failed to create debug messenger");

        rtELog_debug_logInfo("Created debug messenger");

        return VK_SUCCESS;
}

// returns allocated memory that the caller is responsible for freeing. sets count to array size
static VkQueueFamilyProperties2* getQueueFamilyProperties(VkPhysicalDevice physDevice, uint32_t* count) {
        vkGetPhysicalDeviceQueueFamilyProperties2(physDevice, count, nullptr);

        VkQueueFamilyProperties2* queueFamilyProperties = malloc(sizeof(VkQueueFamilyProperties2) * *count);
        for (size_t i = 0; i < *count; i++) {
                queueFamilyProperties[i].sType = VK_STRUCTURE_TYPE_QUEUE_FAMILY_PROPERTIES_2;
                queueFamilyProperties[i].pNext = nullptr;
        }

        vkGetPhysicalDeviceQueueFamilyProperties2(physDevice, count, queueFamilyProperties);

        return queueFamilyProperties;
}

// (tail) Recursively checks each individual queue flag for a queue which supports it.
static bool physicalDeviceHasQueueFamilies(
        VkPhysicalDevice physDevice,
        VkQueueFlagBits* neededQueueFlags
        ) {

        if (*neededQueueFlags == 0) {
                return true;
        }
        uint32_t numQueueFamilies;
        VkQueueFamilyProperties2* queueFamilyProperties = getQueueFamilyProperties(physDevice, &numQueueFamilies);
        // TODO: add more selection criteria parameters if I ever care about that sort of thing

        vkGetPhysicalDeviceQueueFamilyProperties2(physDevice, &numQueueFamilies, queueFamilyProperties);
        rtELog_debug_logInfo("Need %d flags", (*neededQueueFlags));

        for (size_t i = 0; i < numQueueFamilies; i++) {
                if (queueFamilyProperties[i].queueFamilyProperties.queueFlags & *neededQueueFlags) {
                        (*neededQueueFlags) = (*neededQueueFlags) & ~(queueFamilyProperties[i].queueFamilyProperties.queueFlags & *neededQueueFlags);
                        free(queueFamilyProperties);
                        rtELog_debug_logInfo("Found queue family, iterating %d", (*neededQueueFlags));
                        return physicalDeviceHasQueueFamilies(physDevice, neededQueueFlags);
                }
        }

        free(queueFamilyProperties);

        return false;
}


static bool physicalDeviceSupportsPresentation(
        VkPhysicalDevice physDevice,
        VkSurfaceKHR surface
        ) {
        // TODO: add more selection criteria parameters if I ever care about that sort of thing
        uint32_t numQueueFamilies;
        VkQueueFamilyProperties2* queueFamilyProperties = getQueueFamilyProperties(physDevice, &numQueueFamilies);

        for (size_t i = 0; i < numQueueFamilies; i++) {
                VkBool32 supported = VK_FALSE;
                vkGetPhysicalDeviceSurfaceSupportKHR(
                        physDevice,
                        i,
                        surface,
                        &supported
                        );
                if (supported == VK_TRUE) {
                        free(queueFamilyProperties);
                        return true;
                }
        }

        free(queueFamilyProperties);

        return false;
}

static bool checkPhysicalDeviceSuitability(
        VkPhysicalDevice physDevice ,
        VkSurfaceKHR surface
        ) {
        
        VkQueueFlagBits flags = VK_QUEUE_GRAPHICS_BIT;

        return physicalDeviceHasQueueFamilies(physDevice, &flags) && physicalDeviceSupportsPresentation(physDevice, surface);
}

enum VkResult rtER_VK_getSuitablePhysicalDevice(
        VkPhysicalDevice* dest,
        VkInstance instance,
        VkSurfaceKHR surface
        ) {
        uint32_t numPhysDevices;
        vkEnumeratePhysicalDevices(instance, &numPhysDevices, nullptr);

        VkPhysicalDevice* physicalDevices = malloc(sizeof(VkPhysicalDevice) * numPhysDevices);

        vkEnumeratePhysicalDevices(instance, &numPhysDevices, physicalDevices);

        for (size_t i = 0; i < numPhysDevices; i++) {
                if (checkPhysicalDeviceSuitability(physicalDevices[i], surface)) {
                        *dest = physicalDevices[i];
                        rtELog_debug_logInfo("Found a suitable physical device");
                        return VK_SUCCESS;
                }
        }
        
        rtELog_logError("Failed to find a suitable physical device");
        return VK_ERROR_VALIDATION_FAILED;
}
