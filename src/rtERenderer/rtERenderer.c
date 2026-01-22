#include "rtERenderer/rtERenderer.h"
#include "rtEMath/rtEMath.h"
#include "rtERenderer/creation/rtER_VK_infoCreation.h"
#include "rtERenderer/creation/rtER_VK_objectCreation.h"
#include "rtERenderer/debug/debugCallback.h"
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


constexpr size_t MAX_CONCURRENT_FRAMES = 2;

struct rtERenderer { 
        struct rtEngineWindow* window;
        uint32_t apiVersion; 
        VkInstance instance;
        VkDebugUtilsMessengerEXT debugMessenger;
        VkSurfaceKHR surface;
        VkPhysicalDevice physDevice;
        VkDevice logicalDevice;
        struct rtER_VK_queueInfo queueInfo;
        VkSwapchainKHR swapchain;
        struct rtER_VK_swapchainInfo swapchainInfo;
        VkImage* swapchainImages;
        uint32_t swapchainImageCount;
        VkImageView* swapchainImageViews;
        VkRenderPass renderPass;
        VkFramebuffer* framebuffers; // same length as image views, which is the same as swapchainimagecount
        VkPipeline graphicsPipeline;
        VkPipelineLayout pipelineLayout;

        VkCommandPool commandPool;
        VkCommandBuffer commandBuffer[MAX_CONCURRENT_FRAMES];
        VkFence queueExecuteFence[MAX_CONCURRENT_FRAMES];
        VkSemaphore imageAvaiableSemaphore[MAX_CONCURRENT_FRAMES];
        VkSemaphore renderingFinishedSemaphores[2]; // needs to be swapchain image count to have a semaphore fore each image. Hardcoded to 2 but could be less.
        size_t currentFrame;
        struct rtER_VK_Buffer vertexBuffer;
        VkDescriptorSetLayout UBODescriptorSetLayout;
        VkDescriptorPool UBODescriptorPool;
        VkDescriptorSet UBODescriptorSet;
        struct rtER_VK_Buffer UBO;
};

 
enum rtEErrorCode rtER_initializeRenderer(struct rtERenderer** rendererPtr, struct rtEngineWindow* window) {
        assert(rendererPtr != nullptr);

        *rendererPtr = malloc(sizeof(struct rtERenderer));

        struct rtERenderer* renderer = *rendererPtr;

        rtELog_debug_logInfo("Allocated memory for rtERenderer in the impl pointer of rtERenderer");

        renderer->window = window;

        // TODO: Remove needing to put every relevant member in individually and just send through the whole renderer struct

        rtER_VK_createVKInstance(
                &renderer->instance, 
                &renderer->apiVersion, 
                rtER_VK_requiredInstanceExtensions, 
                ARRAY_SIZE(rtER_VK_requiredInstanceExtensions), 
                rtER_VK_requiredValidationLayers, 
                ARRAY_SIZE(rtER_VK_requiredValidationLayers)
                );

        rtER_VK_createDebugMessenger(
                &renderer->debugMessenger, 
                renderer->instance,
                rtER_VK_getDebugMessengerCreateInfo(
                        VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
                        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                        VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,

                        VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                        VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT |
                        VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT,

                        rtER_debug_debugCallback
                        )
                );

        rtEW_VK_createSurface(
                &renderer->surface, 
                renderer->instance, 
                renderer->window
                );

        rtER_VK_getSuitablePhysicalDevice(
                &renderer->physDevice,
                renderer->instance,
                renderer->surface,
                VK_QUEUE_GRAPHICS_BIT,
                rtER_VK_requiredDeviceExtensions,
                ARRAY_SIZE(rtER_VK_requiredDeviceExtensions)
                );

        rtER_VK_createLogicalDevice(
                &renderer->logicalDevice,
                renderer->physDevice,
                &renderer->surface,
                VK_QUEUE_GRAPHICS_BIT,
                rtER_VK_requiredDeviceExtensions,
                ARRAY_SIZE(rtER_VK_requiredDeviceExtensions),
                &renderer->queueInfo
        );

        rtER_VK_createSwapchain(
        &renderer->swapchain,
        &renderer->swapchainInfo,
        renderer->surface,
        renderer->physDevice,
        renderer->logicalDevice,
        &renderer->swapchainImages,
        &renderer->swapchainImageCount
        );

        rtER_VK_createImageViews(
        &renderer->swapchainImageViews,
        renderer->swapchainInfo,
        renderer->swapchainImages,
        renderer->swapchainImageCount,
        renderer->logicalDevice
        );

        rtER_VK_createRenderpass(
        &renderer->renderPass,
        renderer->logicalDevice,
        renderer->swapchainInfo
        );

        rtER_VK_createFramebuffers(
                &renderer->framebuffers,
                renderer->logicalDevice,
                renderer->renderPass,
                renderer->swapchainImageViews,
                renderer->swapchainImageCount,
                renderer->swapchainInfo
        );

        rtER_VK_createDescriptorSetLayout(
                        &renderer->UBODescriptorSetLayout,
                        renderer->logicalDevice,
                        0,
                        1,
                        VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                        VK_SHADER_STAGE_VERTEX_BIT,
                        nullptr
                        );

        rtER_VK_createGraphicsPipeline(
                &renderer->graphicsPipeline,
                &renderer->pipelineLayout,
                renderer->logicalDevice,
                renderer->renderPass,
                renderer->swapchainInfo,
                renderer->UBODescriptorSetLayout
        );

        rtER_VK_createCommandPool(
                &renderer->commandPool,
                renderer->logicalDevice,
                renderer->queueInfo
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


        renderer->currentFrame = 0;

        return rtEErrorCode_SUCCESS;
}

void rtER_bufferVertexData(struct rtERenderer* renderer, void* data, size_t elementSize, size_t elementCount) {

        if (renderer->vertexBuffer.bufferSize < elementSize * elementCount) {
                // destroy current buffer and create new sufficiently sized buffer
                struct rtER_VK_queueCapabilities queueCaps = {
                        .queueFlags = VK_QUEUE_GRAPHICS_BIT,
                        .presentationSupport = VK_FALSE
                };
                uint32_t queueIndex;
        vkQueueWaitIdle(*rtER_VK_getQueueWithCapabilities(renderer->queueInfo, queueCaps, &queueIndex));
                vkDestroyBuffer(
                        renderer->logicalDevice,
                        renderer->vertexBuffer.buffer,
                        nullptr
                        );
                
                vkFreeMemory(
                        renderer->logicalDevice,
                        renderer->vertexBuffer.bufferDeviceMemory,
                        nullptr);

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
        }
        
        // buffer data to buffer
        rtER_VK_bufferData(
                data,
                renderer->logicalDevice,
                renderer->vertexBuffer.bufferDeviceMemory,
                0,
                elementCount * elementSize,
                0
                );
}

void rtER_bufferUniformData(struct rtERenderer* renderer, size_t size, mat4 model, mat4 camera, mat4 proj) {

        mat4 uboData[3];

        memcpy(uboData, model, 64);
        memcpy(uboData+1, camera, 64);
        memcpy(uboData+2, proj, 64);

        rtER_VK_bufferData(
                uboData,
                renderer->logicalDevice,
                renderer->UBO.bufferDeviceMemory,
                0,
                size,
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
