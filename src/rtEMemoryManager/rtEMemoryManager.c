#include "rtEErrorCodes/rtEErrorCodes.h"
#include <rtEMemoryManager/rtEMemoryManager.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define IN_BAND_HEADER_SIZE sizeof(uint32_t) + sizeof(uint8_t)

#define GET_BUFFER_SIZE(buffer) \
        *(uint32_t*)((buffer)) \

#define GET_BUFFER_IS_OCCUPIED(buffer) \
        *(uint8_t*)((buffer + IN_BAND_HEADER_SIZE - 1))

#define GET_BUFFER_SIZE_PTR(buffer) \
        (uint32_t*)((buffer)) \

#define GET_BUFFER_IS_OCCUPIED_PTR(buffer) \
        (uint8_t*)((buffer + IN_BAND_HEADER_SIZE - 1))

struct rtEMemoryManager {
        unsigned char* buff;
        uint32_t buffSize;
};
// How do I find free contiguous blocks to return?
// Solution: in-band headers. Each 'block' of memory has a value denoting its size and if it is free or not. I can search through this quickly.
// The buff pointer will always lie on a in-band header. The LSB will be whether the block is free, and the remaining MSBs will be the block size

// DONE BUT LEAVING THIS HERE SO I DONT FORGET I DID IT:
//DONE: Fix bug where header size is not accounted for when calculating the size of a block (on a stack allocator allocation) and occupied not being set correctly and 
//HAVENT DONE THIS THOUGH:
//TODO: Stop headers denoting size 0 from eating space in allocations

struct rtEMMStackAllocator {
        unsigned char* buff;
        size_t buffSize;
        unsigned char* top;
        struct rtEMemoryManager* manager;
};

enum rtEErrorCode rtEMM_createMemoryManager(struct rtEMemoryManager** obj, uint32_t buffSize) {
        uint32_t buffSizeWithManager = sizeof(struct rtEMemoryManager) + buffSize + IN_BAND_HEADER_SIZE;
        printf("BSWM: %u", buffSizeWithManager);

        unsigned char* memoryBuffer = malloc(buffSizeWithManager);

        if (memoryBuffer == nullptr) {
                *obj = nullptr;
                return rtEErrorCode_MEMORY_ALLOC_FAILURE;
        }
        
        memset(memoryBuffer, 0, buffSizeWithManager);
        *obj = (struct rtEMemoryManager*)memoryBuffer;

        printf("buff actual size: %u\n", buffSize);
        (*obj)->buff = memoryBuffer + sizeof(struct rtEMemoryManager);
        // Cannot assign to buffer in this way because it implicily converts the whole value to a char. memcpy is needed
//        *(*obj)->buff = 0xF0F0F0F0;
        memcpy((*obj)->buff, &buffSize, sizeof(uint32_t));
        *((*obj)->buff + sizeof(uint32_t)) = 0x00;
        (*obj)->buffSize = buffSize + IN_BAND_HEADER_SIZE;

        printf("extracted block size: %u, block occupied: %u\n", GET_BUFFER_SIZE((*obj)->buff), GET_BUFFER_IS_OCCUPIED((*obj)->buff));
        return rtEErrorCode_SUCCESS;
}

enum rtEErrorCode rtEMM_cleanupMemoryManager(struct rtEMemoryManager** obj) {
        if (*obj == nullptr) {
                return rtEErrorCode_PASSED_NULL_PTR;
        }

       unsigned char* memoryBuffer = (*obj)->buff - sizeof(struct rtEMemoryManager); 

       free(memoryBuffer);

       *obj = nullptr;

       return rtEErrorCode_SUCCESS;
}

enum rtEErrorCode rtEMM_allocateStackAllocator(struct rtEMemoryManager** parent, struct rtEMMStackAllocator** child, size_t buffSize) {
        size_t buffActualSize = buffSize + sizeof(struct rtEMMStackAllocator); //+ IN_BAND_HEADER_SIZE;
        size_t totalBuffSize = buffSize + sizeof(struct rtEMMStackAllocator) + IN_BAND_HEADER_SIZE;

        struct rtEMemoryManager* parentAllocator = *parent;

        // find empty block
        unsigned char* block = nullptr;
        unsigned char* bufferPtr = parentAllocator->buff;
        printf("requested block size: %u, STACKALLOCSIZE: %u, needed block size: %u\n", buffSize, sizeof(struct rtEMMStackAllocator), buffActualSize);
        while (block == nullptr) {
                printf("iter\n");
                if (bufferPtr > (parentAllocator->buff + parentAllocator->buffSize)) {
                        printf("fail\n");
                        return rtEErrorCode_MEMORY_ALLOC_FAILURE;
                }

                uint32_t blockSize = GET_BUFFER_SIZE(bufferPtr);
                uint8_t blockOccupied = GET_BUFFER_IS_OCCUPIED(bufferPtr);
                printf("found block size: %u, occupied: %u\n", blockSize, blockOccupied);

                if (!blockOccupied && blockSize >= buffActualSize) {
                        uint32_t deltaM = (bufferPtr - parentAllocator->buff);

                        printf("delta M: %u\n", deltaM);
                        block = bufferPtr;

                        //parentAllocator->buff += deltaM;

                        memcpy(parentAllocator->buff + deltaM, &buffActualSize, sizeof(uint32_t));
                        *(parentAllocator->buff + (deltaM)+ sizeof(uint32_t)) = 0xFF;

                        uint32_t newSize = blockSize - buffActualSize - IN_BAND_HEADER_SIZE;
                        //parentAllocator->buff += buffActualSize;
                        memcpy(parentAllocator->buff + deltaM +totalBuffSize , &newSize, sizeof(uint32_t));
                        *(parentAllocator->buff +deltaM + totalBuffSize  + sizeof(uint32_t)) = 0x00;

                        block += IN_BAND_HEADER_SIZE;

                } else if (blockSize == 0) {
                        return rtEErrorCode_MEMORY_ALLOC_FAILURE;
                } else {
                        bufferPtr += blockSize + IN_BAND_HEADER_SIZE;
                }
        }

        *child = (struct rtEMMStackAllocator*)block;
        block += sizeof(struct rtEMMStackAllocator);
        (*child)->buff = block;
        (*child)->buffSize = buffSize;
        (*child)->top = block;
        (*child)->manager = parentAllocator;

        return rtEErrorCode_SUCCESS;
}

enum rtEErrorCode rtEMM_cleanupStackAllocator(struct rtEMMStackAllocator** alloc) {
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
