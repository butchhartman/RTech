#include "rtELog/rtELog.h" 
#include "rtEW/rtenginewindow.h"
#include <stdlib.h>

// TODO: See about getting tests set up
int main() {
        rtELog_init("RTechLog.log");

        if (!rtEW_init()) {
                rtELog_logError("rtEW Faild to initialize");
                return EXIT_FAILURE;
        }

        struct rtEngineWindow* window = rtEW_createWindow("RTech");

        if (window == nullptr) {
                rtELog_logError("Failed to create engine window");
                return EXIT_FAILURE;
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
