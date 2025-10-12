#ifndef RTERENDERER_H_
#define RTERENDERER_H_
#include "rtEErrorCodes/rtEErrorCodes.h"

enum rendererImplementationID {
        RENDERER_IMPL_ID_VULKAN,
        RENDERER_IMPL_ID_OPENGL
};

struct rtERenderer;

enum rtEErrorCode rtER_initializeRenderer(struct rtERenderer** renderer, enum rendererImplementationID implID);

enum rtEErrorCode rtER_cleanupRenderer(struct rtERenderer** renderer);
#endif // RTERENDERER_H_
