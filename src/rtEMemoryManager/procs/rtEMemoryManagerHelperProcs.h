#ifndef RTEMEMORYMANAGERHELPERPROCS_H_
#define RTEMEMORYMANAGERHELPERPROCS_H_
#include "rtEMemoryManager/rtEMemoryManager.h"
#include <rtEErrorCodes/rtEErrorCodes.h>

enum rtEErrorCode rtEMM_findBlock(struct rtEMemoryManager* manager,  unsigned char** block, size_t blockSize, size_t allocatorSize);

enum rtEErrorCode rtEMM_dumpBlock(const char* outputDst, const unsigned char* block, size_t blockSize);

#endif //RTEMEMORYMANAGERHELPERPROCS_H_ 
