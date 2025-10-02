#ifndef RTEMEMORYMANAGERHELPERPROCS_H_
#define RTEMEMORYMANAGERHELPERPROCS_H_
#include "rtEMemoryManager/rtEMemoryManager.h"
#include <rtEMemoryManager/structs/rtEMemoryManagerStruct.h>
#include <rtEMemoryManager/macros/rtEMMMacros.h>
#include <rtEErrorCodes/rtEErrorCodes.h>
#include <stdio.h>
#include <string.h>

enum rtEErrorCode rtEMM_findBlock(struct rtEMemoryManager* manager,  unsigned char** block, size_t blockSize, size_t allocatorSize) {
        unsigned char* bufferPtr = manager->buff;
        *block = nullptr;

        size_t buffActualNeededSize = blockSize + allocatorSize;

        while (*block == nullptr) {
                printf("iter\n");
                if (bufferPtr >= (manager->buff + manager->buffSize)) {
                        printf("fail\n");
                        return rtEErrorCode_MEMORY_ALLOC_FAILURE;
                }

                uint32_t headerBlockSize = GET_BUFFER_SIZE(bufferPtr);
                uint8_t headerBlockOccupied = GET_BUFFER_IS_OCCUPIED(bufferPtr);
                printf("found block size: %u, occupied: %u\n", headerBlockSize, headerBlockOccupied);

                if (!headerBlockOccupied && headerBlockSize >= buffActualNeededSize) {
                        uint32_t deltaM = (bufferPtr - manager->buff);

                        printf("delta M: %u\n", deltaM);
                        *block = bufferPtr;

                        memcpy(manager->buff + deltaM, &buffActualNeededSize, sizeof(uint32_t));
                        *(manager->buff + (deltaM)+ sizeof(uint32_t)) = 0xFF;

                        uint32_t newSize = headerBlockSize - buffActualNeededSize; 
                        printf("ns: %u\n", newSize);
                        if (newSize != 0) {
                                memcpy(manager->buff + deltaM + buffActualNeededSize + IN_BAND_HEADER_SIZE, &newSize, sizeof(uint32_t));
                                *(manager->buff +deltaM + buffActualNeededSize + IN_BAND_HEADER_SIZE + sizeof(uint32_t)) = 0x00;
                        } else {
                                printf("No space for header, did not create\n");
                        }

                        *block += IN_BAND_HEADER_SIZE;

                } else if (blockSize == 0) {
                        return rtEErrorCode_MEMORY_ALLOC_FAILURE;
                } else {
                        bufferPtr += headerBlockSize + IN_BAND_HEADER_SIZE;
                }
        }

        return rtEErrorCode_SUCCESS;
}

#endif //RTEMEMORYMANAGERHELPERPROCS_H_ 
