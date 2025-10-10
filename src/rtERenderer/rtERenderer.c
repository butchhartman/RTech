#include "rtERenderer/rtERenderer.h"
#include "rtEErrorCodes/rtEErrorCodes.h"
#include "rtERenderer/vulkan/rtERenderer_VulkanImpl.h"
#include <stdlib.h>

struct rtERenderer {
        enum rendererImplementationID implID;
        void* impl;
};

enum rtEErrorCode rtER_initializeRenderer(struct rtERenderer** renderer, enum rendererImplementationID implID) {
        *renderer = malloc(sizeof(struct rtERenderer));
        (*renderer)->implID = implID;

        switch (implID){
                case RENDERER_IMPL_ID_VULKAN:
                        return rtER_VK_initializeRenderer((struct rtER_VulkanImpl**)&(*renderer)->impl);
                        break;
                case RENDERER_IMPL_ID_OPENGL:
                        // :(
                        return rtEErrorCode_MEMORY_ALLOC_FAILURE;
                        break;
                default:
                        return rtEErrorCode_MEMORY_ALLOC_FAILURE;
                        break;
        };

}
