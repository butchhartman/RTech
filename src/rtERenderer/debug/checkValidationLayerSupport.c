#include "rtELog/rtELog.h"
#include "rtERenderer/debug/checkValidationLayerSupport.h"
#include "rtERenderer/macros/rtERendererVKMacros.h"
#include <stdlib.h>
#include <string.h>


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
