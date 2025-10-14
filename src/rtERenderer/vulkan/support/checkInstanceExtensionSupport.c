#include <stdlib.h>
#include <string.h>
#include "rtELog/rtELog.h"
#include "rtERenderer/vulkan/support/checkInstanceExtensionSupport.h"

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
