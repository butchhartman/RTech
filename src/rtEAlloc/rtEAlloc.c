#include "rtEErrorCodes/rtEErrorCodes.h"
#include <rtEalloc/rtEAlloc.h>
#include <stddef.h>
#include <stdlib.h>
#include <assert.h>

struct rtEStackAllocator {
        unsigned char* stack;
        unsigned char* top;
        size_t sizeInBytes;
};

enum rtEErrorCode rtEAlloc_createStackAllocator(struct rtEStackAllocator** stackAllocator, size_t size) {
        *stackAllocator = malloc(sizeof(struct rtEStackAllocator));

        if (*stackAllocator == nullptr) {
                return rtEErrorCode_MEMORY_ALLOC_FAILURE;
        }

        (*stackAllocator)->stack = malloc(size);
        
        (*stackAllocator)->top = (*stackAllocator)->stack; 

        return rtEErrorCode_SUCCESS;
}

enum rtEErrorCode rtEAlloc_mallocPush(struct rtEStackAllocator* stackAllocator, size_t size, void** dest) {
        *dest = stackAllocator->top;
        stackAllocator->top += size;
        assert(stackAllocator->top <= stackAllocator->stack + stackAllocator->sizeInBytes);
        return rtEErrorCode_SUCCESS;
}

enum rtEErrorCode rtEAlloc_freeToBlock(struct rtEStackAllocator* stackAllocator, void* block) {
        assert(block >= (void*)stackAllocator->stack && block <= (void*)(stackAllocator->stack + stackAllocator->sizeInBytes));
        stackAllocator->top = block;
        return rtEErrorCode_SUCCESS;
}

enum rtEErrorCode rtEAlloc_cleanupStackAllocator(struct rtEStackAllocator** stackAllocator) {
        free((*stackAllocator)->stack);
        free(*stackAllocator);
        return rtEErrorCode_SUCCESS;
}
