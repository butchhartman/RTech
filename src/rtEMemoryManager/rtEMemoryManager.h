#ifndef RTEMEMORYMANAGER_H_
#define RTEMEMORYMANAGER_H_ 
#include "rtEErrorCodes/rtEErrorCodes.h"
#include <stddef.h>
#include <stdint.h>

// TODO: Expand documentation from these quick and dirty descriptions
// TODO: Fix looking at the implementation making me sad

struct rtEMemoryManager;
struct rtEMMStackAllocator;

// creates mem manager with an internal buffer of buffSize + sizeof(manager) bytes. The manager is stored in this buffer before the rest of the data.
enum rtEErrorCode rtEMM_createMemoryManager(struct rtEMemoryManager** obj, uint32_t buffSize);
enum rtEErrorCode rtEMM_cleanupMemoryManager(struct rtEMemoryManager** obj);

// uses the manager's internal buffer to allocate a stack allocator. NOTE: the stack allocator is guaranteed to be buffSize bytes large, but will consume at least 37 additional bytes for storing itself.
enum rtEErrorCode rtEMM_allocateStackAllocator(struct rtEMemoryManager** parent, struct rtEMMStackAllocator** child, size_t buffSize);
enum rtEErrorCode rtEMM_cleanupStackAllocator(struct rtEMMStackAllocator** alloc);

enum rtEErrorCode rtEMM_stackMalloc(struct rtEMMStackAllocator* alloc, uint32_t size, void** dest);
enum rtEErrorCode rtEMM_stackFreeTo(struct rtEMMStackAllocator* alloc, void** ptr);

#endif // RETMEMORYMANAGER_H_
