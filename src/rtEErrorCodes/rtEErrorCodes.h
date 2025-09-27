#ifndef RTEERRORCODE_H_
#define RTEERRORCODE_H_

/* The error handling design philosphy:
*       Distinct modules of code (rtEW, rtELog, etc) return error codes as a return value. From there, it is up to the user of that
        module's functions to handle the error or pass it along themselves.
*/

enum rtEErrorCode {
        rtEErrorCode_SUCCESS,
        rtEErrorCode_MEMORY_ALLOC_FAILURE,
        rtEErrorCode_PASSED_NULL_PTR,
};

#endif // RTEERRORCODE_H_
