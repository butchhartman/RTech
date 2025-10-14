#ifndef RTERENDERERVKMACROS_H_
#define RTERENDERERVKMACROS_H_

#define VK_ERROR_LOG_AND_RETURN(vkfunc, message, ...) \
do { \
        if ((vkfunc) < 0) {\
                rtELog_logError(message" VK_ERROR CODE: %d", vkfunc, __VA_ARGS__); \
                return (vkfunc); \
        } \
} while(0) \

#define NO_VK_FLAGS 0
#define ARRAY_SIZE(array) \
        (sizeof(array) / sizeof(array[0]))

#endif // RTERENDERERVKMACROS_H_
