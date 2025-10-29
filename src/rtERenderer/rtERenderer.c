#include "rtERenderer/rtERenderer.h"
#include "rtEErrorCodes/rtEErrorCodes.h"
#include "rtERenderer/vulkan/rtERenderer_VulkanImpl.h"
#include "rtEW/rtenginewindow.h"
#include <stdlib.h>

// TODO: I don't think I'm ever going to use a different graphics API. Kind of defeats the purpose of making a specialized vulkan renderer. Perhaps consider removing this abstraction layer in favor of simpler usage
// This is a bridge. It lets the outward facing abstraction vary independantly from its implementation
typedef enum rtEErrorCode (*pfn_rendererImplCleanup)(void** impl);
typedef void (*pfn_rendererImplDrawFrame)(void* impl);
typedef void (*pfn_rendererImplBufferVertexData)(void* impl, void* data, size_t elementSize, size_t elementCount);
struct rtERenderer {
        enum rendererImplementationID implID;
        void* impl;
        pfn_rendererImplCleanup cleanup;
        pfn_rendererImplDrawFrame drawFrame;
        pfn_rendererImplBufferVertexData bufferVertexData;
};

enum rtEErrorCode rtER_initializeRenderer(struct rtERenderer** renderer, struct rtEngineWindow* window, enum rendererImplementationID implID) {
        *renderer = malloc(sizeof(struct rtERenderer));
        (*renderer)->implID = implID;

        switch (implID){
                case RENDERER_IMPL_ID_VULKAN:
                        (*renderer)->cleanup = rtER_VK_cleanupRenderer;
                        (*renderer)->drawFrame = rtER_VK_drawFrame;
                        (*renderer)->bufferVertexData = rtER_VK_bufferVertexData;
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

void rtER_bufferVertexData(struct rtERenderer* renderer, void* data, size_t elementSize, size_t elementCount) {
        // TODO: Wait for buffer to not be in use before updating
        renderer->bufferVertexData(renderer->impl, data, elementSize, elementCount);
}

void rtER_drawFrame(struct rtERenderer* renderer) {
        renderer->drawFrame(renderer->impl);
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
