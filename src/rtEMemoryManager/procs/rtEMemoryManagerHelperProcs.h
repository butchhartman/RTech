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
* @param manager - The manager to allocate from
* @param block - The address of the pointer to return the start of the block to 
* @param blockSize - The size in bytes the requested block must be
* @param allocatorSize - The size of the allocator that will utilize the found block. 
* Blocks must be be blockSize + allocatorSize large, as the allocator itself will be
* stored in the block.
*/
enum rtEErrorCode rtEMM_findBlock(struct rtEMemoryManager* manager,  unsigned char** block, size_t blockSize, size_t allocatorSize);

/**
* Writes the contents of block to a file in outputDst
*
* @param outputDst - The destination file to write the memory dump to
* @param block - The block of memory to dump
* @param blockSize - Size, in bytes, of block
*/
enum rtEErrorCode rtEMM_dumpBlock(const char* outputDst, const unsigned char* block, size_t blockSize);

#endif //RTEMEMORYMANAGERHELPERPROCS_H_ 
