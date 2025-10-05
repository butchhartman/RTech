#include "rtEMemoryManager/structs/rtEAllocatorProcs.h"
#include <stdlib.h>

void* rtEA_mallocDefault(size_t size, void* usr) {
        (void)usr;
        return malloc(size);
}

void rtEA_freeDefault(void** ptr, void* usr) {
        (void)usr;
        free(*ptr);
}
