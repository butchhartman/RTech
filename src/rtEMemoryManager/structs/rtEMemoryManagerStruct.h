#ifndef RTEMEMORYMANAGERSTRUCT_H_
#define RTEMEMORYMANAGERSTRUCT_H_
#include <stdint.h>

struct rtEMemoryManager {
        unsigned char* buff;
        uint64_t buffSize;
};

#endif // RTEMEMORYMANAGERSTRUCT_H_
