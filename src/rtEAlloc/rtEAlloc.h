#ifndef RTEALLOC_H_
#define RTEALLOC_H_
#include <rtEErrorCodes/rtEErrorCodes.h>

struct rtEStackAllocator;


enum rtEErrorCode rtEAlloc_createStackAllocator(struct rtEStackAllocator** stackAllocator, size_t size); 

enum rtEErrorCode rtEAlloc_mallocPush(struct rtEStackAllocator* stackAllocator, size_t size, void** dest);

enum rtEErrorCode rtEAlloc_freeToBlock(struct rtEStackAllocator* stackAllocator, void* block);

enum rtEErrorCode rtEAlloc_cleanupStackAllocator(struct rtEStackAllocator** stackAllocator);


#endif // RTEALLOC_H_
