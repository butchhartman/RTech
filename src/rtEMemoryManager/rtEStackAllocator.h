#ifndef RTESTACKALLOCATOR_H_ 
#define RTESTACKALLOCATOR_H_
#include <rtEErrorCodes/rtEErrorCodes.h>
#include <stdint.h>

// forward dec
struct rtEMemoryManager;

struct rtEMMStackAllocator;

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
enum rtEErrorCode rtEMM_allocateStackAllocator(struct rtEMemoryManager* parent, struct rtEMMStackAllocator** child, size_t buffSize);

/**
* Frees the memory associated with a stack allocated back to its parent @ref rtEMemoryManager 
*
* @param alloc - The address of the @ref rtEMMStackAllocator to free
*/
enum rtEErrorCode rtEMM_cleanupStackAllocator(struct rtEMMStackAllocator** alloc);

enum rtEErrorCode rtEMM_stackMalloc(struct rtEMMStackAllocator* alloc, uint32_t size, void** dest);
enum rtEErrorCode rtEMM_stackFreeTo(struct rtEMMStackAllocator* alloc, void** ptr);
#endif // RTESTACKALLOCATOR_H_
