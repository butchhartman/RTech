#ifndef RTESTACKALLOCATORSTRUCT_H_
#define RTESTACKALLOCATORSTRUCT_H_

struct rtEMMStackAllocator {
        unsigned char* buff;
        size_t buffSize;
        unsigned char* top;
        struct rtEMemoryManager* manager;
};


#endif // RTESTACKALLOCATORSTRUCT_H_ 
