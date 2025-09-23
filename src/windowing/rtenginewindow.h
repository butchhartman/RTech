#ifndef RTENGINEWINDOW_H_
#define RTENGINEWINDOW_H_
struct rtEngineWindow;

struct rtEngineWindow* rtEW_createWindow(const char* windowTitle);

void rtEW_cleanupWindow(struct rtEngineWindow* window);

inline bool rtEW_windowIsActive(const struct rtEngineWindow* window);

// TODO: Implement the actual window logic


#endif //RTENGINEWINDOW_H_ 
