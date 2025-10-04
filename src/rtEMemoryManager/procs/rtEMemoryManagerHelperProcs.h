#ifndef RTEMEMORYMANAGERHELPERPROCS_H_
#define RTEMEMORYMANAGERHELPERPROCS_H_
#include "rtEMemoryManager/rtEMemoryManager.h"
#include <rtEErrorCodes/rtEErrorCodes.h>
/**
* @file
*/

/**
* Finds a block of memory in manager which is blockSize + allocatorSize bytes large 
* and assigns it to block
*
* @note
* Blocks should be be blockSize + allocatorSize large, as the allocator itself will be
* typically be stored in the block.
*
* @param manager - The manager to allocate from
* @param block - The address of the pointer to return the start of the block to 
* @param blockSize - The size in bytes the requested block must be
*
*/
enum rtEErrorCode rtEMM_findBlock(struct rtEMemoryManager* manager,  unsigned char** block, uint64_t blockSize);

/**
* Writes the contents of block to a file in outputDst
*
* @param outputDst - The destination file to write the memory dump to
* @param block - The block of memory to dump
* @param blockSize - Size, in bytes, of block
*/
enum rtEErrorCode rtEMM_dumpBlock(const char* outputDst, const unsigned char* block, size_t blockSize);

#endif //RTEMEMORYMANAGERHELPERPROCS_H_ 
