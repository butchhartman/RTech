#include "rtEErrorCodes/rtEErrorCodes.h"
#include "rtELog/rtELog.h"
#include <rtEMemoryManager/rtEMemoryManager.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <rtEMemoryManager/macros/rtEMMMacros.h>
#include <rtEMemoryManager/structs/rtEMemoryManagerStruct.h>


// How do I find free contiguous blocks to return?
// Solution: in-band headers. Each 'block' of memory has a value denoting its size and if it is free or not. I can search through this quickly.
// The buff pointer will always lie on a in-band header. The LSB will be whether the block is free, and the remaining MSBs will be the block size

// DONE BUT LEAVING THIS HERE SO I DONT FORGET I DID IT:
//DONE: Fix bug where header size is not accounted for when calculating the size of a block (on a stack allocator allocation) and occupied not being set correctly and 
//HAVENT DONE THIS THOUGH:
//TODO: Stop headers denoting size 0 from eating space in allocations


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
