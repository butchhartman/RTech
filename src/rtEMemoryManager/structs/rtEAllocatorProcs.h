#ifndef RTEALLOCATORPROCS_H_
#define RTEALLOCATORPROCS_H_

/** @file
* 
*/

typedef void*(*pfnrtEA_malloc)(size_t size, void* usr);
typedef void(*pfnrtEA_free)(void** ptr, void* usr);

void* rtEA_mallocDefault(size_t size, void* usr);

void rtEA_freeDefault(void** ptr, void* usr);

/**
* Struct containing memory allocation and freeing procs to be used
* in RTech modules
*/
struct rtEAllocatorProcs {
        pfnrtEA_malloc rtEA_malloc;
        pfnrtEA_free rtEA_free;
        void* usr;
};

#endif // RTEALLOCATORPROCS_H_
