#include "rtERenderer/vulkan/rtERenderer_VulkanImpl.h"
#include "rtEErrorCodes/rtEErrorCodes.h"
#include "rtELog/rtELog.h"
#include <vulkan/vulkan.h>
#include <stdlib.h>

struct rtER_VulkanImpl {
        VkInstance a;
};
 
enum rtEErrorCode rtER_VK_initializeRenderer(struct rtER_VulkanImpl** dest) {
        *dest = malloc(sizeof(struct rtER_VulkanImpl));

        rtELog_debug_logInfo("Allocated memory for rtER_VulkanImpl in the impl pointer of rtERenderer");

        return rtEErrorCode_SUCCESS;
}
