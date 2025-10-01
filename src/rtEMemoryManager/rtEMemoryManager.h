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

struct rtEMemoryManager;
struct rtEMMStackAllocator;

/**
* Allocates a memory manager with @param buffSize bytes
* 
* @note There is some memory overhead to the allocation so more than buffSize bytes will
* be allocated but buffSize bytes are guaranteed to be available
*
* @bug If an allocation fully consumes the available buffer, the next in-band header write will 
* cause a buffer overrun
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

// uses the manager's internal buffer to allocate a stack allocator. NOTE: the stack allocator is guaranteed to be buffSize bytes large, but will consume at least 37 additional bytes for storing itself.
/**
* Allocates a stack allocator using the memory contained within a @ref rtEMemoryManager
*
* @note
* The internal buffer of the stack allocator is guaranteed to be buffSize bytes large,
* but will consume at least 37 additional bytes from the parent memory manager 
*
* @param parent - The address of the memory manager to allocate from
* @param child - The address of the stack allocator object to allocate to
* @param buffSize - The number of bytes to be contained in the stack allocator
*/
enum rtEErrorCode rtEMM_allocateStackAllocator(struct rtEMemoryManager** parent, struct rtEMMStackAllocator** child, size_t buffSize);

/**
* Frees the memory associated with a stack allocated back to its parent @ref rtEMemoryManager 
*
* @param alloc - The address of the @ref rtEMMStackAllocator to free
*/
enum rtEErrorCode rtEMM_cleanupStackAllocator(struct rtEMMStackAllocator** alloc);

enum rtEErrorCode rtEMM_stackMalloc(struct rtEMMStackAllocator* alloc, uint32_t size, void** dest);
enum rtEErrorCode rtEMM_stackFreeTo(struct rtEMMStackAllocator* alloc, void** ptr);

#endif // RETMEMORYMANAGER_H_
