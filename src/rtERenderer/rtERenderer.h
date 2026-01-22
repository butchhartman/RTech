#ifndef RTERENDERER_VULKAN_IMPL_H_
#define RTERENDERER_VULKAN_IMPL_H_
#include "rtEMath/rtEMath.h"
#include "rtEErrorCodes/rtEErrorCodes.h"
#include "rtEW/rtenginewindow.h"

struct rtERenderer;

enum rtEErrorCode rtER_initializeRenderer(struct rtERenderer** renderer, struct rtEngineWindow* window);

void rtER_drawFrame(struct rtERenderer* renderer);

void rtER_bufferVertexData(struct rtERenderer* renderer, void* data, size_t elementSize, size_t elementCount);

void rtER_bufferUniformData(struct rtERenderer* renderer, size_t size, mat4 model, mat4 camera, mat4 proj);

enum rtEErrorCode rtER_cleanupRenderer(struct rtERenderer** rendererPtr);
#endif // RTERENDERER_VULKAN_IMPL_H_
