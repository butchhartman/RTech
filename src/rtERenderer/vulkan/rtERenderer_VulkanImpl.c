#include "rtERenderer/vulkan/rtERenderer_VulkanImpl.h"
#include "rtERenderer/vulkan/creation/rtER_VK_infoCreation.h"
#include "rtERenderer/vulkan/creation/rtER_VK_objectCreation.h"
#include "rtERenderer/vulkan/debug/debugCallback.h"
#include "rtERenderer/vulkan/macros/rtERendererVKMacros.h"
#include "rtEErrorCodes/rtEErrorCodes.h"
#include "rtELog/rtELog.h"
#include "rtERenderer/vulkan/rtERenderer_VK_constants.h"
#include "rtEW/rtenginewindow.h"
#include "rtEW/vulkan/rtEW_VK_createSurface.h"
#include "rtEMath/rtEMath.h"
#include <assert.h>
#include <stdint.h>
#include <vulkan/vulkan.h>
#include <string.h>
#include <stdlib.h>


constexpr size_t MAX_CONCURRENT_FRAMES = 2;

struct rtER_VulkanImpl { 
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
};

 
enum rtEErrorCode rtER_VK_initializeRenderer(struct rtER_VulkanImpl** dest, struct rtEngineWindow* window) {
        assert(dest != nullptr);

        *dest = malloc(sizeof(struct rtER_VulkanImpl));

        rtELog_debug_logInfo("Allocated memory for rtER_VulkanImpl in the impl pointer of rtERenderer");

        (*dest)->window = window;

        rtER_VK_createVKInstance(
                &(*dest)->instance, 
                &(*dest)->apiVersion, 
                rtER_VK_requiredInstanceExtensions, 
                ARRAY_SIZE(rtER_VK_requiredInstanceExtensions), 
                rtER_VK_requiredValidationLayers, 
                ARRAY_SIZE(rtER_VK_requiredValidationLayers)
                );

        rtER_VK_createDebugMessenger(
                &(*dest)->debugMessenger, 
                (*dest)->instance,
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
                &(*dest)->surface, 
                (*dest)->instance, 
                (*dest)->window
                );

        rtER_VK_getSuitablePhysicalDevice(
                &(*dest)->physDevice,
                (*dest)->instance,
                (*dest)->surface,
                VK_QUEUE_GRAPHICS_BIT,
                rtER_VK_requiredDeviceExtensions,
                ARRAY_SIZE(rtER_VK_requiredDeviceExtensions)
                );

        rtER_VK_createLogicalDevice(
                &(*dest)->logicalDevice,
                (*dest)->physDevice,
                &(*dest)->surface,
                VK_QUEUE_GRAPHICS_BIT,
                rtER_VK_requiredDeviceExtensions,
                ARRAY_SIZE(rtER_VK_requiredDeviceExtensions),
                &(*dest)->queueInfo
        );

        rtER_VK_createSwapchain(
        &(*dest)->swapchain,
        &(*dest)->swapchainInfo,
        (*dest)->surface,
        (*dest)->physDevice,
        (*dest)->logicalDevice,
        &(*dest)->swapchainImages,
        &(*dest)->swapchainImageCount
        );

        rtER_VK_createImageViews(
        &(*dest)->swapchainImageViews,
        (*dest)->swapchainInfo,
        (*dest)->swapchainImages,
        (*dest)->swapchainImageCount,
        (*dest)->logicalDevice
        );

        rtER_VK_createRenderpass(
        &(*dest)->renderPass,
        (*dest)->logicalDevice,
        (*dest)->swapchainInfo
        );

        rtER_VK_createFramebuffers(
                &(*dest)->framebuffers,
                (*dest)->logicalDevice,
                (*dest)->renderPass,
                (*dest)->swapchainImageViews,
                (*dest)->swapchainImageCount,
                (*dest)->swapchainInfo
        );

        rtER_VK_createGraphicsPipeline(
                &(*dest)->graphicsPipeline,
                &(*dest)->pipelineLayout,
                (*dest)->logicalDevice,
                (*dest)->renderPass,
                (*dest)->swapchainInfo
        );

        rtER_VK_createCommandPool(
                &(*dest)->commandPool,
                (*dest)->logicalDevice,
                (*dest)->queueInfo
        );

        rtER_VK_createCommandBuffer(
                &(*dest)->commandBuffer[0],
                (*dest)->logicalDevice,
                (*dest)->commandPool
        );

        rtER_VK_createCommandBuffer(
                &(*dest)->commandBuffer[1],
                (*dest)->logicalDevice,
                (*dest)->commandPool
        );

        rtER_VK_createFence(
                &(*dest)->queueExecuteFence[0],
                (*dest)->logicalDevice
                );
        rtER_VK_createFence(
                &(*dest)->queueExecuteFence[1],
                (*dest)->logicalDevice
                );

        rtER_VK_createSemaphore(
                &(*dest)->renderingFinishedSemaphores[0],
                (*dest)->logicalDevice
                );
        rtER_VK_createSemaphore(
                &(*dest)->renderingFinishedSemaphores[1],
                (*dest)->logicalDevice
                );

        rtER_VK_createSemaphore(
                &(*dest)->imageAvaiableSemaphore[0],
                (*dest)->logicalDevice
                );
        rtER_VK_createSemaphore(
                &(*dest)->imageAvaiableSemaphore[1],
                (*dest)->logicalDevice
                );

        rtER_VK_createBuffer(
                &(*dest)->vertexBuffer,
                sizeof(struct vertex) * 3,
                (*dest)->logicalDevice,
                (*dest)->physDevice,
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
                (*dest)->logicalDevice,
                (*dest)->vertexBuffer.bufferDeviceMemory,
                0,
                sizeof(struct vertex) * 3,
                0
        );

        (*dest)->currentFrame = 0;

        return rtEErrorCode_SUCCESS;
}

void rtER_VK_bufferVertexData(void* vkImpl, void* data, size_t elementSize, size_t elementCount) {
        struct rtER_VulkanImpl* vkContext = (struct rtER_VulkanImpl*)vkImpl;

        if (vkContext->vertexBuffer.bufferSize < elementSize * elementCount) {
                // destroy current buffer and create new sufficiently sized buffer
                struct rtER_VK_queueCapabilities queueCaps = {
                        .queueFlags = VK_QUEUE_GRAPHICS_BIT,
                        .presentationSupport = VK_FALSE
                };
                uint32_t queueIndex;
        vkQueueWaitIdle(*rtER_VK_getQueueWithCapabilities(vkContext->queueInfo, queueCaps, &queueIndex));
                vkDestroyBuffer(
                        vkContext->logicalDevice,
                        vkContext->vertexBuffer.buffer,
                        nullptr
                        );
                
                vkFreeMemory(
                        vkContext->logicalDevice,
                        vkContext->vertexBuffer.bufferDeviceMemory,
                        nullptr);

                rtER_VK_createBuffer(
                        &vkContext->vertexBuffer,
                        elementSize * elementCount,
                        vkContext->logicalDevice,
                        vkContext->physDevice,
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
                vkContext->logicalDevice,
                vkContext->vertexBuffer.bufferDeviceMemory,
                0,
                elementCount * elementSize,
                0
                );
}

void rtER_VK_drawFrame(void* vpImpl) {
        struct rtER_VulkanImpl* VkContext = (struct rtER_VulkanImpl*)vpImpl;

        vkWaitForFences(
                VkContext->logicalDevice,
                1,
                &VkContext->queueExecuteFence[VkContext->currentFrame],
                VK_TRUE,
                UINT64_MAX
                );

        uint32_t imageIndex;
        enum VkResult imageAcquireResult = vkAcquireNextImageKHR(VkContext->logicalDevice, VkContext->swapchain, UINT64_MAX, VkContext->imageAvaiableSemaphore[VkContext->currentFrame], VK_NULL_HANDLE, &imageIndex); 

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
                VkContext->logicalDevice,
                1,
                &VkContext->queueExecuteFence[VkContext->currentFrame] 
        );

        VkCommandBufferBeginInfo cbBeginInfo = {
                .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
                .pNext = nullptr,
                .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
                .pInheritanceInfo = nullptr,
        };

        vkBeginCommandBuffer(VkContext->commandBuffer[VkContext->currentFrame], &cbBeginInfo);

        VkClearValue clearValue = {
                .color = {{0.0f, 0.0f, 0.0f, 0.0f}}
        };

        VkRenderPassBeginInfo renderPassBegin = {
                .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
                .pNext = nullptr,
                .renderPass = VkContext->renderPass,
                .framebuffer = VkContext->framebuffers[imageIndex], 
                .renderArea = {
                        .extent = {
                                .width = VkContext->swapchainInfo.swapchianExtent.width,
                                .height = VkContext->swapchainInfo.swapchianExtent.height
                        },
                        .offset = {
                                .x = 0,
                                .y = 0
                        }
                },
                .clearValueCount = 1,
                .pClearValues = &clearValue
        };

        vkCmdBeginRenderPass(VkContext->commandBuffer[VkContext->currentFrame], &renderPassBegin, VK_SUBPASS_CONTENTS_INLINE);

        vkCmdBindPipeline(VkContext->commandBuffer[VkContext->currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, VkContext->graphicsPipeline);
        
        VkDeviceSize offset = 0;
        // TODO: Add mechanism for writing to this vertex buffer and update shaders to read from it
        vkCmdBindVertexBuffers(
                VkContext->commandBuffer[VkContext->currentFrame],
                0,
                1,
                &VkContext->vertexBuffer.buffer,
                &offset
        );

        vkCmdDraw(VkContext->commandBuffer[VkContext->currentFrame], VkContext->vertexBuffer.bufferSize / sizeof(struct vertex), 1, 0, 0); // Remember - this  needs to know the # of vertices to draw

        vkCmdEndRenderPass(VkContext->commandBuffer[VkContext->currentFrame]);

        vkEndCommandBuffer(VkContext->commandBuffer[VkContext->currentFrame]);

        VkPipelineStageFlags waitStage = {
                VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
        };

        VkSubmitInfo submitInfo = {
                .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
                .waitSemaphoreCount = 1,
                .pWaitSemaphores = &VkContext->imageAvaiableSemaphore[VkContext->currentFrame],
                .pWaitDstStageMask = &waitStage,
                .commandBufferCount = 1,
                .pCommandBuffers = &VkContext->commandBuffer[VkContext->currentFrame],
                .signalSemaphoreCount = 1,
                .pSignalSemaphores = &VkContext->renderingFinishedSemaphores[imageIndex] // image index
        };
        struct rtER_VK_queueCapabilities reqCapabilities = {
                .queueFlags = VK_QUEUE_GRAPHICS_BIT,
                .presentationSupport = VK_FALSE
        };
        uint32_t queueIndex;
        VkQueue* graphicsq = rtER_VK_getQueueWithCapabilities(
                VkContext->queueInfo,
                reqCapabilities,
                &queueIndex
                );
        
        vkQueueSubmit(*graphicsq, 1, &submitInfo, VkContext->queueExecuteFence[VkContext->currentFrame]);

        VkPresentInfoKHR present = {
                .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
                .pNext = nullptr,
                .waitSemaphoreCount = 1,
                .pWaitSemaphores = &VkContext->renderingFinishedSemaphores[imageIndex],
                .swapchainCount = 1,
                .pSwapchains = &VkContext->swapchain,
                .pImageIndices = &imageIndex,
        };

        reqCapabilities.presentationSupport = VK_TRUE;
        graphicsq = rtER_VK_getQueueWithCapabilities(
                VkContext->queueInfo,
                reqCapabilities,
                &queueIndex
                );

        vkQueuePresentKHR(*graphicsq, &present);

        VkContext->currentFrame = (VkContext->currentFrame + 1) % MAX_CONCURRENT_FRAMES;
}

enum rtEErrorCode rtER_VK_cleanupRenderer(void** ptr) {
        struct rtER_VulkanImpl** implPtr = (struct rtER_VulkanImpl**)ptr; 
        vkDeviceWaitIdle((*implPtr)->logicalDevice);
        vkDestroyInstance((*implPtr)->instance, nullptr);

        free(*implPtr);

        rtELog_debug_logInfo("Cleaned up Vulkan Renderer Resources");
        return rtEErrorCode_SUCCESS;
}
