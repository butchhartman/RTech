#ifndef RTERENDERER_H_
#define RTERENDERER_H_
#include "rtEErrorCodes/rtEErrorCodes.h"
#include "rtEW/rtenginewindow.h"

enum rendererImplementationID {
        RENDERER_IMPL_ID_VULKAN,
        RENDERER_IMPL_ID_OPENGL
};

struct rtERenderer;

enum rtEErrorCode rtER_initializeRenderer(struct rtERenderer** renderer, struct rtEngineWindow* window, enum rendererImplementationID implID);

void rtER_drawFrame(struct rtERenderer* renderer);

// Takes an array of vertex data structures. Must specifiy the size of each vertex data struct and the length of array
void rtER_bufferVertexData(struct rtERenderer* renderer, void* data, size_t elementSize, size_t elementCount);

enum rtEErrorCode rtER_cleanupRenderer(struct rtERenderer** renderer);
#endif // RTERENDERER_H_
