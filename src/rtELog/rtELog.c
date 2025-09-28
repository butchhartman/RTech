#include <assert.h>
#include <math.h>
#include <rtELog/rtELog.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <vadefs.h>

FILE* mainLogFile;
clock_t startingTime;

// TODO: Determine if adding strict error checking is over the top or not

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
        char* formattedMessage = malloc(sizeof(char) * formattedMessageSize);
        vsnprintf(formattedMessage, formattedMessageSize, message, args);

        int timeStringSize = snprintf(NULL, 0, "[%02d:%02d:%02d] -- ", hours, minutes, seconds) + strlen(formattedMessage) + 2;

        char* messageBuffer = malloc(sizeof(char) * timeStringSize);
        memset(messageBuffer, 0, sizeof(char) * timeStringSize);
        snprintf(messageBuffer, timeStringSize, "[%02d:%02d:%02d] -- ", hours, minutes, seconds); 
        strcat_s(messageBuffer, timeStringSize, formattedMessage);
        strcat_s(messageBuffer, timeStringSize, "\n");

        return messageBuffer;
}

static char* appendToString(const char* a, const char* b) {
        size_t buffSize = strlen(a) + strlen(b) + 1;
        char* buff = malloc(sizeof(char) * buffSize);
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
        char* buff = malloc(sizeof(char) * buffSize);
        snprintf(
                buff, 
                buffSize, 
                "_%02d%02d%04d_%02d%02d%02d.log", 
                t.tm_mon + 1, t.tm_mday, t.tm_year + 1900,
                t.tm_hour, t.tm_min, t.tm_sec 
        );

        return buff;
}

void rtELog_init(const char* logFileName) {
        time_t now = time(NULL);
        struct tm t = {};
        localtime_s(&t, &now);
        char* logFileSuffix = createLogFileSuffix(t);
        char* fullFileName = appendToString(logFileName, logFileSuffix);
        errno_t failed = fopen_s(&mainLogFile, fullFileName, "w");


        if (failed) {
                // TODO: Actually do something here
                return;
        }

        fprintf_s(mainLogFile, "RTech - <version>\nCompiled on %s %s\n", __DATE__, __TIME__);
        
        fprintf_s(mainLogFile, "File generated on [%02d/%02d/%d - %02d:%02d:%02d]\n\n", t.tm_mon + 1, t.tm_mday, t.tm_year, t.tm_hour, t.tm_min, t.tm_sec);
        fflush(mainLogFile);
        startingTime = clock();

        free(logFileSuffix);
        free(fullFileName);
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

        free(messageBuffer);
}

void rtELog_cleanup() {
        fclose(mainLogFile);
}

