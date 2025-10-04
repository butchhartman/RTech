#include "rtEErrorCodes/rtEErrorCodes.h"
#include <rtEMemoryManager/procs/rtEMemoryManagerHelperProcs.h>
#include <rtEMemoryManager/structs/rtEMemoryManagerStruct.h>
#include <rtEMemoryManager/macros/rtEMMMacros.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static enum rtEErrorCode rtEMM_findBlockIter(struct rtEMemoryManager* manager, unsigned char** block, uint64_t blockSize, unsigned char* bufferPtr) {
        printf("iter\n");
        if (bufferPtr >= (manager->buff + manager->buffSize)) {
                printf("fail\n");
                return rtEErrorCode_MEMORY_ALLOC_FAILURE;
        }

        uint64_t headerBlockSize = GET_BUFFER_SIZE(bufferPtr);
        uint64_t headerBlockOccupied = GET_BUFFER_IS_OCCUPIED(bufferPtr);
        printf("found block size: %llu, occupied: %llu\n", headerBlockSize, headerBlockOccupied);

        if (!headerBlockOccupied && headerBlockSize >= blockSize) {
                uint64_t deltaM = (bufferPtr - manager->buff);

                printf("delta M: %llu\n", deltaM);
                *block = bufferPtr;

                uint64_t header = blockSize | BUFFER_IS_OCCUPIED_BIT;

                memcpy(manager->buff + deltaM, &header, IN_BAND_HEADER_SIZE);

                uint64_t newSize = headerBlockSize - blockSize; 
                printf("ns: %llu\n", newSize);
                if (newSize != 0) {
                        memcpy(manager->buff + deltaM + blockSize + IN_BAND_HEADER_SIZE, &newSize, IN_BAND_HEADER_SIZE);
//                                *(manager->buff +deltaM + buffActualNeededSize + IN_BAND_HEADER_SIZE + sizeof(uint32_t)) = 0x00;
                } else {
                        printf("No space for header, did not create\n");
                }

                *block += IN_BAND_HEADER_SIZE;
                return rtEErrorCode_SUCCESS;

        } else if (blockSize == 0) {
                return rtEErrorCode_MEMORY_ALLOC_FAILURE;
        }

        bufferPtr += headerBlockSize + IN_BAND_HEADER_SIZE;
        return rtEMM_findBlockIter(manager, block, blockSize, bufferPtr);
}

enum rtEErrorCode rtEMM_findBlock(struct rtEMemoryManager* manager,  unsigned char** block, uint64_t blockSize) {
        *block = nullptr;

        return rtEMM_findBlockIter(manager, block, blockSize, manager->buff);
}

enum rtEErrorCode rtEMM_dumpBlock(const char* outputDst, const unsigned char* block, size_t blockSize) {
        FILE* dumpFile;
        int failure = fopen_s(&dumpFile, outputDst, "wb");

        if (failure) {
                // TODO: Put relevant error code
                fclose(dumpFile);
                return rtEErrorCode_MEMORY_ALLOC_FAILURE;
        }

        for (size_t i = 0; i < blockSize; i++) {

                uint8_t* value = malloc(8);
                failure = memcpy_s(value, 8, (block+i), 8);
                if (failure) {
                        printf("GFAAAG");
                        free(value);
                        fclose(dumpFile);
                        return rtEErrorCode_MEMORY_ALLOC_FAILURE;
                }
                fprintf_s(dumpFile, "%d", *value);
                free(value);
        }

        fclose(dumpFile);

        return rtEErrorCode_SUCCESS;
}
