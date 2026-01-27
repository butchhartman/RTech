#include "rtEErrorCodes/rtEErrorCodes.h"
#include "rtELog/rtELog.h" 
#include "rtEMath/rtEMath.h"
#include "rtEW/rtenginewindow.h"
#include "rtERenderer/rtERenderer.h"
#include <string.h>
#define _USE_MATH_DEFINES
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
vec3 cameraLookDirection;
vec3 up = {0.0, 1.0, 0.0};
mat4 camera;
mat4 proj;

float pitch = 0;
float yaw = -90;
float mouseLastX = 0;
float mouseLastY = 0;

bool forward = false;
bool backward = false;
bool right = false;
bool left = false;

static inline float toRadians(float deg) {
        return (2 * M_PI * deg)/360.0;
}

static void handleInput(struct inputEvent event) {
        switch (event.inputType) {
                case (RTEW_INPUT_TYPE_KEYBOARD):
                        if (event.keycode == RTEW_KEYCODE_W && event.keystate == RTEW_KEY_DOWN ) {
                                forward = true;
                        } else if (event.keycode == RTEW_KEYCODE_A && event.keystate == RTEW_KEY_DOWN ) {
                                left = true;
                        } else if (event.keycode == RTEW_KEYCODE_S && event.keystate == RTEW_KEY_DOWN ) {
                                backward = true;
                        } else if (event.keycode == RTEW_KEYCODE_D && event.keystate == RTEW_KEY_DOWN ) {
                                right = true;
                        }

                        if (event.keycode == RTEW_KEYCODE_W && event.keystate == RTEW_KEY_UP) {
                                forward = false;
                        } else if (event.keycode == RTEW_KEYCODE_A && event.keystate == RTEW_KEY_UP) {
                                left = false;
                        } else if (event.keycode == RTEW_KEYCODE_S && event.keystate == RTEW_KEY_UP) {
                                backward = false;
                        } else if (event.keycode == RTEW_KEYCODE_D && event.keystate == RTEW_KEY_UP) {
                                right = false;
                        }


                        break;

                case (RTEW_INPUT_TYPE_MOUSE):
                        // the offset is calculated in the window proc... not sure how happy I am about that. Consider changing to raw input api
                        float xOffset = event.mouseXPos ;//- mouseLastX;
                        float yOffset = /**mouseLastY - */event.mouseYPos; // since y is down we do this

                        yaw += xOffset * 0.25;
                        pitch += yOffset * 0.25;

                        mouseLastX = event.mouseXPos;
                        mouseLastY = event.mouseYPos;

                        vec3 tmpcameraLookDirection = {
                                cos(toRadians(yaw)) * cos(toRadians(pitch)), 
                                sin(toRadians(pitch)),
                                sin(toRadians(yaw)) * cos(toRadians(pitch))
                        };

                        rtEMath_vec3Normalize(tmpcameraLookDirection, cameraLookDirection);

                        break;
                default:
                        return;
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
        if (rtER_initializeRenderer(&renderer, window) != rtEErrorCode_SUCCESS) {
                rtELog_logError("Failed to initialize renderer");
                return EXIT_FAILURE;
        }


        rtEW_showWindow(window);  
        rter_vbo_t vbo = nullptr;
        (void)vbo;
        rtER_createVertexBuffer(renderer, &vbo);
        rtER_bufferVertexData(renderer, vbo, vertices, sizeof(struct vertex) * 6);
        rtER_bindVertexBuffer(renderer, vbo);

        rter_ubo_t ubo = nullptr;
        (void)ubo;
        rtER_createUniformBuffer(renderer, &ubo);

        rtELog_log("Beginning main loop");

        rtEW_setInputCallback(window, handleInput);

        clock_t start = clock();

        while(!rtEW_windowShouldClose(window)) {
                double elapsedTime = ((clock() - start) / (double)CLOCKS_PER_SEC);
                start = clock();

                if (forward) {
                        vec3 move;
                        memcpy(move, cameraLookDirection, sizeof(float) * 3); 
                        rtEMath_vec3MultScalar(move, elapsedTime, move);
                        rtEMath_vec3Add(cameraPos, move, cameraPos);
                }

                if (backward) {
                        vec3 move;
                        memcpy(move, cameraLookDirection, sizeof(float) * 3); 
                        rtEMath_vec3MultScalar(move, -elapsedTime, move);
                        rtEMath_vec3Add(cameraPos, move, cameraPos);
                }

                if (right) {
                        vec3 move;
                        rtEMath_vec3Cross(up, cameraLookDirection, move);
                        rtEMath_vec3MultScalar(move, -elapsedTime, move);
                        rtEMath_vec3Add(cameraPos, move, cameraPos);
                }
                if (left) {
                        vec3 move;
                        rtEMath_vec3Cross(up, cameraLookDirection, move);
                        rtEMath_vec3MultScalar(move, elapsedTime, move);
                        rtEMath_vec3Add(cameraPos, move, cameraPos);
                }


                rtEMath_mat4CreateModel(modelPos, model);

                //vec3 look = {0.0, 0.0, -1.0};
                //rtEMath_vec3Add(cameraPos, look, cameraTargetPos);
                rtEMath_vec3Add(cameraPos, cameraLookDirection, cameraTargetPos);

                rtEMath_mat4CreateLookAt(cameraPos, cameraTargetPos, up, camera);

                rtEMath_mat4CreatePerspectiveProjection(1.5707, .01, 100, 16.0/9.0, proj);

                mat4 ubodata[3];
                memcpy(ubodata, model, sizeof(float) * 16);
                memcpy(ubodata + 1, camera, sizeof(float) * 16);
                memcpy(ubodata + 2, proj, sizeof(float) * 16);

                rtER_bufferUniformData(renderer, ubo, ubodata, sizeof(float) * 16 * 3);

                rtER_drawFrame(renderer);

        }

        rtELog_log("Cleaning up resources");
        rtER_cleanupRenderer(&renderer);
        rtEW_cleanupWindow(&window);
        rtEW_cleanup();
        rtELog_cleanup();
        return 0;
}
