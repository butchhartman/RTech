#include <stdlib.h>
#include <assert.h>
#include <stdint.h>
#include <string.h>
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

// TODO: Break this functionality into its own function 'isSubset' which returns true if parameter A is a subset of parameter B (string arrays) 
static bool physicalDeviceSupportsExtensions(
        VkPhysicalDevice physDevice,
        const char** requiredExtensions,
        uint32_t requiredExtensionsCount
        ) {
        uint32_t supportedExtensionsCount; 
        vkEnumerateDeviceExtensionProperties(physDevice, nullptr, &supportedExtensionsCount, nullptr); 

        VkExtensionProperties* supportedExtensions = malloc(sizeof(VkExtensionProperties) * supportedExtensionsCount);

        vkEnumerateDeviceExtensionProperties(physDevice, nullptr, &supportedExtensionsCount, supportedExtensions); 

        #ifndef NDEBUG
        rtELog_debug_logInfo("Required device extensions:");
        for (size_t i = 0; i < requiredExtensionsCount; i++) {
                rtELog_debug_logInfo("\t%s", requiredExtensions[i]);
        }

        rtELog_debug_logInfo("Supported device extensions:");
        for (size_t i = 0; i < supportedExtensionsCount; i++) {
                rtELog_debug_logInfo("\t%s", supportedExtensions[i].extensionName);
        }
        #endif

        for (size_t i = 0; i < requiredExtensionsCount; i++) {
                bool requiredExtensionSupported = false;
                for (size_t j = 0; j < supportedExtensionsCount; j++) {
                        if (strcmp(requiredExtensions[i], supportedExtensions[j].extensionName) == 0) {
                                requiredExtensionSupported = true;
                                break;
                        }
                }

                if (!requiredExtensionSupported) {
                        free(supportedExtensions);
                        return false;
                }
        }

        free(supportedExtensions);
        return true;
}

static bool checkPhysicalDeviceSuitability(
        VkPhysicalDevice physDevice ,
        VkSurfaceKHR surface,
        VkQueueFlagBits requiredQueueFlags,
        const char** requiredExtensions,
        uint32_t requiredExtensionsCount
        ) {
        
        return 
        physicalDeviceHasQueueFamilies(physDevice, &requiredQueueFlags) && 
        physicalDeviceSupportsPresentation(physDevice, surface) &&
        physicalDeviceSupportsExtensions(physDevice, requiredExtensions, requiredExtensionsCount);
}

enum VkResult rtER_VK_getSuitablePhysicalDevice(
        VkPhysicalDevice* dest,
        VkInstance instance,
        VkSurfaceKHR surface,
        VkQueueFlagBits requiredQueueFlags,
        const char** requiredExtensions,
        uint32_t requiredExtensionsCount
        ) {
        uint32_t numPhysDevices;
        vkEnumeratePhysicalDevices(instance, &numPhysDevices, nullptr);

        VkPhysicalDevice* physicalDevices = malloc(sizeof(VkPhysicalDevice) * numPhysDevices);

        vkEnumeratePhysicalDevices(instance, &numPhysDevices, physicalDevices);

        for (size_t i = 0; i < numPhysDevices; i++) {
                if (checkPhysicalDeviceSuitability(physicalDevices[i], surface, requiredQueueFlags, requiredExtensions, requiredExtensionsCount)) {
                        *dest = physicalDevices[i];
                        rtELog_debug_logInfo("Found a suitable physical device");
                        free(physicalDevices);
                        return VK_SUCCESS;
                }
        }
        
        rtELog_logError("Failed to find a suitable physical device");
        free(physicalDevices);
        return VK_ERROR_VALIDATION_FAILED;
}

static void addDeviceQueueCreateInfoToArray(
        VkDeviceQueueCreateInfo* dest,
        uint32_t queueCreateInfosCount,
        uint32_t queueFamilyIndex
        ) {

        (dest)[(queueCreateInfosCount) - 1].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        (dest)[(queueCreateInfosCount) - 1].pNext = nullptr;
        (dest)[(queueCreateInfosCount) - 1].flags = 0;
        (dest)[(queueCreateInfosCount) - 1].queueFamilyIndex = queueFamilyIndex;
        (dest)[(queueCreateInfosCount) - 1].queueCount = 1; // queues must have at least 1
        float* priority = malloc(sizeof(float));
        *priority = 1.0;
        (dest)[(queueCreateInfosCount) - 1].pQueuePriorities = priority;
}

static bool addPresentationQueueCreateInfoToArray(
        VkDeviceQueueCreateInfo** dest,
        VkPhysicalDevice physDevice,
        VkSurfaceKHR surface,
        uint32_t queueFamilyCount,
        uint32_t* queueCreateInfosCount
        ) {
        
        VkBool32 presentationSupported = VK_FALSE;
        for (size_t i = 0; i < queueFamilyCount; i++) {
                vkGetPhysicalDeviceSurfaceSupportKHR(
                        physDevice,
                        i,
                        surface,
                        &presentationSupported
                        );

                if (presentationSupported == VK_TRUE) {
                        bool indexUsed = false;
                        for (size_t j = 0; j < *queueCreateInfosCount; j++) {
                                if ((*dest)[j].queueFamilyIndex == i) {
                                        rtELog_debug_logInfo("Found a presentation queue, but it does not have a unique index.");
                                        indexUsed = true;
                                }
                        }
                        if (!indexUsed) {
                                (*queueCreateInfosCount)++;
                                *dest = realloc(*dest, sizeof(VkDeviceQueueCreateInfo) * (*queueCreateInfosCount));

                                addDeviceQueueCreateInfoToArray(
                                       *dest,
                                       *queueCreateInfosCount,
                                       i
                                );
                                rtELog_debug_logInfo("Found a unique presentation queue");
                        }
                        break;
                }
        }

        return presentationSupported == VK_TRUE;
}

static bool addQueuesWithFlagsToArray(
        VkDeviceQueueCreateInfo** dest,
        uint32_t* queueCreateInfosCount,
        VkQueueFlagBits requiredQueueTypeFlags,
        uint32_t numQueueFamilies,
        VkQueueFamilyProperties2* queueFamilyProperties
        ) {
        for (size_t i = 0; i < numQueueFamilies; i++) {
                if (queueFamilyProperties[i].queueFamilyProperties.queueFlags & requiredQueueTypeFlags) {

                        (*queueCreateInfosCount)++;
                        *dest = realloc(*dest, sizeof(VkDeviceQueueCreateInfo) * (*queueCreateInfosCount));

                        addDeviceQueueCreateInfoToArray(*dest, *queueCreateInfosCount, i);

                        requiredQueueTypeFlags = requiredQueueTypeFlags & ~(queueFamilyProperties[i].queueFamilyProperties.queueFlags & requiredQueueTypeFlags);
                        if (requiredQueueTypeFlags == 0) {
                                rtELog_debug_logInfo("Found all required queue bits");
                                break;
                        }
                }
        }

        return requiredQueueTypeFlags == 0;
}

static void freeDeviceQueueCreateInfos(
        VkDeviceQueueCreateInfo* ptr,
        uint32_t queueCreateInfosCount
        ) {

        for (size_t i = 0; i < queueCreateInfosCount; i++) {
                free((void*)ptr[i].pQueuePriorities);
        }

        free(ptr);
}

static bool getDeviceQueueCreateInfos(
        VkDeviceQueueCreateInfo** dest,
        uint32_t* queueCreateInfosCount,
        VkQueueFlagBits requiredQueueTypeFlags,
        VkPhysicalDevice physDevice,
        VkSurfaceKHR* surface
        ) {
        *dest = nullptr;
        *queueCreateInfosCount = 0;

        uint32_t numQueueFamilies;
        VkQueueFamilyProperties2* queueFamilyProperties = getQueueFamilyProperties(physDevice, &numQueueFamilies);

        bool foundQueuesWithFlags = addQueuesWithFlagsToArray(
                dest,
                queueCreateInfosCount,
                requiredQueueTypeFlags,
                numQueueFamilies,
                queueFamilyProperties
        );

        bool presentationSupported = false;
        if (surface != nullptr) {
                presentationSupported = addPresentationQueueCreateInfoToArray(
                        dest,
                        physDevice,
                        *surface,
                        numQueueFamilies,
                        queueCreateInfosCount
                );
        }

        free(queueFamilyProperties);
        // TODO: add logic to free queue create info array
        // (memory leak)
        return (surface == nullptr) ? foundQueuesWithFlags : foundQueuesWithFlags && presentationSupported ;
}
// TODO: Fix memory leak and find a way to specify if there needs to a be a queue that supports presentation
// OPTIONS: Use an unused bit to signift presentation (no)
//          Wrap vulkan flags in my own flag enum
//          Add a bool signifying presentation support check <-- probbaly
//          Assume presentation support is needed because this is a game engine :)
// SOLUTION: If the surface pointer is nullptr, then it is implied presentation is not wanted. if it is a valid pointer, then it is implied surface support is wanted
enum VkResult rtER_VK_createLogicalDevice(
        VkDevice* dest,
        VkPhysicalDevice physDevice,
        VkSurfaceKHR* surface,
        VkQueueFlagBits requiredQueueTypeFlags,
        const char** requiredExtensions,
        uint32_t requiredExtensionsCount
        ) {
        // instance extension support should already be inferred by finding a suitable physical device
        // create queue create infos
        // create device
        uint32_t queueCreateInfoCount;
        VkDeviceQueueCreateInfo* queueCreateInfos;
        if (!getDeviceQueueCreateInfos(&queueCreateInfos, &queueCreateInfoCount, requiredQueueTypeFlags, physDevice, surface)) {
                rtELog_logError("Required queues not supported");
                return VK_ERROR_INCOMPATIBLE_DRIVER;
        }

        VkDeviceCreateInfo createInfo = {
                .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,
                .queueCreateInfoCount = queueCreateInfoCount,
                .pQueueCreateInfos = queueCreateInfos,
                .enabledLayerCount = 0,
                .ppEnabledLayerNames = nullptr,
                .enabledExtensionCount = requiredExtensionsCount,
                .ppEnabledExtensionNames = requiredExtensions,
                .pEnabledFeatures = nullptr
        };

        VK_ERROR_LOG_AND_RETURN(vkCreateDevice(physDevice, &createInfo, nullptr, dest), "Failed to create logical device");

        rtELog_debug_logInfo("Successfully created logical device");

        freeDeviceQueueCreateInfos(queueCreateInfos, queueCreateInfoCount); 
        return VK_SUCCESS;
}

