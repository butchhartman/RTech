#ifndef RTERENDERER_VULKAN_IMPL_H_
#define RTERENDERER_VULKAN_IMPL_H_
#include "rtEErrorCodes/rtEErrorCodes.h"

struct rtER_VulkanImpl;

enum rtEErrorCode rtER_VK_initializeRenderer(struct rtER_VulkanImpl** dest);

enum rtEErrorCode rtER_VK_cleanupRenderer(void** ptr);
#endif // RTERENDERER_VULKAN_IMPL_H_
