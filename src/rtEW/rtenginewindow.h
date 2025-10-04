#ifndef RTENGINEWINDOW_H_
#define RTENGINEWINDOW_H_
#include <rtEErrorCodes/rtEErrorCodes.h>

/**
* @file 
* 
*/

typedef void*(*pfnrtEW_malloc)(size_t size, void* usr);
typedef void(*pfnrtEW_free)(void** ptr, void* usr);

struct rtEW_Allocator {
        pfnrtEW_malloc rtEW_malloc;
        pfnrtEW_free rtEW_free;
        void* usr;
};

/**
* An opaque handle representing a window
*/
struct rtEngineWindow;

/**
* Sets the global allocator rtEW will use for all its (non-OS managed) memory allocations
*
* @warning
* This must be set before calling @ref rtEW_init and should not be set after.
*
* @param alloc - a @ref rtEW_Allocator populated with the relevant functions
*/
enum rtEErrorCode rtEW_setAllocator(struct rtEW_Allocator alloc);

/**
* Initializes internal systems needed before any rtEW functions can run.
*
* @warning 
* The behavior of rtEW functions before rtEW_init has been run is undefined.
*/
enum rtEErrorCode rtEW_init();

/**
* Cleanups resources associated with rtEW internal systems.
*
* @note
* I have no idea what happens if one runs this while a window is still active. It is not recommended to try to find out.
*/
enum rtEErrorCode rtEW_cleanup();

/**
* Allocates resources for the passed window.
*
* @param window - The address of the rtEngineWindow* to allocate
* @param windowTitle - The title the window will display
*/
enum rtEErrorCode rtEW_createWindow(struct rtEngineWindow** window, const char* windowTitle);

/**
* Sets the internal state of a window to be visible to the user.
*
* @note
* A window's internal message processing occurs on a separate thread. A visible window 
* will not block the main thread.
* One must have an active loop or some other method of blocking the main thread to actually see the window.
*
* @param window - The window to show
*/
void rtEW_showWindow(struct rtEngineWindow* window);
/**
* Sets the internal state of a window to be hidden from the user.
*
* @note
* This does not signal the window to close. It simply makes the window not visible.
*
* @param window - The window to hide
*/

void rtEW_hideWindow(struct rtEngineWindow* window);

/**
* Frees resources associated with the passed window
*
* @param window - The address of the rtEngineWindow* to cleanup
*/
enum rtEErrorCode rtEW_cleanupWindow(struct rtEngineWindow** window);

/**
* Returns the internal flag indicating if a window should be terminated.
*
* @param window - The window to query
*/
bool rtEW_windowShouldClose(const struct rtEngineWindow* window);

/**
* Sets the internal flag indicating the window should close to true. The window is unrecoverable after calling this function.
*
* @param window - The window to close
*/
void rtEW_setWindowShouldClose(struct rtEngineWindow* window);

#endif //RTENGINEWINDOW_H_ 
