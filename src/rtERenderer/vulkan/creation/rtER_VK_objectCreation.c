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

// I'm going to use the 1.0 version of Vulkan here so I can understand how everything fits together (also bonues compatibility but i dont think that is really a concern). Once that is done, I will upgrade to future versions as I see fit.

VkQueue* rtER_VK_getQueueWithCapabilities(
        struct rtER_VK_queueInfo queueInfo, 
        struct rtER_VK_queueCapabilities requiredCapabilities, 
        uint32_t* queueFamilyIndex) {
        for (size_t i = 0; i < queueInfo.queueCount; i++) {
                if (
                        queueInfo.queueFlags[i].queueFlags == requiredCapabilities.queueFlags &&
                        queueInfo.queueFlags[i].presentationSupport == requiredCapabilities.presentationSupport
                   ) {
                        if (queueFamilyIndex != nullptr) {
                                *queueFamilyIndex = queueInfo.queueFamilyIndices[i];
                        }
                        return &queueInfo.queues[i];
                }

        }

        return nullptr;
}

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
                .apiVersion = VK_API_VERSION_1_0//VK_API_VERSION_1_4
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
// TODO: Change this to use the 1.0 version of vkqueuefamilyproperties
static VkQueueFamilyProperties* getQueueFamilyProperties(VkPhysicalDevice physDevice, uint32_t* count) {
        vkGetPhysicalDeviceQueueFamilyProperties(physDevice, count, nullptr);

        VkQueueFamilyProperties* queueFamilyProperties = malloc(sizeof(VkQueueFamilyProperties) * *count);
//        for (size_t i = 0; i < *count; i++) {
 //               queueFamilyProperties[i]. = VK_STRUCTURE_TYPE_QUEUE_FAMILY_PROPERTIES_2;
   //             queueFamilyProperties[i].pNext = nullptr;
  //      }

        vkGetPhysicalDeviceQueueFamilyProperties(physDevice, count, queueFamilyProperties);

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
        VkQueueFamilyProperties* queueFamilyProperties = getQueueFamilyProperties(physDevice, &numQueueFamilies);
        // TODO: add more selection criteria parameters if I ever care about that sort of thing

        rtELog_debug_logInfo("Need %d flags", (*neededQueueFlags));

        for (size_t i = 0; i < numQueueFamilies; i++) {
                if (queueFamilyProperties[i].queueFlags & *neededQueueFlags) {
                        (*neededQueueFlags) = (*neededQueueFlags) & ~(queueFamilyProperties[i].queueFlags & *neededQueueFlags);
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
        uint32_t numQueueFamilies;
        VkQueueFamilyProperties* queueFamilyProperties = getQueueFamilyProperties(physDevice, &numQueueFamilies);
        free(queueFamilyProperties);

        for (size_t i = 0; i < numQueueFamilies; i++) {
                VkBool32 supported = VK_FALSE;
                vkGetPhysicalDeviceSurfaceSupportKHR(
                        physDevice,
                        i,
                        surface,
                        &supported
                        );
                if (supported == VK_TRUE) {
                        return true;
                }
        }


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

static void freeDeviceQueueCreateInfos(
        VkDeviceQueueCreateInfo* ptr,
        uint32_t queueCreateInfosCount
        ) {

        for (size_t i = 0; i < queueCreateInfosCount; i++) {
                free((void*)ptr[i].pQueuePriorities);
        }

        free(ptr);
}

static bool populatertERQueueInfo(
        struct rtER_VK_queueInfo* dest,
        struct rtER_VK_queueCapabilities requiredQueueCapabilities,
        VkPhysicalDevice physDevice,
        VkSurfaceKHR surface
) {
        rtELog_debug_logInfo("Populating queue info struct");
        // zero everything so realloc will work correctly
        dest->queues = nullptr;
        dest->queueFamilyIndices = nullptr;
        dest->queueFlags = nullptr;
        dest->queueCount = 0;

        uint32_t queueFamilyCount;
        VkQueueFamilyProperties* queueFamilyProperties = getQueueFamilyProperties(physDevice, &queueFamilyCount);
        
        for (size_t i = 0; i < queueFamilyCount; i++) {
                VkBool32 queueSupportsPresentation = VK_FALSE;
                vkGetPhysicalDeviceSurfaceSupportKHR(
                        physDevice,
                        i,
                        surface,
                        &queueSupportsPresentation
                );
                if (
                        (queueFamilyProperties[i].queueFlags & requiredQueueCapabilities.queueFlags) || 
                        (requiredQueueCapabilities.presentationSupport && queueSupportsPresentation)

                        ) {
                        enum VkQueueFlagBits supportedQueueFlags = queueFamilyProperties[i].queueFlags & requiredQueueCapabilities.queueFlags;
                        VkBool32 presentationSupport = (requiredQueueCapabilities.presentationSupport && queueSupportsPresentation);
                        requiredQueueCapabilities.queueFlags &= ~(queueFamilyProperties[i].queueFlags & requiredQueueCapabilities.queueFlags);
                        requiredQueueCapabilities.presentationSupport = (presentationSupport) ? false : requiredQueueCapabilities.presentationSupport;

                        struct rtER_VK_queueCapabilities queueCapabilities = {
                                .queueFlags = supportedQueueFlags,
                                .presentationSupport = presentationSupport
                        };

                        dest->queueCount++;

                        dest->queues = realloc(dest->queues, sizeof(VkQueue) * dest->queueCount);
                        dest->queueFamilyIndices = realloc(dest->queueFamilyIndices, sizeof(uint32_t) * dest->queueCount);
                        dest->queueFlags= realloc(dest->queueFlags, sizeof(struct rtER_VK_queueCapabilities) * dest->queueCount);

                        dest->queues[dest->queueCount-1] = nullptr;
                        dest->queueFamilyIndices[dest->queueCount-1] = i;
                        dest->queueFlags[dest->queueCount-1] = queueCapabilities;

                        if (requiredQueueCapabilities.queueFlags == 0 && requiredQueueCapabilities.presentationSupport == false) {
                                rtELog_debug_logInfo("Found all required queues");
                                free(queueFamilyProperties);
                                return true;
                        }
                }
        }

        free(queueFamilyProperties);
        return false;
}

static bool getDeviceQueueCreateInfosFromrtERQueueInfo(
        VkDeviceQueueCreateInfo** dest,
        uint32_t* queueCreateInfoCount,
        struct rtER_VK_queueInfo queueInfo
) {
        *queueCreateInfoCount = queueInfo.queueCount;
        rtELog_debug_logInfo("Queue count: %lu", queueInfo.queueCount);
        *dest = malloc(sizeof(VkDeviceQueueCreateInfo) * queueInfo.queueCount);

        for (size_t i = 0; i < queueInfo.queueCount; i++) {
                (*dest)[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
                (*dest)[i].pNext = nullptr;
                (*dest)[i].flags = 0;
                (*dest)[i].queueFamilyIndex = queueInfo.queueFamilyIndices[i];
                (*dest)[i].queueCount = 1; // queues must have at least 1
                float* priority = malloc(sizeof(float));
                *priority = 1.0;
                (*dest)[i].pQueuePriorities = priority;
                rtELog_debug_logInfo("Added device queue index: %lu to the device queue create info array", queueInfo.queueFamilyIndices[i]);
        }

        return true;
}

static bool populationrtERQueueInfoQueueHandles(
        struct rtER_VK_queueInfo queueInfo,
        VkDevice device
) {
        for (size_t i = 0; i < queueInfo.queueCount; i++) {
                vkGetDeviceQueue(
                        device,
                       queueInfo.queueFamilyIndices[i],
                       0, // this is the index of the queue within the queue family. I only request 1 queue so this is always 0
                       &queueInfo.queues[i]
                );
                rtELog_debug_logInfo("Created queue handle for queue index %lu", queueInfo.queueFamilyIndices[i]);
        }

        return true;
}

// SOLUTION: If the surface pointer is nullptr, then it is implied presentation is not wanted. if it is a valid pointer, then it is implied surface support is wanted
enum VkResult rtER_VK_createLogicalDevice(
        VkDevice* dest,
        VkPhysicalDevice physDevice,
        VkSurfaceKHR* surface,
        VkQueueFlagBits requiredQueueTypeFlags,
        const char** requiredExtensions,
        uint32_t requiredExtensionsCount,
        struct rtER_VK_queueInfo* queueInfo
        ) {
        struct rtER_VK_queueCapabilities requiredQueueCapabilities = {
                .queueFlags = requiredQueueTypeFlags,
                .presentationSupport = !(surface == nullptr)
        };
        if (!populatertERQueueInfo(
                queueInfo,
                requiredQueueCapabilities,
                physDevice,
                *surface
                )) {
                rtELog_logError("Required queues not supported");
                return VK_ERROR_INCOMPATIBLE_DRIVER;
        }

        uint32_t queueCreateInfoCount;
        VkDeviceQueueCreateInfo* queueCreateInfos;
        getDeviceQueueCreateInfosFromrtERQueueInfo(
                &queueCreateInfos,
                &queueCreateInfoCount,
                *queueInfo
        );

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

        populationrtERQueueInfoQueueHandles(*queueInfo, *dest);

        rtELog_debug_logInfo("Successfully retrieved all queue handles");

        freeDeviceQueueCreateInfos(queueCreateInfos, queueCreateInfoCount); 
        return VK_SUCCESS;
}

// TODO: dynamically adjust sharing modes to account for graphics and present queues not being the same

//TODO: create render pass objects

enum VkResult rtER_VK_createSwapchain(
        VkSwapchainKHR* dest,
        struct rtER_VK_swapchainInfo* infoDest,
        VkSurfaceKHR surface,
        VkPhysicalDevice physDevice,
        VkDevice logicalDevice,
        VkImage** swapchainImages,
        uint32_t* swapchainImageCount
        ) {
        
        VkSurfaceCapabilitiesKHR surfaceCapabilities;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
                physDevice,
                surface,
                &surfaceCapabilities
                );

        uint32_t numSurfaceFormats;
        vkGetPhysicalDeviceSurfaceFormatsKHR(
                physDevice,
                surface,
                &numSurfaceFormats,
                nullptr
                );
        VkSurfaceFormatKHR* surfaceFormats = malloc(sizeof(VkSurfaceFormatKHR) * numSurfaceFormats);
        vkGetPhysicalDeviceSurfaceFormatsKHR(
                physDevice,
                surface,
                &numSurfaceFormats,
                surfaceFormats
                );

       VkSurfaceFormatKHR selectedSurfaceFormat = {};

        for (size_t i = 0; i < numSurfaceFormats; i++) {
                if (surfaceFormats[i].format == VK_FORMAT_R8G8B8A8_SRGB &&
                    surfaceFormats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                        rtELog_debug_logInfo("Found preferred image format");
                        selectedSurfaceFormat = surfaceFormats[i];
                        break;
                }
                selectedSurfaceFormat = surfaceFormats[0];
        }

        infoDest->imageFormat = selectedSurfaceFormat.format;

        VkSwapchainCreateInfoKHR swapchainCreateInfo = {
                .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
                .pNext = nullptr,
                .flags = 0,
                .surface = surface,
                .minImageCount = surfaceCapabilities.minImageCount,
                .imageFormat = selectedSurfaceFormat.format,
                .imageColorSpace = selectedSurfaceFormat.colorSpace,
                .imageExtent = surfaceCapabilities.currentExtent,
                .imageArrayLayers = 1,
                .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
                .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE, // will cause errors if present and graphics queue are not equal
                .queueFamilyIndexCount = 0,// only used when concurrent 
                .pQueueFamilyIndices = nullptr, // only used when concurrent
                .preTransform = surfaceCapabilities.currentTransform,
                .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR, // TODO: should check for support on this
                .presentMode = VK_PRESENT_MODE_FIFO_KHR, // fifo always supported
                .clipped = VK_TRUE,
                .oldSwapchain = nullptr
        };

        VK_ERROR_LOG_AND_RETURN(vkCreateSwapchainKHR(
               logicalDevice,
               &swapchainCreateInfo,
               nullptr,
               dest
        ), "Failed to create swapchain");

        VK_ERROR_LOG_AND_RETURN(vkGetSwapchainImagesKHR(
                logicalDevice,
                *dest,
                swapchainImageCount,
                nullptr
        ), "Failed to retrieve swapchain image count");

        (*swapchainImages) = malloc(sizeof(VkImage) * *swapchainImageCount);
        VK_ERROR_LOG_AND_RETURN(vkGetSwapchainImagesKHR(
                logicalDevice,
                *dest,
                swapchainImageCount,
                *swapchainImages
        ), "Failed to retrieve swapchain images");


        return VK_SUCCESS;
}

enum VkResult rtER_VK_createImageViews(
        VkImageView** dest,
        struct rtER_VK_swapchainInfo swapchainInfo,
        VkImage* images,
        uint32_t imageCount,
        VkDevice logicalDevice
        ) {
        *dest = malloc(sizeof(VkImageView) * imageCount);
        for (size_t i = 0; i < imageCount; i++) {
                VkImageViewCreateInfo createInfo = {
                        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
                        .pNext = nullptr,
                        .flags = 0,
                        .image = images[i],
                        .viewType = VK_IMAGE_VIEW_TYPE_2D,
                        .format = swapchainInfo.imageFormat,
                        .components = {
                                .r = VK_COMPONENT_SWIZZLE_R,
                                .g = VK_COMPONENT_SWIZZLE_G,
                                .b = VK_COMPONENT_SWIZZLE_B,
                                .a = VK_COMPONENT_SWIZZLE_A,

                        },

                        .subresourceRange = {
                                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                                .baseMipLevel = 0,
                                .levelCount = 1,
                                .baseArrayLayer = 0,
                                .layerCount = 1
                        }
                };

                VK_ERROR_LOG_AND_RETURN(
                        vkCreateImageView(
                                logicalDevice,
                                &createInfo,
                                nullptr,
                                &(*dest)[i]
                        ),

                        "Failed to create image view"
                ); 

        }

        return VK_SUCCESS;
}
/*
enum VkResult rtER_VK_createRenderpass(
        VkRenderPass* dest,
        VkDevice logicalDevice
        ) {

        VkAttachmentDescription imageAttachmentDesc = {
                .flags = 0,
                .format =
        }


        VkRenderPassCreateInfo createInfo = {
                .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,
                .attachmentCount = 1, // color attachment
                .subpassCount = 1, // draw to image subpass
                .dependencyCount = 0,
                .pDependencies = nullptr
        };
        

        return VK_SUCCESS;
}
*/
