#ifndef RTEMMMACROS_H_
#define RTEMMMACROS_H_

#define IN_BAND_HEADER_SIZE sizeof(uint32_t) + sizeof(uint8_t)

#define GET_BUFFER_SIZE(buffer) \
        *(uint32_t*)((buffer)) \

#define GET_BUFFER_IS_OCCUPIED(buffer) \
        *(uint8_t*)((buffer + IN_BAND_HEADER_SIZE - 1))

#define GET_BUFFER_SIZE_PTR(buffer) \
        (uint32_t*)((buffer)) \

#define GET_BUFFER_IS_OCCUPIED_PTR(buffer) \
        (uint8_t*)((buffer + IN_BAND_HEADER_SIZE - 1))

#endif // RTEMMMACROS_H_
