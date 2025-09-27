#ifndef RTELOG_H_
#define RTELOG_H_ 

#define rtELog_logError(msg, ...) rtELog_log("***ERROR:  " msg, __VA_ARGS__)
#define rtELog_logWarning(msg, ...) rtELog_log("**WARNING:  " msg, __VA_ARGS__)
#define rtELog_logInfo(msg, ...) rtELog_log("*INFO:  " msg, __VA_ARGS__)

void rtELog_init(const char* logFileName);

void rtELog_log(const char* message, ...);

void rtELog_cleanup();

#endif // RTELOG_H_
