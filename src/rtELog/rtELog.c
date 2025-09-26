#include <math.h>
#include <rtELog/rtELog.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

FILE* mainLogFile;
clock_t startingTime;

void rtELog_init(const char* logFileName) {
        errno_t failed = fopen_s(&mainLogFile, logFileName, "w");

        if (failed) {
                // TODO: Actually do something here
                return;
        }

        time_t now = time(NULL);
        struct tm t = {};
        localtime_s(&t, &now);
        fprintf_s(mainLogFile, "RTech - <version>\nCompiled on %s %s\n", __DATE__, __TIME__);
        
        fprintf_s(mainLogFile, "File generated on [%02d/%02d/%d - %02d:%02d:%02d]\n\n", t.tm_mon + 1, t.tm_mday, t.tm_year, t.tm_hour, t.tm_min, t.tm_sec);
        fflush(mainLogFile);
        startingTime = clock();
}


void rtELog_log(const char* message) {
        double currentTime = (clock() - startingTime)  / (double)CLOCKS_PER_SEC;
        int seconds = (int)(currentTime) % 60;
        int minutes = (int)(currentTime/60.0) % 60;
        int hours = (int)(currentTime/120.0);
        // +2 for null terminator & newline
        int timeStringSize = snprintf(NULL, 0, "[%02d:%02d:%02d] -- ", hours, minutes, seconds) + strlen(message) + 2;

        char* messageBuffer = malloc(sizeof(char) * timeStringSize);
        memset(messageBuffer, 0, sizeof(char) * timeStringSize);
        snprintf(messageBuffer, timeStringSize, "[%02d:%02d:%02d] -- ", hours, minutes, seconds); 
        strcat_s(messageBuffer, timeStringSize, message);
        strcat_s(messageBuffer, timeStringSize, "\n");

        fprintf_s(mainLogFile, messageBuffer);
        fflush(mainLogFile);

        free(messageBuffer);
}

void rtELog_cleanup() {
        fclose(mainLogFile);
}
