#ifndef RTELOG_H_
#define RTELOG_H_ 

void rtELog_init(const char* logFileName);

void rtELog_log(const char* message);

void rtELog_cleanup();

#endif // RTELOG_H_
