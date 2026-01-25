#include "rtERenderer/support/rtER_VK_support.h"
#include "rtERenderer/macros/rtERendererVKMacros.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "rtELog/rtELog.h"

VkBool32 rtER_VK_checkInstanceExtensionSupport(const char** requiredInstanceExtensions, uint32_t numRequiredInstanceExtensions) {
        uint32_t numSupportedInstanceExtensions;
        if (vkEnumerateInstanceExtensionProperties(nullptr, &numSupportedInstanceExtensions, nullptr) < 0) {
                rtELog_logError("Failed to enumerate instance extension properties");
                return VK_FALSE;
        }

        struct VkExtensionProperties* supportedExtensions = malloc(sizeof(struct VkExtensionProperties) * numSupportedInstanceExtensions);

        if (vkEnumerateInstanceExtensionProperties(nullptr, &numSupportedInstanceExtensions, supportedExtensions) < 0) {
                rtELog_logError("Failed to enumerate instance extension properties");
                free(supportedExtensions);
                return VK_FALSE;
        }

        for(size_t i = 0; i < numRequiredInstanceExtensions; i++) {
                bool extensionSupported = false;
                for(size_t j = 0; j < numSupportedInstanceExtensions; j++) {
                        if (strcmp(requiredInstanceExtensions[i], supportedExtensions[j].extensionName) == 0) {
                                extensionSupported = true;
                        }
                                
                }

                if (!extensionSupported) {
                        free(supportedExtensions);
                        return VK_FALSE;
                }

        }

        #ifndef NDEBUG
                rtELog_debug_logInfo("Required Instance Extensions:");

               for(size_t i = 0; i < numRequiredInstanceExtensions; i++) {
                       rtELog_debug_logInfo("\t%s", requiredInstanceExtensions[i]);
               }

               rtELog_debug_logInfo("Supported Instance Extensions:");

                for(size_t j = 0; j < numSupportedInstanceExtensions; j++) {
                       rtELog_debug_logInfo("\t%s", supportedExtensions[j].extensionName);
                }

        #endif



        free(supportedExtensions);
        rtELog_debug_logInfo("All required instance extensions supported");
        return VK_TRUE;
}


unsigned char* rtER_VK_readShaderSource(const char* shaderPath, uint32_t* codeSize) {
        FILE* shaderSource;

        errno_t err = fopen_s(&shaderSource, shaderPath, "rb");

        if (err != 0) {
                *codeSize = 0;
                return nullptr;
        }

        fseek(shaderSource, 0, SEEK_END);
        size_t shaderSrcSize = ftell(shaderSource);
        rewind(shaderSource);

        unsigned char* shaderCode = malloc(shaderSrcSize * sizeof(unsigned char));

        fread(shaderCode, sizeof(unsigned char), shaderSrcSize, shaderSource);

        fclose(shaderSource);

        *codeSize = shaderSrcSize;
        return shaderCode;
}

VkBool32 rtER_debug_checkValidationLayerSupport(const char** requiredValidationLayers, uint32_t numRequiredLayers) {
        uint32_t numSupportedLayers;
        VK_ERROR_LOG_AND_RETURN(
                vkEnumerateInstanceLayerProperties(
                        &numSupportedLayers,
                        nullptr
                ),
                "Failed to enumerate instance layer properties"
        );

        VkLayerProperties* layerPropertiesArray = malloc(sizeof(VkLayerProperties) * numSupportedLayers);

        VK_ERROR_LOG_AND_RETURN(
                vkEnumerateInstanceLayerProperties(
                        &numSupportedLayers,
                        layerPropertiesArray 
                ),
                "Failed to enumerate instance layer properties"
        );


        for(size_t i = 0; i < numRequiredLayers; i++) {
                bool requiredLayerSupported = false;
                for(size_t j = 0; j < numSupportedLayers; j++) {
                        if (strcmp(requiredValidationLayers[i], layerPropertiesArray[j].layerName) == 0) {
                                requiredLayerSupported = true;
                        }
                                
                }

                if (!requiredLayerSupported) {
                        free(layerPropertiesArray);
                        return VK_FALSE;
                }

        }
        
        #ifndef NDEBUG
                rtELog_debug_logInfo("Required Validation Layers:");

               for(size_t i = 0; i < numRequiredLayers; i++) {
                       rtELog_debug_logInfo("\t%s", requiredValidationLayers[i]);
               }

               rtELog_debug_logInfo("Supported Validation Layers:");

                for(size_t j = 0; j < numSupportedLayers; j++) {
                       rtELog_debug_logInfo("\t%s", layerPropertiesArray[j].layerName);
                }

        #endif


        free(layerPropertiesArray);
        rtELog_debug_logInfo("All required validation layers supported");
        return VK_TRUE;
}

VkBool32 VKAPI_PTR rtER_debug_debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageTypes,
        const struct VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData) {

        (void)messageSeverity;
        (void)messageTypes;
        (void)pUserData;

        rtELog_logWarning("%s", pCallbackData->pMessage);
/*
This function should always return VK_FALSE as per the spec:
https://docs.vulkan.org/spec/latest/chapters/debugging.html#PFN_vkDebugUtilsMessengerCallbackEXT
*/
        return VK_FALSE;
}
