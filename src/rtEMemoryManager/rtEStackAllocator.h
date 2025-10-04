#ifndef RTESTACKALLOCATOR_H_ 
#define RTESTACKALLOCATOR_H_
#include <rtEErrorCodes/rtEErrorCodes.h>
#include <stdint.h>

/**
* @file
*/

// forward dec
struct rtEMemoryManager;

struct rtEMMStackAllocator;

/**
* Allocates a stack allocator using the memory contained within a @ref rtEMemoryManager
*
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

/**
* Assigns dest a pointer to a free memory block of size bytes in alloc
*
* @param alloc - A @ref rtEMMStackAllocator too allocate from
* @param size - The size in bytes to allocate to dest
* @param dest - The address of the pointer to recieve the memory block
*/
enum rtEErrorCode rtEMM_stackMalloc(struct rtEMMStackAllocator* alloc, uint32_t size, void** dest);
/**
* Moves the stack pointer to ptr, effectively de-allocating all pointers up to and including ptr
*
* @warning 
* Passing a pointer not allocated from alloc is undefined behavior. 
* Attempting to use a freed pointer is undefined behavior. One must keep track of their allocation
* order.
*
* @param alloc - The rtEMMStackAllocator that ptr belongs to
* @param ptr - The address of the pointer to free to. ptr is additionally set to nullptr
*/
enum rtEErrorCode rtEMM_stackFreeTo(struct rtEMMStackAllocator* alloc, void** ptr);
#endif // RTESTACKALLOCATOR_H_
