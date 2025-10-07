#include "rtEErrorCodes/rtEErrorCodes.h"
#include "rtELog/rtELog.h"
#include <rtEMemoryManager/rtEMemoryManager.h>
#include <stdint.h>
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


enum rtEErrorCode rtEMM_createMemoryManager(struct rtEMemoryManager** obj, uint64_t buffSize) {
        // MSB is reserved for occupied bit, so the range of values is 0-2^63-1, which should be more than enough for at least the next 100 years
        if (buffSize > INT64_MAX) {
                // TODO: create real error code
                rtELog_logError("Memory Manager buffer size too large (what are you doing?)");
                return rtEErrorCode_MEMORY_ALLOC_FAILURE;
        }

        uint64_t buffSizeWithManager = sizeof(struct rtEMemoryManager) + buffSize + IN_BAND_HEADER_SIZE;
//        printf("BSWM: %llu", buffSizeWithManager);

        unsigned char* memoryBuffer = malloc(buffSizeWithManager);

        if (memoryBuffer == nullptr) {
                *obj = nullptr;
                rtELog_logError("Memory Manager memory allocation failed");
                return rtEErrorCode_MEMORY_ALLOC_FAILURE;
        }
        
        memset(memoryBuffer, 0, buffSizeWithManager);
        *obj = (struct rtEMemoryManager*)memoryBuffer;

 //       printf("buff actual size: %llu\n", buffSize);
        (*obj)->buff = memoryBuffer + sizeof(struct rtEMemoryManager);
        // Cannot assign to buffer in this way because it implicily converts the whole value to a char. memcpy is needed
//        *(*obj)->buff = 0xF0F0F0F0;

        // Should not need to zero the occupied bit because buffSize can never be big enough to use it
        memcpy((*obj)->buff, &buffSize, IN_BAND_HEADER_SIZE);
       (*obj)->buffSize = buffSize + IN_BAND_HEADER_SIZE;

//        memcpy((*obj)->buff, &buffSize, sizeof(uint32_t));
 //       *((*obj)->buff + sizeof(uint32_t)) = 0x00;
  //      (*obj)->buffSize = buffSize + IN_BAND_HEADER_SIZE;

  //      printf("extracted block size: %llu, block occupied: %llu\n", GET_BUFFER_SIZE((*obj)->buff), GET_BUFFER_IS_OCCUPIED((*obj)->buff));

        rtELog_debug_logInfo("Allocated a buffer of %d for a memory manager", buffSizeWithManager);
        return rtEErrorCode_SUCCESS;
}

enum rtEErrorCode rtEMM_cleanupMemoryManager(struct rtEMemoryManager** obj) {
        if (*obj == nullptr) {
                return rtEErrorCode_PASSED_NULL_PTR;
        }

       unsigned char* memoryBuffer = (*obj)->buff - sizeof(struct rtEMemoryManager); 

       free(memoryBuffer);

       *obj = nullptr;

       rtELog_debug_logInfo("Freed memory manager memory buffer");

       return rtEErrorCode_SUCCESS;
}

