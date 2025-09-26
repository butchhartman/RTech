#include "rtELog/rtELog.h" 
#include "rtEW/rtenginewindow.h"

// TODO: See about getting tests set up
int main() {
        rtELog_init("RTechLog.log");

        if (!rtEW_init()) {
                rtELog_log("***ERROR: RTEW INITIALIZATION FAILED");
                return 1;
        }

        struct rtEngineWindow* window = rtEW_createWindow("RTech");

        if (window == nullptr) {
                rtELog_log("***ERROR: FAILED TO CREATE ENGINE WINDOW");
                return 1;
        }
        
        rtEW_showWindow(window);  

        rtELog_log("Beginning main loop");
        while(!rtEW_windowShouldClose(window)) {

        }

        rtELog_log("Cleaning up resources");
        rtEW_cleanupWindow(window);
        rtELog_cleanup();

        return 0;
}
