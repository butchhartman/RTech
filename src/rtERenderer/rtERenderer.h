#ifndef RTERENDERER_VULKAN_IMPL_H_
#define RTERENDERER_VULKAN_IMPL_H_
#include "rtEErrorCodes/rtEErrorCodes.h"
#include "rtEW/rtenginewindow.h"
#include <stdint.h>

typedef uint32_t* rter_vbo_t;
typedef uint32_t* rter_ubo_t;

struct rtERenderer;

enum rtEErrorCode rtER_initializeRenderer(struct rtERenderer** renderer, struct rtEngineWindow* window);

void rtER_drawFrame(struct rtERenderer* renderer);

enum rtEErrorCode rtER_createVertexBuffer(struct rtERenderer* renderer, rter_vbo_t* vbo);

enum rtEErrorCode rtER_createUniformBuffer(struct rtERenderer* renderer, rter_ubo_t* ubo);

void rtER_bindVertexBuffer(struct rtERenderer* renderer, rter_vbo_t vbo);

void rtER_unbindVertexBuffer(struct rtERenderer* renderer, rter_vbo_t vbo);

void rtER_bufferVertexData(struct rtERenderer* renderer, rter_vbo_t vbo, void* data, size_t dataSize);

void rtER_bufferUniformData(struct rtERenderer* renderer, rter_ubo_t ubo, void* data, size_t dataSize);

enum rtEErrorCode rtER_cleanupRenderer(struct rtERenderer** rendererPtr);
#endif // RTERENDERER_VULKAN_IMPL_H_
