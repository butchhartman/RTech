#ifndef RTENGINEWINDOW_H_
#define RTENGINEWINDOW_H_

struct rtEngineWindow;

bool rtEW_init();

struct rtEngineWindow* rtEW_createWindow(const char* windowTitle);

// NOTE: Using showWindow/hideWindow does NOT affect a window's should close member. The only time a window 'should close' is when the msg loop is terminated or destroy window is directly called.

void rtEW_showWindow(struct rtEngineWindow* window);

void rtEW_hideWindow(struct rtEngineWindow* window);

void rtEW_cleanupWindow(struct rtEngineWindow* window);

bool rtEW_windowShouldClose(const struct rtEngineWindow* window);

// TODO: Implement the actual window logic
#endif //RTENGINEWINDOW_H_ 
