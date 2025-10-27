#include <stdlib.h>
#include <assert.h>
#include <stdint.h>
#include <string.h>
#include "rtELog/rtELog.h"
#include "rtERenderer/vulkan/creation/rtER_VK_infoCreation.h"
#include "rtERenderer/vulkan/creation/rtER_VK_readShaderSource.h"
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
                        (queueInfo.queueFlags[i].presentationSupport == requiredCapabilities.presentationSupport ||
                        (requiredCapabilities.presentationSupport == VK_FALSE && queueInfo.queueFlags[i].presentationSupport == VK_TRUE))
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

        infoDest->imageFormat = selectedSurfaceFormat.format;
        infoDest->swapchianExtent = swapchainCreateInfo.imageExtent;

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

enum VkResult rtER_VK_createRenderpass(
        VkRenderPass* dest,
        VkDevice logicalDevice,
        struct rtER_VK_swapchainInfo swapchainInfo
        ) {

        VkAttachmentDescription imageAttachmentDesc = {
                .flags = 0,
                .format = swapchainInfo.imageFormat,
                .samples = VK_SAMPLE_COUNT_1_BIT, // No MSAA
                .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
                .storeOp = VK_ATTACHMENT_STORE_OP_STORE, // Keeps what we draw
                .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE, // no stencil buffer, so dont care
                .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
                .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR // The layout becomes ready for presentation when the renderpass ends
        };

        VkAttachmentReference colorAttach = {
                .attachment = 0, // index of attachment in  renderpass create info pattachments
                .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
        };

        VkSubpassDescription subpassDesc = {
                .flags = 0,
                .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
                .inputAttachmentCount = 0,
                .pInputAttachments = nullptr,
                .colorAttachmentCount = 1,
                .pColorAttachments = &colorAttach,
                .pResolveAttachments = nullptr,
                .pDepthStencilAttachment = nullptr,
                .preserveAttachmentCount = 0,
                .pPreserveAttachments = nullptr
        };

        // TODO: Figure out how this thing works as a synchronizer
        // This makes the subpass wait for the VK_PIPELINAE_STAGE_COLOR_ATTACHMENT_OUPUT stage, which is the stage which waits for the WSI semaphore according to the vulkan sample. (I assume that this means the subpass waits until the color attachment output stage to do anything, otherwise it may not have access to an image)
        VkSubpassDependency subpassDependancy = {
                .srcSubpass = VK_SUBPASS_EXTERNAL,
                .dstSubpass = 0,
                .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                .srcAccessMask = 0,
                .dstAccessMask = 0,
                .dependencyFlags = 0
        };


        VkRenderPassCreateInfo createInfo = {
                .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,
                .attachmentCount = 1, // color attachment
                .pAttachments = &imageAttachmentDesc,
                .subpassCount = 1, // draw to image subpass
                .pSubpasses = &subpassDesc,
                .dependencyCount = 1,
                .pDependencies = &subpassDependancy
        };
        
        VK_ERROR_LOG_AND_RETURN(
                vkCreateRenderPass(
                        logicalDevice,
                        &createInfo,
                        nullptr,
                        dest),
                "Failed to create render pass");

        return VK_SUCCESS;
}

enum VkResult rtER_VK_createFramebuffers(
        VkFramebuffer** dest,
        VkDevice logicalDevice,
        VkRenderPass renderPass,
        VkImageView* imageViews,
        uint32_t numImageViews,
        struct rtER_VK_swapchainInfo swapchainInfo
        ) {

        (*dest) = malloc(sizeof(VkFramebuffer) * numImageViews);

        for (size_t i = 0; i < numImageViews; i++) {
                VkFramebufferCreateInfo createInfo = {
                        .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
                        .pNext = nullptr,
                        .flags = 0,
                        .renderPass = renderPass,
                        .attachmentCount = 1,
                        .pAttachments = &(imageViews[i]),
                        .width = swapchainInfo.swapchianExtent.width,
                        .height = swapchainInfo.swapchianExtent.height,
                        .layers = 1 // ?
                };

                VK_ERROR_LOG_AND_RETURN(
                        vkCreateFramebuffer(
                                logicalDevice,
                                &createInfo,
                                nullptr,
                                &(*dest)[i]
                        ),
                        "Failed to create framebuffer"
                );
        }

        return VK_SUCCESS;
}

enum VkResult rtER_VK_createShaderModule(
        VkShaderModule* dest,
        VkDevice logicalDevice,
        unsigned char* code,
        uint32_t codeSize) {
        VkShaderModuleCreateInfo createInfo = {
                .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,
                .codeSize = codeSize,
                .pCode = (uint32_t*)code
        };

        VK_ERROR_LOG_AND_RETURN(
                vkCreateShaderModule(
                        logicalDevice,
                        &createInfo,
                        nullptr,
                        dest
                ),
                "Failed to create shader module"
        );

        return VK_SUCCESS;
}

enum VkResult rtER_VK_createGraphicsPipeline(
        VkPipeline* dest,
        VkDevice logicalDevice,
        VkRenderPass renderpass,
        struct rtER_VK_swapchainInfo swapchainInfo
        ) {

        uint32_t vertexShaderSize;
        unsigned char* vertexShaderCode = rtER_VK_readShaderSource("shaders/default.vert.spv", &vertexShaderSize);

        uint32_t fragmentShaderSize;
        unsigned char* fragmentShaderCode = rtER_VK_readShaderSource("shaders/default.frag.spv", &fragmentShaderSize);

        VkShaderModule vertexShaderModule;
        VkShaderModule fragmentShaderModule;

        rtER_VK_createShaderModule(
                &vertexShaderModule,
                logicalDevice,
                vertexShaderCode,
                vertexShaderSize);

        rtER_VK_createShaderModule(
                &fragmentShaderModule,
                logicalDevice,
                fragmentShaderCode,
                fragmentShaderSize);

        free(fragmentShaderCode);
        free(vertexShaderCode);

        VkPipelineShaderStageCreateInfo shaderStages[2] = {
                {
                        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                        .pNext = nullptr,
                        .flags = 0,
                        .stage = VK_SHADER_STAGE_VERTEX_BIT,
                        .module = vertexShaderModule,
                        .pName = "main",
                        .pSpecializationInfo = nullptr
                },
                {
                        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                        .pNext = nullptr,
                        .flags = 0,
                        .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
                        .module = fragmentShaderModule,
                        .pName = "main",
                        .pSpecializationInfo = nullptr
                }
        };

        VkVertexInputBindingDescription inputBinding = {
                .binding = 0,
                .stride = sizeof(struct vertex),
                .inputRate = VK_VERTEX_INPUT_RATE_VERTEX
        };

        VkVertexInputAttributeDescription inputAttrib = {
                .location = 0,
                .binding = inputBinding.binding,
                .format = VK_FORMAT_R32G32B32_SFLOAT,
                .offset = 0
        };

        VkPipelineVertexInputStateCreateInfo vertexInputState = {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,
                .vertexBindingDescriptionCount = 1,
                .pVertexBindingDescriptions = &inputBinding,
                .vertexAttributeDescriptionCount = 1, // vertices are hardcoded, not needed
                .pVertexAttributeDescriptions = &inputAttrib,

        };

        VkPipelineInputAssemblyStateCreateInfo inputAssemblyState = {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,
                .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
                .primitiveRestartEnable = VK_FALSE
        };

        VkPipelineTessellationStateCreateInfo tesselationState = {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,
                .patchControlPoints = 1 // idk what this does but must be > 0
        };

        VkPipelineRasterizationStateCreateInfo rasterizationState = {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,
                .depthClampEnable = VK_FALSE,
                .rasterizerDiscardEnable = VK_FALSE,
                .polygonMode = VK_POLYGON_MODE_FILL,
                .cullMode = VK_CULL_MODE_BACK_BIT,
                .frontFace = VK_FRONT_FACE_CLOCKWISE,
                .depthBiasEnable = VK_FALSE,
                // there are other depth bias fields, but I dont think i need them cuz depth bias is disabled
                .lineWidth = 1.0
        };

        VkPipelineMultisampleStateCreateInfo multisampleState = {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,
                .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
                .sampleShadingEnable = VK_FALSE,
                // there are sample shading fields, but not needded cuz sample shading is off
                .alphaToCoverageEnable = VK_FALSE,
                .alphaToOneEnable = VK_FALSE
        };

        VkPipelineDepthStencilStateCreateInfo depthStencilState = {

                .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,
                .depthTestEnable = VK_FALSE,
                .depthWriteEnable = VK_FALSE,
                .depthBoundsTestEnable = VK_FALSE,
                .stencilTestEnable = VK_FALSE
                // once again, there are other fields that are only useful when everything isnt disabled
        };

        VkPipelineColorBlendAttachmentState colorBlendAttachment = {
                .blendEnable = VK_FALSE,
                .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
                //...
        };

        VkPipelineColorBlendStateCreateInfo colorBlendState = {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,
                .logicOpEnable = VK_FALSE,
                .logicOp = VK_LOGIC_OP_COPY,
                .attachmentCount = 1,
                .pAttachments = &colorBlendAttachment,
                //...
        };

        VkViewport viewport = {
                .x = 0,
                .y = swapchainInfo.swapchianExtent.height, // Changes origin to lower left corner
                .width = swapchainInfo.swapchianExtent.width,
                .height = -((float)swapchainInfo.swapchianExtent.height), // Changes viewport so < 0 is down and > 0 is up
                .minDepth = 0.0f,
                .maxDepth = 1.0f
        };

        VkRect2D scissor = {
                .offset = {
                        .x = 0,
                        .y = 0
                },
                .extent = {
                        .width = swapchainInfo.swapchianExtent.width,
                        .height = swapchainInfo.swapchianExtent.height
                }
        };

        VkPipelineViewportStateCreateInfo viewportStateCreateInfo = {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,
                .viewportCount = 1,
                .pViewports = &viewport,
                .scissorCount = 1,
                .pScissors = &scissor
        };

        VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,
                .setLayoutCount = 0,
                .pSetLayouts = nullptr,
                .pushConstantRangeCount = 0,
                .pPushConstantRanges = nullptr
        };

        VkPipelineLayout layout;

        vkCreatePipelineLayout(
                logicalDevice,
                &pipelineLayoutCreateInfo,
                nullptr,
                &layout
        );

        VkGraphicsPipelineCreateInfo createInfo = {
                .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,
                .stageCount = ARRAY_SIZE(shaderStages),
                .pStages = shaderStages,
                .pVertexInputState = &vertexInputState,
                .pInputAssemblyState = &inputAssemblyState,
                .pTessellationState = &tesselationState,
                .pViewportState = &viewportStateCreateInfo,
                .pRasterizationState = &rasterizationState,
                .pMultisampleState = &multisampleState,
                .pDepthStencilState = &depthStencilState,
                .pColorBlendState = &colorBlendState,
                // TODO: dynamic viewport and scissor
                .pDynamicState = nullptr,
                .layout = layout,
                .renderPass = renderpass,
                .subpass = 0,
                .basePipelineHandle = VK_NULL_HANDLE,
                .basePipelineIndex = 0
        };

        VK_ERROR_LOG_AND_RETURN(
                vkCreateGraphicsPipelines(
                        logicalDevice,
                        VK_NULL_HANDLE,
                        1,
                        &createInfo,
                        nullptr,
                        dest),
                        "Failed to create graphics pipeline"
        );

        return VK_SUCCESS;
}

enum VkResult rtER_VK_createCommandPool(
        VkCommandPool* dest,
        VkDevice logicalDevice,
        struct rtER_VK_queueInfo queueInfo 
        ) {

        struct rtER_VK_queueCapabilities reqCapa = {
                .queueFlags = VK_QUEUE_GRAPHICS_BIT,
                .presentationSupport = VK_FALSE
        };

        uint32_t qfi;
        rtER_VK_getQueueWithCapabilities(
                queueInfo,
                reqCapa,
                &qfi);

        VkCommandPoolCreateInfo createInfo = {
                .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
                .pNext = nullptr,
                .flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
                .queueFamilyIndex = qfi
        };

        VK_ERROR_LOG_AND_RETURN(
               vkCreateCommandPool(
                logicalDevice,
                &createInfo,
                nullptr,
                dest
               ),
               "Failed to create command pool"
        );

        return VK_SUCCESS;
}

enum VkResult rtER_VK_createCommandBuffer(
        VkCommandBuffer* dest,
        VkDevice logicalDevice,
        VkCommandPool commandPool
) {
        VkCommandBufferAllocateInfo commandBufferInfo = {
                .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
                .pNext = nullptr,
                .commandPool = commandPool,
                .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
                .commandBufferCount = 1
        };

        VK_ERROR_LOG_AND_RETURN(
                vkAllocateCommandBuffers(
                        logicalDevice,
                        &commandBufferInfo,
                        dest
                ),
                "Failed to create command buffer"
        );

        return VK_SUCCESS;
}

enum VkResult rtER_VK_createFence(
                VkFence* dest,
                VkDevice logicalDevice
        ) {
       VkFenceCreateInfo createInfo = {
               .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
               .pNext = nullptr,
               .flags = VK_FENCE_CREATE_SIGNALED_BIT
       };

       VK_ERROR_LOG_AND_RETURN(
                vkCreateFence(
                        logicalDevice,
                        &createInfo,
                        nullptr,
                        dest
                ),
                "Failed to create fence"
       );
        
        return VK_SUCCESS;
}

enum VkResult rtER_VK_createSemaphore(
                VkSemaphore *dest,
                VkDevice logicalDevice
        ) {
        VkSemaphoreCreateInfo createInfo = {
                .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0
        };

        VK_ERROR_LOG_AND_RETURN(
                vkCreateSemaphore(
                        logicalDevice,
                        &createInfo,
                        nullptr,
                        dest
                        ),
                "Failed to create semaphore"
                );

        return VK_SUCCESS;
}

static int64_t findMemoryTypeIndex(
        VkPhysicalDevice physDevice,
        VkMemoryPropertyFlags reqMemProps
        ) {

        VkPhysicalDeviceMemoryProperties memProps;
        vkGetPhysicalDeviceMemoryProperties(
                physDevice,
                &memProps
        );

        for (int64_t i = 0; i < memProps.memoryTypeCount; i++) {
                if ((memProps.memoryTypes[i].propertyFlags & reqMemProps) == reqMemProps) {
                        return i;
                }
        }


        return -1; // failure
}


enum VkResult rtER_VK_createBuffer(
        struct rtER_VK_Buffer* dest,
        size_t size,
        VkDevice logicalDevice,
        VkPhysicalDevice physDevice,
        VkBufferCreateFlags flags,
        VkBufferUsageFlags usage,
        VkSharingMode sharingMode,
        size_t queueCount,
        uint32_t* qfi,
        VkMemoryPropertyFlags memoryProperties
) {
        VkBufferCreateInfo createInfo = {
                .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
                .pNext = nullptr,
                .flags = flags,
                .size = size,
                .usage = usage,
                .sharingMode = sharingMode,
                .queueFamilyIndexCount = queueCount,
                .pQueueFamilyIndices = qfi
        };

        VkBuffer buffer;

        VK_ERROR_LOG_AND_RETURN(
                vkCreateBuffer(
                        logicalDevice,
                        &createInfo,
                        nullptr,
                        &buffer
                ),
                "Failed to create buffer"
        );

        VkMemoryRequirements memReqs;
        vkGetBufferMemoryRequirements(
                logicalDevice,
                buffer,
                &memReqs
        );

        int64_t memoryTypeIndex = findMemoryTypeIndex(
                                        physDevice,
                                        memoryProperties);
        VkMemoryAllocateInfo allocInfo = {
                .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
                .pNext = nullptr,
                .allocationSize = memReqs.size,
                .memoryTypeIndex = memoryTypeIndex
        };

        VkDeviceMemory memory;
        VK_ERROR_LOG_AND_RETURN(
                vkAllocateMemory(
                        logicalDevice,
                        &allocInfo,
                        nullptr,
                        &memory
                ),
                "Failed to allocate device memory"
        );

        VK_ERROR_LOG_AND_RETURN(
                vkBindBufferMemory(
                        logicalDevice,
                        buffer,
                        memory,
                        0),
                        "Failed to bind buffer memory"
                        );

        dest->buffer = buffer;
        dest->bufferDeviceMemory = memory;
        dest->bufferSize = size;

        return VK_SUCCESS;
}

enum VkResult rtER_VK_bufferData(
        struct vertex* data,
        VkDevice logicalDevice,
        VkDeviceMemory deviceMemory,
        VkDeviceSize offset,
        VkDeviceSize sizeToMap,
        VkMemoryMapFlags flags
        ) {

        void* pData;

        VK_ERROR_LOG_AND_RETURN(
                vkMapMemory(
                        logicalDevice,
                        deviceMemory,
                        offset,
                        sizeToMap,
                        flags,
                        &pData
                        ),
                "Failed to map device memory"
                );
        
        // memory is now mapped and can be written to via pData
        errno_t err = 
                memcpy_s(pData, sizeToMap, data, sizeToMap); // Dangerous!

        if (err != 0) {
                rtELog_logError("Failed to copy memory to device memory");
                return VK_ERROR_MEMORY_MAP_FAILED;
        }

        vkUnmapMemory(
                logicalDevice,
                deviceMemory
                );


        return VK_SUCCESS;
}
