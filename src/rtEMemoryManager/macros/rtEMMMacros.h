#ifndef RTEMMMACROS_H_
#define RTEMMMACROS_H_

#define IN_BAND_HEADER_SIZE sizeof(uint64_t)

#define BUFFER_SIZE_BIT 0x7FFFFFFFFFFFFFFF

#define BUFFER_IS_OCCUPIED_BIT 0x8000000000000000 

#define GET_BUFFER_SIZE(buffer) \
        (*(uint64_t*)((buffer)) & BUFFER_SIZE_BIT)

#define GET_BUFFER_IS_OCCUPIED(buffer) \
        (*(uint64_t*)(buffer) & BUFFER_IS_OCCUPIED_BIT)

#endif // RTEMMMACROS_H_
