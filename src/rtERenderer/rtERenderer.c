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

        renderer->uniformBufferCount = 0;
        renderer->uniformBuffers = nullptr;

        renderer->vertexBufferCount = 0;
        renderer->vertexBuffers = nullptr;

        renderer->boundVertexBuffersCount = 0;
        renderer->boundVertexBuffers = nullptr;

        renderer->currentFrame = 0;

        return rtEErrorCode_SUCCESS;
}

static void recreateBuffer(struct rtERenderer* renderer, struct rtER_VK_Buffer* buffer, size_t newSize, VkBufferUsageFlags bufferUsage) {
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
                        bufferUsage,
                        VK_SHARING_MODE_EXCLUSIVE,
                        0,
                        nullptr,
                        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
                );
}

void rtER_bufferVertexData(struct rtERenderer* renderer, rter_vbo_t vbo, void* data, size_t dataSize) {
        struct rtER_VK_Buffer* theBufferInQuestion = &renderer->vertexBuffers[*vbo];

        rtELog_debug_logInfo("BVD");

        rtELog_debug_logInfo("VBO SIZE: %d, NEEDED SIZE: %d", theBufferInQuestion->bufferSize, dataSize);

        if (theBufferInQuestion->bufferSize < dataSize) {
                // destroy current buffer and create new sufficiently sized buffer
                rtELog_debug_logInfo("Recreating VBO ID#%d. Old size: %d, New size: %d", *vbo, theBufferInQuestion->bufferSize, dataSize);
                recreateBuffer(renderer, theBufferInQuestion, dataSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
        }
        // Recreated buffer is being used below since the address of the buffer has been taken previously
        rtELog_debug_logInfo("BVD 2");
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

enum rtEErrorCode rtER_createVertexBuffer(struct rtERenderer* renderer, rter_vbo_t* vbo) {
        renderer->vertexBuffers = 
                realloc(
                        renderer->vertexBuffers, 
                        sizeof(struct rtERenderer) * (renderer->vertexBufferCount + 1)
                );

        renderer->vertexBuffers[renderer->vertexBufferCount] = createNullBuffer();
        *vbo = malloc(sizeof(uint32_t));
        **vbo = renderer->vertexBufferCount;

        renderer->vertexBufferCount++;
        return rtEErrorCode_SUCCESS;
}

enum rtEErrorCode rtER_createUniformBuffer(struct rtERenderer* renderer, rter_ubo_t* ubo) {
        renderer->uniformBuffers= 
                realloc(
                        renderer->uniformBuffers, 
                        sizeof(struct rtERenderer) * (renderer->uniformBufferCount + 1)
                );

        renderer->uniformBuffers[renderer->uniformBufferCount] = createNullBuffer();
        *ubo = malloc(sizeof(uint32_t));
        **ubo = renderer->uniformBufferCount;

        renderer->uniformBufferCount++;
        return rtEErrorCode_SUCCESS;
}

void rtER_bufferUniformData(struct rtERenderer* renderer, rter_ubo_t ubo, void* data, size_t dataSize) {
        struct rtER_VK_Buffer* theBufferInQuestion = &renderer->uniformBuffers[*ubo];

        if (theBufferInQuestion->bufferSize < dataSize) {
                // destroy current buffer and create new sufficiently sized buffer
                recreateBuffer(renderer, theBufferInQuestion, dataSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
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

void rtER_bindVertexBuffer(struct rtERenderer* renderer, rter_vbo_t vbo) {
        // Resize bound vertex buffers array to permit adding new vbo
        renderer->boundVertexBuffers = realloc(
                        renderer->boundVertexBuffers,
                        sizeof(VkBuffer) * (renderer->boundVertexBuffersCount+ 1)
                        );

        // Get the raw buffer and add it to the bound vertex buffers array
        renderer->boundVertexBuffers[renderer->boundVertexBuffersCount] = renderer->vertexBuffers[*vbo].buffer;
        // Track this addition
        renderer->boundVertexBuffersCount++;
}

void rtER_unbindVertexBuffer(struct rtERenderer* renderer, rter_vbo_t vbo) {
        // When loading vertex data, the buffer is destroyed and remade with a new size.
        // This obviously will make bound buffer handles stale and invalid, so currently
        // buffers must be unbound before buffering vertex data

        // Find and the buffer being unbound and shift other members downward

        bool foundBuffer = false;

        VkBuffer bufferBeingRemoved = renderer->vertexBuffers[*vbo].buffer;
        for (size_t i = 0; i < renderer->boundVertexBuffersCount; i++) {
                // Skip if not the correct buffer
                if (renderer->boundVertexBuffers[i] != bufferBeingRemoved) {
                        continue;
                }

                // Shift everything down one
                for (size_t j = i; j < renderer->boundVertexBuffersCount - 1; j++) {
                        renderer->boundVertexBuffers[j] = renderer->boundVertexBuffers[j + 1]; 
                }

                // Indicate that the buffer was found and the shifts have been made
                foundBuffer = true;
                break;
        }

        // Return without reallocating if buffer not found; bound buffer array unchanged
        if (!foundBuffer) {
                return;
        }
        
        renderer->boundVertexBuffersCount--;

        // Realloc has UB when size is zero. Array will not be resized if last vbo is removed
        if (renderer->boundVertexBuffersCount == 0) {
                return;
        }

        renderer->boundVertexBuffers = realloc(
                        renderer->boundVertexBuffers,
                        sizeof(VkBuffer) * (renderer->boundVertexBuffersCount)
                        );
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

        // Only draw if there is something to draw
        // Prevents crashing
        if (renderer->boundVertexBuffersCount > 0) {
                VkDeviceSize offset = 0;

                        // Specifies # vertex input bindings that are updated. MEANING: There must be a binding for each vertex buffer. 
                        // uh oh!!
                        // I'm dumb. A max VBO count would be stupid. All that error meant is that there can only be a certain amount of vertex input binding descriptions.
                        // See https://docs.vulkan.org/refpages/latest/refpages/source/vkCmdBindVertexBuffers.html
                vkCmdBindVertexBuffers(
                        renderer->commandBuffer[renderer->currentFrame],
                        0,
                        1, 
                        renderer->boundVertexBuffers,
                        &offset
                );

                // TODO: This shoul probably just be kept track of internally by the renderer so this loop isnt needed in this performance critical section lol
                size_t vboDataSizes = 0 ;
                for (size_t i = 0; i < renderer->vertexBufferCount; i++) {
                        vboDataSizes += renderer->vertexBuffers[i].bufferSize;
                }

                // Remember - this  needs to know the # of vertices to draw
                vkCmdDraw(renderer->commandBuffer[renderer->currentFrame], vboDataSizes / sizeof(struct vertex), 1, 0, 0); 
        }
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
