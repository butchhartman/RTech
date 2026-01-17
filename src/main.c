#include "rtEErrorCodes/rtEErrorCodes.h"
#include "rtELog/rtELog.h" 
#include "rtEW/rtenginewindow.h"
#include "rtERenderer/rtERenderer.h"
#include <math.h>
#include <stdlib.h>
#include <time.h>


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

        clock_t start = clock();

        while(!rtEW_windowShouldClose(window)) {
                double elapsedTime = (clock() - start) / (double)CLOCKS_PER_SEC;

                vec3 modelPos = {0.0, 0.0, -1.0};
                mat4 model;
                rtEMath_mat4CreateModel(modelPos, model);

                vec3 cameraPos = {sin(elapsedTime), tan(elapsedTime), cos(elapsedTime) + 1};
                vec3 cameraTargetPos = {0.0, 0.0, -1.0};
                vec3 up = {0.0, 1.0, 0.0};
                mat4 camera;
                rtEMath_mat4CreateLookAt(cameraPos, cameraTargetPos, up, camera);

                mat4 proj;
                rtEMath_mat4CreatePerspectiveProjection(1.5707, .01, 100, 16.0/9.0, proj);

                rtER_bufferUniformData(renderer, 192, model, camera, proj);

                rtER_drawFrame(renderer);
        }

        rtELog_log("Cleaning up resources");
        rtER_cleanupRenderer(&renderer);
        rtEW_cleanupWindow(&window);
        rtEW_cleanup();
        rtELog_cleanup();
        return 0;
}
