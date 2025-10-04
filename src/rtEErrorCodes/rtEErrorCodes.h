#ifndef RTEERRORCODE_H_
#define RTEERRORCODE_H_
/**
* file
*/

/** 
* The error handling design philosphy:
*       Distinct modules of code (rtEW, rtELog, etc) return error codes as a return value. From there, it is up to the user of that
        module's functions to handle the error or pass it along themselves.
*/

/**
* @todo
* See if it is possible to find a way to separate the usage of the error codes from this header so that error codes can be added without blowing up every single header
*/

enum rtEErrorCode {
        rtEErrorCode_SUCCESS,
        rtEErrorCode_MEMORY_ALLOC_FAILURE,
        rtEErrorCode_PASSED_NULL_PTR,
};

#endif // RTEERRORCODE_H_
