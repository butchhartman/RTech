#include "rtEErrorCodes/rtEErrorCodes.h"
#include "rtELog/rtELog.h" 
#include "rtEW/rtenginewindow.h"
#include <stdlib.h>

// TODO: See about getting tests set up
int main() {
        rtELog_init("RTechLog");

        if (rtEW_init() != rtEErrorCode_SUCCESS) {
                rtELog_logError("rtEW Faild to initialize");
                return EXIT_FAILURE;
        }

        struct rtEngineWindow* window;
        if (rtEW_createWindow(&window, "RTech") != rtEErrorCode_SUCCESS) {
                rtELog_logError("Failed to create rtEWindow");
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
