#ifndef RTENGINEWINDOW_H_
#define RTENGINEWINDOW_H_
#include <rtEErrorCodes/rtEErrorCodes.h>

struct rtEngineWindow;

enum rtEErrorCode rtEW_init();

enum rtEErrorCode rtEW_cleanup();

enum rtEErrorCode rtEW_createWindow(struct rtEngineWindow** window, const char* windowTitle);

// NOTE: Using showWindow/hideWindow does NOT affect a window's should close member. The only time a window 'should close' is when the msg loop is terminated or destroy window is directly called.

void rtEW_showWindow(struct rtEngineWindow* window);

void rtEW_hideWindow(struct rtEngineWindow* window);

enum rtEErrorCode rtEW_cleanupWindow(struct rtEngineWindow** window);

bool rtEW_windowShouldClose(const struct rtEngineWindow* window);

// TODO: Implement the actual window logic
#endif //RTENGINEWINDOW_H_ 
