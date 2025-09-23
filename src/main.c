#include <stdio.h>
#include "windowing/rtenginewindow.h"

// TODO: See about getting tests set up
int main() {

        struct rtEngineWindow* window = rtEW_createWindow("s");

        printf("Hello, World!");

        rtEW_cleanupWindow(window);

        return 0;
}
