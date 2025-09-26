#include "rtEW/rtenginewindow.h"

// TODO: See about getting tests set up
int main() {
        if (!rtEW_init()) {
                return 1;
        }

        struct rtEngineWindow* window = rtEW_createWindow("RTech");

        if (window == nullptr) {
                return 1;
        }
        
        rtEW_showWindow(window);

        while(!rtEW_windowShouldClose(window)) {

        }

        rtEW_cleanupWindow(window);

        return 0;
}
