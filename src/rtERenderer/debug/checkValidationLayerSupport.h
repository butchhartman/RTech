#ifndef CHECKVALIDATIONLAYERSUPPORT_H_
#define CHECKVALIDATIONLAYERSUPPORT_H_
#include <vulkan/vulkan.h>

VkBool32 rtER_debug_checkValidationLayerSupport(const char** requiredValidationLayers, uint32_t numRequiredLayers);

#endif // CHECKVALIDATIONLAYERSUPPORT_H_
