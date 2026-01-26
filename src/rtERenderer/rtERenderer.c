#include "rtERenderer/rtERenderer.h"
#include "rtEMath/rtEMath.h"
#include "rtERenderer/creation/rtER_VK_objectCreation.h"
#include "rtERenderer/macros/rtERendererVKMacros.h"
#include "rtEErrorCodes/rtEErrorCodes.h"
#include "rtELog/rtELog.h"
#include "rtERenderer/rtERenderer_VK_constants.h"
#include "rtEW/rtenginewindow.h"
#include "rtEW/vulkan/rtEW_VK_createSurface.h"
#include <assert.h>
#include <stdint.h>
#include <vulkan/vulkan.h>
#include <string.h>
#include <stdlib.h>
#include "rtERenderer/rtER_VK_struct_definitions.h"



 
enum rtEErrorCode rtER_initializeRenderer(struct rtERenderer** rendererPtr, struct rtEngineWindow* window) {
        assert(rendererPtr != nullptr);

        *rendererPtr = malloc(sizeof(struct rtERenderer));

        struct rtERenderer* renderer = *rendererPtr;

        rtELog_debug_logInfo("Allocated memory for rtERenderer in the impl pointer of rtERenderer");

        renderer->window = window;

        // TODO: Remove needing to put every relevant member in individually and just send through the whole renderer struct

        rtER_VK_createVKInstance(
                renderer,
                rtER_VK_requiredInstanceExtensions, 
                ARRAY_SIZE(rtER_VK_requiredInstanceExtensions), 
                rtER_VK_requiredValidationLayers, 
                ARRAY_SIZE(rtER_VK_requiredValidationLayers)
                );

        rtER_VK_createDebugMessenger(
                renderer
                );

        // this is a rtEW function, it makes sense to not give the windowing library access to the renderer definition.
        rtEW_VK_createSurface(
                &renderer->surface, 
                renderer->instance, 
                renderer->window
                );

        rtER_VK_getSuitablePhysicalDevice(
                renderer,
                VK_QUEUE_GRAPHICS_BIT,
                rtER_VK_requiredDeviceExtensions,
                ARRAY_SIZE(rtER_VK_requiredDeviceExtensions)
                );

        rtER_VK_createLogicalDevice(
                renderer,
                VK_QUEUE_GRAPHICS_BIT,
                rtER_VK_requiredDeviceExtensions,
                ARRAY_SIZE(rtER_VK_requiredDeviceExtensions),
                &renderer->queueInfo
        );

        rtER_VK_createSwapchain(
                renderer
        );

        rtER_VK_createImageViews(
                renderer
        );

        rtER_VK_createRenderpass(
                renderer
        );

        rtER_VK_createFramebuffers(
                renderer
        );

        rtER_VK_createDescriptorSetLayout(
                renderer,
                0,
                1,
                VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                VK_SHADER_STAGE_VERTEX_BIT,
                nullptr
        );

        rtER_VK_createGraphicsPipeline(
                renderer
        );

        rtER_VK_createCommandPool(
                renderer
        );

        rtER_VK_createCommandBuffer(
                &renderer->commandBuffer[0],
                renderer->logicalDevice,
                renderer->commandPool
        );

        rtER_VK_createCommandBuffer(
                &renderer->commandBuffer[1],
                renderer->logicalDevice,
                renderer->commandPool
        );

        rtER_VK_createFence(
                &renderer->queueExecuteFence[0],
                renderer->logicalDevice
                );
        rtER_VK_createFence(
                &renderer->queueExecuteFence[1],
                renderer->logicalDevice
                );

        rtER_VK_createSemaphore(
                &renderer->renderingFinishedSemaphores[0],
                renderer->logicalDevice
                );
        rtER_VK_createSemaphore(
                &renderer->renderingFinishedSemaphores[1],
                renderer->logicalDevice
                );

        rtER_VK_createSemaphore(
                &renderer->imageAvaiableSemaphore[0],
                renderer->logicalDevice
                );
        rtER_VK_createSemaphore(
                &renderer->imageAvaiableSemaphore[1],
                renderer->logicalDevice
                );

        renderer->uniformBufferCount = 0;
        renderer->uniformBuffers = nullptr;
        renderer->vertexBufferCount = 0;
        renderer->vertexBuffers = nullptr;

        /*
        rtER_VK_createBuffer(
                &renderer->vertexBuffer,
                sizeof(struct vertex) * 3,
                renderer->logicalDevice,
                renderer->physDevice,
                0,
                VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                VK_SHARING_MODE_EXCLUSIVE,
                0,
                nullptr,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
                );

        struct vertex vertices[3] = {
                {0.0, 0.5, 1.0, 0.0, 0.0, 1.0, 1.0, 1.0},
                {0.5, -0.5, 1.0, 0.0, 0.0, 1.0, 1.0, 1.0},
                {-0.5, -0.5, 1.0, 0.0, 0.0, 1.0, 1.0, 1.0}, // I may be stupid... watch out for culling...
        };

        // TODO: Now, I'm going to need a way to specify vertices through the rtER API
        // TODO: Perhaps also a way to create/reallocate buffers via the api. I think the way the pipeline consumes vertex data is either going to be set in stone or configurable via api. Probably set in stone because it seems excessive to try otherwise

        rtER_VK_bufferData(
                vertices,
                renderer->logicalDevice,
                renderer->vertexBuffer.bufferDeviceMemory,
                0,
                sizeof(struct vertex) * 3,
                0
        );

        rtER_VK_createDescriptorPool(
                        &renderer->UBODescriptorPool,
                        renderer->logicalDevice
                );

        rtER_VK_allocateDescriptorSets(
                        &renderer->UBODescriptorSet,
                        renderer->UBODescriptorSetLayout,
                        renderer->UBODescriptorPool,
                        renderer->logicalDevice
                );

        rtER_VK_createBuffer(
                &renderer->UBO,
                192,
                renderer->logicalDevice,
                renderer->physDevice,
                0,
                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VK_SHARING_MODE_EXCLUSIVE,
                0,
                nullptr,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
                );


        vec3 modelPos = {0.0, 0.0, -1.0};
        mat4 model;
        rtEMath_mat4CreateModel(modelPos, model);

        vec3 cameraPos = {1.0, 0.0, 0.0};
        vec3 cameraTargetPos = {0.0, 0.0, -1.0};
        vec3 up = {0.0, 1.0, 0.0};
        mat4 camera;
        rtEMath_mat4CreateLookAt(cameraPos, cameraTargetPos, up, camera);

        mat4 proj;
        rtEMath_mat4CreatePerspectiveProjection(1.5707, .01, 100, 16.0/9.0, proj);


        mat4 uboData[3] = {
                RTEMATH_MAT4_IDENTITY,
                RTEMATH_MAT4_IDENTITY,
                RTEMATH_MAT4_IDENTITY
        };
        memcpy(uboData, model, 64);
        memcpy(uboData+1, camera, 64);
        memcpy(uboData+2, proj, 64);

        rtER_VK_bufferData(
                uboData,
                renderer->logicalDevice,
                renderer->UBO.bufferDeviceMemory,
                0,
                192,
                0
        );

        struct VkDescriptorBufferInfo bufferInfo = {
                .buffer = renderer->UBO.buffer,
                .offset = 0,
                .range = VK_WHOLE_SIZE
        };

        struct VkWriteDescriptorSet writeSet = {
                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .pNext = nullptr,
                .dstSet = renderer->UBODescriptorSet,
                .dstBinding = 0,
                .dstArrayElement = 0,
                .descriptorCount = 1,
                .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .pBufferInfo = &bufferInfo,

        };

        vkUpdateDescriptorSets(
                        renderer->logicalDevice,
                        1,
                        &writeSet,
                        0,
                        nullptr);

        */

        renderer->currentFrame = 0;

        return rtEErrorCode_SUCCESS;
}

static void recreateBuffer(struct rtERenderer* renderer, struct rtER_VK_Buffer* buffer, size_t newSize) {
        struct rtER_VK_queueCapabilities queueCaps = {
                .queueFlags = VK_QUEUE_GRAPHICS_BIT,
                .presentationSupport = VK_FALSE
        };
        uint32_t queueIndex;
        // TODO: a little sussy, consider storing the graphics queue index/handle directly in renderer
        vkQueueWaitIdle(*rtER_VK_getQueueWithCapabilities(renderer->queueInfo, queueCaps, &queueIndex));
                vkDestroyBuffer(
                        renderer->logicalDevice,
                        buffer->buffer,
                        nullptr
                        );
                
                vkFreeMemory(
                        renderer->logicalDevice,
                        buffer->bufferDeviceMemory,
                        nullptr);

                rtER_VK_createBuffer(
                        buffer,
                        newSize,
                        renderer->logicalDevice,
                        renderer->physDevice,
                        0,
                        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                        VK_SHARING_MODE_EXCLUSIVE,
                        0,
                        nullptr,
                        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
                );
                // idk why this is here twice
                /*
                rtER_VK_createBuffer(
                        &renderer->vertexBuffer,
                        elementSize * elementCount,
                        renderer->logicalDevice,
                        renderer->physDevice,
                        0,
                        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                        VK_SHARING_MODE_EXCLUSIVE,
                        0,
                        nullptr,
                        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
                );
                */
}

void rtER_bufferVertexData(struct rtERenderer* renderer, rter_vbo_t vbo, void* data, size_t dataSize) {
        struct rtER_VK_Buffer* theBufferInQuestion = &renderer->vertexBuffers[*vbo];

        if (theBufferInQuestion->bufferSize < dataSize) {
                // destroy current buffer and create new sufficiently sized buffer
                recreateBuffer(renderer, theBufferInQuestion, dataSize);
        }
        // buffer data to buffer
        rtER_VK_bufferData(
                data,
                renderer->logicalDevice,
                theBufferInQuestion->bufferDeviceMemory,
                0,
                dataSize,
                0
                );
}

static struct rtER_VK_Buffer createNullBuffer() {
        struct rtER_VK_Buffer newNullBuff = {
                .buffer = nullptr,
                .bufferDeviceMemory = nullptr,
                .bufferSize = 0
        };

        return newNullBuff;
}

enum rtEErrorCode rtER_createVertexBuffer(struct rtERenderer* renderer, rter_vbo_t vbo) {
        renderer->vertexBuffers = 
                realloc(
                        renderer->vertexBuffers, 
                        sizeof(struct rtERenderer) * renderer->vertexBufferCount + 1
                );

        renderer->vertexBuffers[renderer->vertexBufferCount] = createNullBuffer();
        *vbo = renderer->vertexBufferCount;

        renderer->vertexBufferCount++;
        return rtEErrorCode_SUCCESS;
}

enum rtEErrorCode rtER_createUniformBuffer(struct rtERenderer* renderer, rter_ubo_t ubo) {
        renderer->uniformBuffers= 
                realloc(
                        renderer->uniformBuffers, 
                        sizeof(struct rtERenderer) * renderer->uniformBufferCount + 1
                );

        renderer->uniformBuffers[renderer->uniformBufferCount] = createNullBuffer();
        *ubo = renderer->uniformBufferCount;

        renderer->uniformBufferCount++;
        return rtEErrorCode_SUCCESS;
}

void rtER_bufferUniformData(struct rtERenderer* renderer, rter_ubo_t ubo, void* data, size_t dataSize) {
        struct rtER_VK_Buffer* theBufferInQuestion = &renderer->uniformBuffers[*ubo];

        if (theBufferInQuestion->bufferSize < dataSize) {
                // destroy current buffer and create new sufficiently sized buffer
                recreateBuffer(renderer, theBufferInQuestion, dataSize);
        }
        rtER_VK_bufferData(
                data,
                renderer->logicalDevice,
                theBufferInQuestion->bufferDeviceMemory,
                0,
                dataSize,
                0
        );

        struct VkDescriptorBufferInfo bufferInfo = {
                .buffer = theBufferInQuestion->buffer,
                .offset = 0,
                .range = dataSize
        };

        struct VkWriteDescriptorSet writeSet = {
                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .pNext = nullptr,
                // yeesh
                .dstSet = renderer->UBODescriptorSet,
                .dstBinding = 0,
                .dstArrayElement = 0,
                .descriptorCount = 1,
                .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .pBufferInfo = &bufferInfo,

        };

        vkUpdateDescriptorSets(
                        renderer->logicalDevice,
                        1,
                        &writeSet,
                        0,
                        nullptr);

}

void rtER_drawFrame(struct rtERenderer* renderer) {
        vkWaitForFences(
                renderer->logicalDevice,
                1,
                &renderer->queueExecuteFence[renderer->currentFrame],
                VK_TRUE,
                UINT64_MAX
                );

        uint32_t imageIndex;
        enum VkResult imageAcquireResult = vkAcquireNextImageKHR(renderer->logicalDevice, renderer->swapchain, UINT64_MAX, renderer->imageAvaiableSemaphore[renderer->currentFrame], VK_NULL_HANDLE, &imageIndex); 

        switch (imageAcquireResult) {
                case(VK_ERROR_OUT_OF_DATE_KHR):
                        // do recreation shenaneghans when I get around to it
                        return;
                        break;
                default:
                        break;
        }

        // Reset fences only after swapchain is confirmed to be ok to avoid infinite hanging
        vkResetFences(
                renderer->logicalDevice,
                1,
                &renderer->queueExecuteFence[renderer->currentFrame] 
        );

        VkCommandBufferBeginInfo cbBeginInfo = {
                .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
                .pNext = nullptr,
                .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
                .pInheritanceInfo = nullptr,
        };

        vkBeginCommandBuffer(renderer->commandBuffer[renderer->currentFrame], &cbBeginInfo);

        VkClearValue clearValue = {
                .color = {{0.0f, 0.0f, 0.0f, 0.0f}}
        };

        VkRenderPassBeginInfo renderPassBegin = {
                .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
                .pNext = nullptr,
                .renderPass = renderer->renderPass,
                .framebuffer = renderer->framebuffers[imageIndex], 
                .renderArea = {
                        .extent = {
                                .width = renderer->swapchainInfo.swapchianExtent.width,
                                .height = renderer->swapchainInfo.swapchianExtent.height
                        },
                        .offset = {
                                .x = 0,
                                .y = 0
                        }
                },
                .clearValueCount = 1,
                .pClearValues = &clearValue
        };

        vkCmdBeginRenderPass(renderer->commandBuffer[renderer->currentFrame], &renderPassBegin, VK_SUBPASS_CONTENTS_INLINE);

        vkCmdBindDescriptorSets(
                renderer->commandBuffer[renderer->currentFrame],
                VK_PIPELINE_BIND_POINT_GRAPHICS,
                renderer->pipelineLayout,
                0,
                1,
                &renderer->UBODescriptorSet,
                0,
                nullptr);

        vkCmdBindPipeline(renderer->commandBuffer[renderer->currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, renderer->graphicsPipeline);
        
        VkDeviceSize offset = 0;
        vkCmdBindVertexBuffers(
                renderer->commandBuffer[renderer->currentFrame],
                0,
                1,
                &renderer->vertexBuffer.buffer,
                &offset
        );

        vkCmdDraw(renderer->commandBuffer[renderer->currentFrame], renderer->vertexBuffer.bufferSize / sizeof(struct vertex), 1, 0, 0); // Remember - this  needs to know the # of vertices to draw

        vkCmdEndRenderPass(renderer->commandBuffer[renderer->currentFrame]);

        vkEndCommandBuffer(renderer->commandBuffer[renderer->currentFrame]);

        VkPipelineStageFlags waitStage = {
                VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
        };

        VkSubmitInfo submitInfo = {
                .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
                .waitSemaphoreCount = 1,
                .pWaitSemaphores = &renderer->imageAvaiableSemaphore[renderer->currentFrame],
                .pWaitDstStageMask = &waitStage,
                .commandBufferCount = 1,
                .pCommandBuffers = &renderer->commandBuffer[renderer->currentFrame],
                .signalSemaphoreCount = 1,
                .pSignalSemaphores = &renderer->renderingFinishedSemaphores[imageIndex] // image index
        };
        struct rtER_VK_queueCapabilities reqCapabilities = {
                .queueFlags = VK_QUEUE_GRAPHICS_BIT,
                .presentationSupport = VK_FALSE
        };
        uint32_t queueIndex;
        VkQueue* graphicsq = rtER_VK_getQueueWithCapabilities(
                renderer->queueInfo,
                reqCapabilities,
                &queueIndex
                );
        
        vkQueueSubmit(*graphicsq, 1, &submitInfo, renderer->queueExecuteFence[renderer->currentFrame]);

        VkPresentInfoKHR present = {
                .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
                .pNext = nullptr,
                .waitSemaphoreCount = 1,
                .pWaitSemaphores = &renderer->renderingFinishedSemaphores[imageIndex],
                .swapchainCount = 1,
                .pSwapchains = &renderer->swapchain,
                .pImageIndices = &imageIndex,
        };

        reqCapabilities.presentationSupport = VK_TRUE;
        graphicsq = rtER_VK_getQueueWithCapabilities(
                renderer->queueInfo,
                reqCapabilities,
                &queueIndex
                );

        vkQueuePresentKHR(*graphicsq, &present);

        renderer->currentFrame = (renderer->currentFrame + 1) % MAX_CONCURRENT_FRAMES;
}

enum rtEErrorCode rtER_cleanupRenderer(struct rtERenderer** rendererPtr) {
        struct rtERenderer* renderer = *rendererPtr;
        vkDeviceWaitIdle(renderer->logicalDevice);
        vkDestroyInstance(renderer->instance, nullptr);

        free(*rendererPtr);

        rtELog_debug_logInfo("Cleaned up Vulkan Renderer Resources");
        return rtEErrorCode_SUCCESS;
}
