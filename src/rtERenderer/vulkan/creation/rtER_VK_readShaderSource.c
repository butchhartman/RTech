#include "rtERenderer/vulkan/creation/rtER_VK_readShaderSource.h"
#include <stdio.h>
#include <stdlib.h>

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
