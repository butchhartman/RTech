#ifndef RTELOG_H_
#define RTELOG_H_ 

/** @file 
* 
*/


#ifndef NDEBUG
        /**
        * Identical to @ref rtELog_logError but resolves to nothing on release build
        */
        #define rtELog_debug_logError(msg, ...) rtELog_log("***DB_ERROR:  " msg, __VA_ARGS__)
        /**
        * Identical to @ref rtELog_logWarning but resolves to nothing on release build
        */
        #define rtELog_debug_logWarning(msg, ...) rtELog_log("**DB_WARNING:  " msg, __VA_ARGS__)
        /**
        * Identical to @ref rtELog_logInfo but resolves to nothing on release build
        */
        #define rtELog_debug_logInfo(msg, ...) rtELog_log("*DB_INFO:  " msg, __VA_ARGS__)
#else
        #define rtELog_debug_logError(msg, ...)
        #define rtELog_debug_logWarning(msg, ...)
        #define rtELog_debug_logInfo(msg, ...)
#endif

/**
* Prepends '***ERROR: ' to the passed message
*/
#define rtELog_logError(msg, ...) rtELog_log("***ERROR:  " msg, __VA_ARGS__)
/**
* Prepends '**WARNING: ' to the passed message
*/
#define rtELog_logWarning(msg, ...) rtELog_log("**WARNING:  " msg, __VA_ARGS__)
/**
* Prepends '*INFO: ' to the passed message
*/
#define rtELog_logInfo(msg, ...) rtELog_log("*INFO:  " msg, __VA_ARGS__)


/**
* Initializes the global state of the rtELogger. This must happen before any logging functions are called. All subsequent logging calls will
* be realized in the log file created by this function.
* 
* @param logFileName The name of the file to write the log to. The passed file name will be appended in the following format: 
*                    _DATE_TIME.log
*/
void rtELog_init(const char* logFileName);

/**
* Writes the passed message to the global log file initialized by rtELog_init.
*
* @note It is preferred to use the logging macros as opposed to this function directly.
*
* @param message A formatted string to write to the log
* @param ... Inputs matching the format specifiers in message
*/
void rtELog_log(const char* message, ...);

/**
* Closes the global log file initialized by rtELog_init. Should be done when the log file is no longer in use OR the program is terminating.
*/
void rtELog_cleanup();

#endif // RTELOG_H_
