#include "rtEErrorCodes/rtEErrorCodes.h"
#include <rtEMemoryManager/structs/rtEStackAllocatorStruct.h>
#include <rtEMemoryManager/procs/rtEMemoryManagerHelperProcs.h>
#include <stdio.h>
#include <string.h>

enum rtEErrorCode rtEMM_allocateStackAllocator(struct rtEMemoryManager* parent, struct rtEMMStackAllocator** child, size_t buffSize) {
        size_t buffActualSize = buffSize + sizeof(struct rtEMMStackAllocator); //+ IN_BAND_HEADER_SIZE;

        // find empty block
        unsigned char* block = nullptr;

        printf("requested block size: %u, STACKALLOCSIZE: %u, needed block size: %u\n", buffSize, sizeof(struct rtEMMStackAllocator), buffActualSize);
        enum rtEErrorCode err = rtEMM_findBlock(parent, &block, buffSize, sizeof(struct rtEMMStackAllocator));
        if (err != rtEErrorCode_SUCCESS) {
                *child = nullptr;
                return err;
        }


        *child = (struct rtEMMStackAllocator*)block;
        block += sizeof(struct rtEMMStackAllocator);
        (*child)->buff = block;
        (*child)->buffSize = buffSize;
        (*child)->top = block;
        (*child)->manager = parent;

        return rtEErrorCode_SUCCESS;
}

enum rtEErrorCode rtEMM_cleanupStackAllocator(struct rtEMMStackAllocator** alloc) {
        if (*alloc == nullptr) {
                return rtEErrorCode_PASSED_NULL_PTR;
        }

        struct rtEMMStackAllocator* stackAlloc = *alloc;

        *(stackAlloc->buff - 1 - sizeof(struct rtEMMStackAllocator)) = 0x00;
        memset(stackAlloc->buff - sizeof(struct rtEMMStackAllocator), 0, stackAlloc->buffSize + sizeof(struct rtEMMStackAllocator));
        stackAlloc->buff = nullptr;
        stackAlloc->top = nullptr;
        stackAlloc->manager = nullptr;
        stackAlloc->buffSize = 0;

        *alloc = nullptr;

        return rtEErrorCode_SUCCESS;
}

enum rtEErrorCode rtEMM_stackMalloc(struct rtEMMStackAllocator* alloc, uint32_t size, void** dest) {
        if (size > (alloc->buffSize - (alloc->buff - alloc->top))) {
                return rtEErrorCode_MEMORY_ALLOC_FAILURE;
        }

        *dest = alloc->top;
        alloc->top += size;

        return rtEErrorCode_SUCCESS;
}

enum rtEErrorCode rtEMM_stackFreeTo(struct rtEMMStackAllocator* alloc, void** ptr) {
        alloc->top = *ptr;
        *ptr = nullptr;

        return rtEErrorCode_SUCCESS;
}
