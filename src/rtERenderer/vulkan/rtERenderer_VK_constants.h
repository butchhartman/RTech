#ifndef RTERENDERER_VK_CONSTANTS_H_
#define RTERENDERER_VK_CONSTANTS_H_

        const char* rtER_VK_requiredInstanceExtensions[] = { 
                "VK_KHR_surface",
                "VK_KHR_win32_surface",
                "VK_EXT_debug_utils",
        }; 

        const char* rtER_VK_requiredValidationLayers[] = {
                "VK_LAYER_KHRONOS_validation"
        };

        const char* rtER_VK_requiredDeviceExtensions[] = {
                "VK_KHR_swapchain"
        };

#endif // RTERENDERER_VK_CONSTANTS_H_
