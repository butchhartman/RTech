#include "rtEErrorCodes/rtEErrorCodes.h"
#include <rtEMemoryManager/structs/rtEStackAllocatorStruct.h>
#include <rtEMemoryManager/procs/rtEMemoryManagerHelperProcs.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

enum rtEErrorCode rtEMM_allocateStackAllocator(struct rtEMemoryManager* parent, struct rtEMMStackAllocator** child, uint64_t buffSize) {
        // The size of a SA is implicity limited to 2^63-1 because of the same limit imposed on the memory manager
        uint64_t buffActualSize = buffSize + sizeof(struct rtEMMStackAllocator); //+ IN_BAND_HEADER_SIZE;
        // find empty block
        unsigned char* block = nullptr;

//        printf("requested block size: %llu, STACKALLOCSIZE: %llu, needed block size: %llu\n", buffSize, (uint64_t)sizeof(struct rtEMMStackAllocator), buffActualSize);
        enum rtEErrorCode err = rtEMM_findBlock(parent, &block, buffActualSize);
        if (err != rtEErrorCode_SUCCESS) {
                *child = nullptr;
                return err;
        }


        *child = (struct rtEMMStackAllocator*)block;
        block += sizeof(struct rtEMMStackAllocator);
        (*child)->buff = block;
        (*child)->buffSize = buffSize;
        (*child)->top = block;
        (*child)->manager = parent;

        return rtEErrorCode_SUCCESS;
}

enum rtEErrorCode rtEMM_cleanupStackAllocator(struct rtEMMStackAllocator** alloc) {
        if (*alloc == nullptr) {
                return rtEErrorCode_PASSED_NULL_PTR;
        }

        struct rtEMMStackAllocator* stackAlloc = *alloc;

        *(stackAlloc->buff - 1 - sizeof(struct rtEMMStackAllocator)) = 0x00;
        memset(stackAlloc->buff - sizeof(struct rtEMMStackAllocator), 0, stackAlloc->buffSize + sizeof(struct rtEMMStackAllocator));
        stackAlloc->buff = nullptr;
        stackAlloc->top = nullptr;
        stackAlloc->manager = nullptr;
        stackAlloc->buffSize = 0;

        *alloc = nullptr;

        return rtEErrorCode_SUCCESS;
}

// I'll be honest, I didn't understand alignment until today. Pretty much all this code is from the book.
// Is there ever a time when I'd want to allocate potentiall unaligned memory? probbles not
enum rtEErrorCode rtEMM_stackMalloc(struct rtEMMStackAllocator* alloc, uint32_t size, void** dest) {
        size_t actualSize = size + alignof(max_align_t);
        if (actualSize > (alloc->buffSize - (alloc->buff - alloc->top))) {
                return rtEErrorCode_MEMORY_ALLOC_FAILURE;
        }

        unsigned char* mem = alloc->top;
        unsigned char* alignedMem;
        size_t mask = alignof(max_align_t) - 1;
        alignedMem = (unsigned char*)(((uintptr_t)mem + mask) & ~mask);

        if (alignedMem == mem) {
                alignedMem = alignedMem + alignof(max_align_t);
        }

        ptrdiff_t shift = alignedMem - mem;
        alignedMem[-1] = (unsigned char)(shift & 0xFF);

 //       printf("stack alloc of: %llu\n", actualSize);
        *dest = alignedMem;
        alloc->top += actualSize;
        return rtEErrorCode_SUCCESS;
}

enum rtEErrorCode rtEMM_stackFreeTo(struct rtEMMStackAllocator* alloc, void** ptr) {
//        printf("stack free\n");

        unsigned char* alignedMem = *ptr;

        ptrdiff_t shift = alignedMem[-1];
        if (shift == 0) {
                shift = 256;
        }

        unsigned char* rawMem = alignedMem - shift;

        alloc->top = rawMem;
        *ptr = nullptr;

        return rtEErrorCode_SUCCESS;
}

enum rtEErrorCode rtEMM_stackMallocAligned(struct rtEMMStackAllocator* alloc, uint32_t size, void** dest, size_t alignment) {
        size_t actualSize = size + alignment;
        unsigned char* mem;
        rtEMM_stackMalloc(alloc, actualSize, (void**)(&mem));
        *dest = (void*)((uintptr_t)mem+(alignment-1) & ~(alignment-1));

        if (mem == *dest) {
                *dest = (unsigned char*)(*dest) + alignment;
        }

        ptrdiff_t shift = (unsigned char*)(*dest) - mem;

        ((unsigned char*)(*dest))[-1] = shift;

        return rtEErrorCode_SUCCESS;
}
enum rtEErrorCode rtEMM_stackFreeToAligned(struct rtEMMStackAllocator* alloc, void** ptr, size_t alignment) {
}

enum rtEErrorCode rtEMM_dumpBuffer(struct rtEMMStackAllocator* alloc) {
        return rtEMM_dumpBlock("stackdump.dmp", alloc->buff, alloc->buffSize);
}
