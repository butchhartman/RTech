#ifndef RTENGINEWINDOW_H_
#define RTENGINEWINDOW_H_

struct rtEngineWindow;

struct rtEngineWindow* rtEW_createWindow(const char* windowTitle);

void rtEW_cleanupWindow(struct rtEngineWindow* window);

#endif //RTENGINEWINDOW_H_ 
