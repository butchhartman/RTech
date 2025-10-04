#ifndef RTEMEMORYMANAGER_H_
#define RTEMEMORYMANAGER_H_ 
#include "rtEErrorCodes/rtEErrorCodes.h"
#include <stddef.h>
#include <stdint.h>

/**
* @file
*/

struct rtEMemoryManager;

/**
* Allocates a memory manager with @param buffSize bytes
* 
* @note There is some memory overhead to the allocation, so more than buffSize bytes will
* be allocated but buffSize bytes are guaranteed to be available
*
* @note
* While buffsize is a 64 bit integer, only 2^63 - 1 bytes are allowed to be allocated. 
* Attempting to allocate more will invariably cause an allocation failure.
*
* @todo Write more tests, IDK if this is stable enough to use right now
*
* @param obj - The address of the memory manager to allocate
* @param buffSize - buffer size, in bytes, the memory manager will containe
*/
enum rtEErrorCode rtEMM_createMemoryManager(struct rtEMemoryManager** obj, uint64_t buffSize);

/**
* Frees resources associated with a memory manager and sets it to nullptr
* 
* @param obj - The address of the memory manager to free and set to nullptr
*/
enum rtEErrorCode rtEMM_cleanupMemoryManager(struct rtEMemoryManager** obj);



#endif // RETMEMORYMANAGER_H_
