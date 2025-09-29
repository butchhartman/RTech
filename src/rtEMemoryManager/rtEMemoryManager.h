#ifndef RTEMEMORYMANAGER_H_
#define RTEMEMORYMANAGER_H_ 
#include "rtEErrorCodes/rtEErrorCodes.h"
#include <stddef.h>
#include <stdint.h>

struct rtEMemoryManager;
struct rtEMMStackAllocator;

enum rtEErrorCode rtEMM_createMemoryManager(struct rtEMemoryManager** obj, uint32_t buffSize);
enum rtEErrorCode rtEMM_cleanupMemoryManager(struct rtEMemoryManager** obj);

enum rtEErrorCode rtEMM_allocateStackAllocator(struct rtEMemoryManager** parent, struct rtEMMStackAllocator** child, size_t buffSize);
enum rtEErrorCode rtEMM_cleanupStackAllocator(struct rtEMMStackAllocator** alloc);

enum rtEErrorCode rtEMM_stackMalloc(struct rtEMMStackAllocator* alloc, uint32_t size, void** dest);
enum rtEErrorCode rtEMM_stackFreeTo(struct rtEMMStackAllocator* alloc, void** ptr);

#endif // RETMEMORYMANAGER_H_
