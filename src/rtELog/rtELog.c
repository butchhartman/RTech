#include <assert.h>
#include <math.h>
#include <rtELog/rtELog.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <vadefs.h>
#include "rtEErrorCodes/rtEErrorCodes.h"
#include "rtEMemoryManager/structs/rtEAllocatorProcs.h"

struct rtEAllocatorProcs RTELOG_GLOBAL_alloc = {
        .rtEA_malloc= rtEA_mallocDefault,
        .rtEA_free = rtEA_freeDefault,
        .usr = nullptr
};

FILE* mainLogFile;
clock_t startingTime;

// TODO: Determine if adding strict error checking is over the top or not

enum rtEErrorCode rtELog_setAllocator(struct rtEAllocatorProcs alloc) {
        RTELOG_GLOBAL_alloc = alloc;
        return rtEErrorCode_SUCCESS;
}

static double helper_getCurrentTime(int* seconds, int* minutes, int* hours) {
        double currentTime = (clock() - startingTime) / (double)CLOCKS_PER_SEC;        
        *seconds = (int)(currentTime) % 60;
        *minutes = (int)(currentTime/60.0) % 60;
        *hours = (int)(currentTime/120.0);
        return currentTime;
}

static char* helper_createLogMessage(const char* message, va_list args){
        int seconds, minutes, hours;
        helper_getCurrentTime(&seconds, &minutes, &hours);
        // +2 for null terminator & newline
        int formattedMessageSize = vsnprintf(NULL, 0, message, args) + 1;

        int timeStringSize = snprintf(NULL, 0, "[%02d:%02d:%02d] -- ", hours, minutes, seconds) + formattedMessageSize + 2;

        char* messageBuffer = RTELOG_GLOBAL_alloc.rtEA_malloc(sizeof(char) * timeStringSize, RTELOG_GLOBAL_alloc.usr);
        memset(messageBuffer, 0, sizeof(char) * timeStringSize);
        snprintf(messageBuffer, timeStringSize, "[%02d:%02d:%02d] -- ", hours, minutes, seconds); 
        strcat_s(messageBuffer, timeStringSize, message);
        strcat_s(messageBuffer, timeStringSize, "\n");

        return messageBuffer;
}

static char* appendToString(const char* a, const char* b) {
        size_t buffSize = strlen(a) + strlen(b) + 1;
        char* buff = RTELOG_GLOBAL_alloc.rtEA_malloc(sizeof(char) * buffSize, RTELOG_GLOBAL_alloc.usr);
        snprintf(buff, buffSize, "%s%s", a, b);
        return buff;
}

static char* createLogFileSuffix(struct tm t) {

        size_t buffSize = snprintf(
                nullptr, 
                0, 
                "_%02d%02d%04d_%02d%02d%02d.log", 
                t.tm_mon + 1, t.tm_mday, t.tm_year + 1900,
                t.tm_hour, t.tm_min, t.tm_sec 
        ) + 1;
        char* buff = RTELOG_GLOBAL_alloc.rtEA_malloc(sizeof(char) * buffSize, RTELOG_GLOBAL_alloc.usr);
        snprintf(
                buff, 
                buffSize, 
                "_%02d%02d%04d_%02d%02d%02d.log", 
                t.tm_mon + 1, t.tm_mday, t.tm_year + 1900,
                t.tm_hour, t.tm_min, t.tm_sec 
        );

        return buff;
}

enum rtEErrorCode rtELog_init(const char* logFileName) {
        time_t now = time(NULL);
        struct tm t = {};
        localtime_s(&t, &now);
        char* logFileSuffix = createLogFileSuffix(t);
        char* fullFileName = appendToString(logFileName, logFileSuffix);
        errno_t failed = fopen_s(&mainLogFile, fullFileName, "w");


        if (failed) {
                return rtEErrorCode_MEMORY_ALLOC_FAILURE;
        }

        fprintf_s(mainLogFile, "RTech - <version>\nCompiled on %s %s\n", __DATE__, __TIME__);
        
        fprintf_s(mainLogFile, "File generated on [%02d/%02d/%d - %02d:%02d:%02d]\n\n", t.tm_mon + 1, t.tm_mday, t.tm_year+1900, t.tm_hour, t.tm_min, t.tm_sec);
        fflush(mainLogFile);
        startingTime = clock();

        RTELOG_GLOBAL_alloc.rtEA_free((void**)&fullFileName, RTELOG_GLOBAL_alloc.usr);
        RTELOG_GLOBAL_alloc.rtEA_free((void**)&logFileSuffix, RTELOG_GLOBAL_alloc.usr);

        return rtEErrorCode_SUCCESS;
}


void rtELog_log(const char* message, ...) {
        if (mainLogFile == NULL) {
                return;
        }

        va_list args;
        va_start(args);
        char* messageBuffer = helper_createLogMessage(message, args) ;
        va_end(args);

        fprintf_s(mainLogFile, messageBuffer);
        fflush(mainLogFile);

        RTELOG_GLOBAL_alloc.rtEA_free((void**)&messageBuffer, RTELOG_GLOBAL_alloc.usr);
}

enum rtEErrorCode rtELog_cleanup() {
        int success = fclose(mainLogFile);
        mainLogFile = nullptr;
        if (success == EOF) {
                return rtEErrorCode_MEMORY_ALLOC_FAILURE;
        }
        return rtEErrorCode_SUCCESS;
}

