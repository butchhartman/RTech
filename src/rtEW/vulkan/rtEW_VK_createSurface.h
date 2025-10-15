#ifndef RTEW_VK_CREATESURFACE_H_
#define RTEW_VK_CREATESURFACE_H_
#include "rtEW/rtenginewindow.h"
#include <vulkan/vulkan.h>

enum VkResult rtEW_VK_createSurface(
        VkSurfaceKHR* dest,
        VkInstance instance,
        const struct rtEngineWindow* window
        );


#endif // RTEW_VK_CREATESURFACE_H_
