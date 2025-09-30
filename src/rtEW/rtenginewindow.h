#ifndef RTENGINEWINDOW_H_
#define RTENGINEWINDOW_H_
#include <rtEErrorCodes/rtEErrorCodes.h>

/**
* @file 
* The API for the windowing module
*/

/**
* An opaque handle representing a window
*/
struct rtEngineWindow;

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

// NOTE: Using showWindow/hideWindow does NOT affect a window's should close member. The only time a window 'should close' is when the msg loop is terminated or destroy window is directly called.

/**
* Sets the internal state of a window to be visible to the user.
*
* @note
* A window's internal message processing occurs on a separate thread. It will not block the main thread.
*
* @note
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
* @todo
* There is currently no way to modify this flag without user input. This needs to be changed.
*
* @param window - The window to query
*/
bool rtEW_windowShouldClose(const struct rtEngineWindow* window);

// TODO: Implement the actual window logic
#endif //RTENGINEWINDOW_H_ 
