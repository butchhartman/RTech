#include "rtERenderer/rtERenderer.h"
#include "rtEErrorCodes/rtEErrorCodes.h"
#include "rtERenderer/vulkan/rtERenderer_VulkanImpl.h"
#include "rtEW/rtenginewindow.h"
#include <stdlib.h>

// This is a bridge. It lets the outward facing abstraction vary independantly from its implementation
typedef enum rtEErrorCode (*pfn_rendererImplCleanup)(void** impl);
struct rtERenderer {
        enum rendererImplementationID implID;
        void* impl;
        pfn_rendererImplCleanup cleanup;
};

enum rtEErrorCode rtER_initializeRenderer(struct rtERenderer** renderer, struct rtEngineWindow* window, enum rendererImplementationID implID) {
        *renderer = malloc(sizeof(struct rtERenderer));
        (*renderer)->implID = implID;

        switch (implID){
                case RENDERER_IMPL_ID_VULKAN:
                        (*renderer)->cleanup = rtER_VK_cleanupRenderer;
                        return rtER_VK_initializeRenderer((struct rtER_VulkanImpl**)&(*renderer)->impl, window);
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

enum rtEErrorCode rtER_cleanupRenderer(struct rtERenderer** renderer) {
        enum rtEErrorCode err = (*renderer)->cleanup(&(*renderer)->impl);

        if (err != rtEErrorCode_SUCCESS) {
                return err;
        }

        free(*renderer);
        *renderer = nullptr;

        return rtEErrorCode_SUCCESS;
}
