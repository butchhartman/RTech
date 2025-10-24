#ifndef RTERENDERER_VULKAN_IMPL_H_
#define RTERENDERER_VULKAN_IMPL_H_
#include "rtEErrorCodes/rtEErrorCodes.h"
#include "rtEW/rtenginewindow.h"

struct rtER_VulkanImpl;

enum rtEErrorCode rtER_VK_initializeRenderer(struct rtER_VulkanImpl** dest, struct rtEngineWindow* window);

void rtER_VK_drawFrame(void* vpImpl);

enum rtEErrorCode rtER_VK_cleanupRenderer(void** ptr);
#endif // RTERENDERER_VULKAN_IMPL_H_
