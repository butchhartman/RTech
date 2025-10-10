#ifndef RTERENDERER_H_
#define RTERENDERER_H_
#include "rtEErrorCodes/rtEErrorCodes.h"

enum rendererImplementationID {
        RENDERER_IMPL_ID_VULKAN,
        RENDERER_IMPL_ID_OPENGL
};

struct rtERenderer;

enum rtEErrorCode rtER_initializeRenderer(struct rtERenderer** renderer, enum rendererImplementationID implID);

#endif // RTERENDERER_H_
