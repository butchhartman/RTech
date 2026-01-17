#ifndef RTERENDERER_VULKAN_IMPL_H_
#define RTERENDERER_VULKAN_IMPL_H_
#include "rtEMath/rtEMath.h"
#include "rtEErrorCodes/rtEErrorCodes.h"
#include "rtEW/rtenginewindow.h"

struct rtER_VulkanImpl;

enum rtEErrorCode rtER_VK_initializeRenderer(struct rtER_VulkanImpl** dest, struct rtEngineWindow* window);

void rtER_VK_drawFrame(void* vpImpl);

void rtER_VK_bufferVertexData(void* vkImpl, void* data, size_t elementSize, size_t elementCount);

void rtER_VK_bufferUniformData(void* vkImpl, size_t size, mat4 model, mat4 camera, mat4 proj);

enum rtEErrorCode rtER_VK_cleanupRenderer(void** ptr);
#endif // RTERENDERER_VULKAN_IMPL_H_
