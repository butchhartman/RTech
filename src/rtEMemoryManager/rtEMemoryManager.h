#ifndef RTEMEMORYMANAGER_H_
#define RTEMEMORYMANAGER_H_ 
#include "rtEErrorCodes/rtEErrorCodes.h"
#include <stddef.h>
#include <stdint.h>

/**
* @file 
* The public api for the RTech memory manager
*/

// TODO: Expand documentation from these quick and dirty descriptions
// TODO: Fix looking at the implementation making me sad
// TODO: Separate the header and source files of the memory manager and allocators
// TODO: Change rtEMM in-band header encoding to use 64 bit ints instead of 32 bit. Perhaps use a single 64 bit int and reserve the LSB for occupied

struct rtEMemoryManager;

/**
* Allocates a memory manager with @param buffSize bytes
* 
* @note There is some memory overhead to the allocation so more than buffSize bytes will
* be allocated but buffSize bytes are guaranteed to be available
*
* @todo Write more tests, IDK if this is stable enough to use right now
*
* @param obj - The address of the memory manager to allocate
* @param buffSize - buffer size, in bytes, the memory manager will containe
*/
enum rtEErrorCode rtEMM_createMemoryManager(struct rtEMemoryManager** obj, uint32_t buffSize);

/**
* Frees resources associated with a memory manager and sets it to nullptr
* 
* @param obj - The address of the memory manager to free and set to nullptr
*/
enum rtEErrorCode rtEMM_cleanupMemoryManager(struct rtEMemoryManager** obj);



#endif // RETMEMORYMANAGER_H_
