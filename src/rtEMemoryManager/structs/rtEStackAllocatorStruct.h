#ifndef RTESTACKALLOCATORSTRUCT_H_
#define RTESTACKALLOCATORSTRUCT_H_

#include <stddef.h>
#include <stdint.h>
struct rtEMMStackAllocator {
        unsigned char* buff;
        uint64_t buffSize;
        unsigned char* top;
        struct rtEMemoryManager* manager;
};


#endif // RTESTACKALLOCATORSTRUCT_H_ 
