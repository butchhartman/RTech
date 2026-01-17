#include "rtEErrorCodes/rtEErrorCodes.h"
#include "rtELog/rtELog.h" 
#include "rtEMath/rtEMath.h"
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
vec3 modelPos = {0.0, 0.0, -1.0};
mat4 model;

vec3 cameraPos = {0, 0, 0};
vec3 cameraTargetPos = {0.0, 0.0, -1.0};
vec3 up = {0.0, 1.0, 0.0};
mat4 camera;
mat4 proj;


static void handleInput(struct inputEvent event) {
        if (event.keycode == RTEW_KEYCODE_W) {
                rtELog_debug_logInfo("PRESSED W!");
                vec3 add = {0.0, 0.0, -1.0};
                rtEMath_vec3Add(cameraPos, add, cameraPos);
        } else if (event.keycode == RTEW_KEYCODE_A) {
                rtELog_debug_logInfo("PRESSED A!");
                vec3 add = {-1.0, 0.0, 0.0};
                rtEMath_vec3Add(cameraPos, add, cameraPos);
        } else if (event.keycode == RTEW_KEYCODE_S) {
                rtELog_debug_logInfo("PRESSED S!");
                vec3 add = {0.0, 0.0, 1.0};
                rtEMath_vec3Add(cameraPos, add, cameraPos);
        } else if (event.keycode == RTEW_KEYCODE_D) {
                rtELog_debug_logInfo("PRESSED D!");
                vec3 add = {1.0, 0.0, 0.0};
                rtEMath_vec3Add(cameraPos, add, cameraPos);
        }
}

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

        rtEW_setInputCallback(window, handleInput);

       // clock_t start = clock();

        while(!rtEW_windowShouldClose(window)) {
                //double elapsedTime = (clock() - start) / (double)CLOCKS_PER_SEC;


                rtEMath_mat4CreateModel(modelPos, model);

                vec3 look = {0.0, 0.0, -1.0};
                rtEMath_vec3Add(cameraPos, look, cameraTargetPos);

                rtEMath_mat4CreateLookAt(cameraPos, cameraTargetPos, up, camera);

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
