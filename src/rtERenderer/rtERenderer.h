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

enum rtEErrorCode rtER_cleanupRenderer(struct rtERenderer** renderer);
#endif // RTERENDERER_H_
