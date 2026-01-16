#include "rtEErrorCodes/rtEErrorCodes.h"
#include "rtELog/rtELog.h" 
#include "rtEW/rtenginewindow.h"
#include "rtERenderer/rtERenderer.h"
#include <stdlib.h>


struct vertex {
        float x, y, z;
        float u, v;
        float r, g, b;
};

struct vertex vertices[6] = {
        {-1.0, 1.0, 0.0, 0.0, 0.0, 1.0, 1.0, 0.0},
        {0.0, 0.0, 0.0, 0.0, 0.0, 0.3, 0.5, 1.0},
        {-1.0, -1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.5}, 
        {1.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0},
        {1.0, -1.0, 0.0, 0.0, 0.0, 0.3, 0.5, 0.3}, 
        {0.0, 0.0, 0.0, 0.0, 0.0, 0.4, 0.7, 0.3},
};

// TODO: Model, view, proj matrices
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

        struct rtERenderer* renderer;
        if (rtER_initializeRenderer(&renderer, window, RENDERER_IMPL_ID_VULKAN) != rtEErrorCode_SUCCESS) {
                rtELog_logError("Failed to initialize renderer");
                return EXIT_FAILURE;
        }


        rtEW_showWindow(window);  

        rtER_bufferVertexData(renderer, vertices, sizeof(struct vertex), 6);
        rtELog_log("Beginning main loop");
        while(!rtEW_windowShouldClose(window)) {
                rtER_drawFrame(renderer);
        }

        rtELog_log("Cleaning up resources");
        rtER_cleanupRenderer(&renderer);
        rtEW_cleanupWindow(&window);
        rtEW_cleanup();
        rtELog_cleanup();
        return 0;
}
