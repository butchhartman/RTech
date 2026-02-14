#include <stdlib.h>
#include <time.h>

#include "game/gameClasses/camera/camera.h"
#include "game/renderObjects/chunk.h"
#include "rtEErrorCodes/rtEErrorCodes.h"
#include "rtELog/rtELog.h" 
#include "rtEMath/rtEMath.h"
#include "rtEW/rtenginewindow.h"
#include "rtERenderer/rtERenderer.h"
#include "game/renderObjects/vertex.h"

struct rtGame_camera* gameCamera;

static void handleInput(struct inputEvent event) {
        rtGame_cameraHandleInput(gameCamera, event);
}

/*
 * The main entry point
 *
 * Main's control flow is as follows:
 *      - Do all necessary things for engine initialization (window, renderer, etc)
 *      - Once engine is initialized, run the game initialization routine
 *      - Then, the engine will run the game update routine in its main loop
 *      - Once main loop exited, call game cleanup rountine
 *      - Cleanup engine
 */
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
        
        vec3 cameraPosition = {0, 0, 0};
        vec3 cameraTargetPos = {0, 0, -1};
        vec3 up = {0, 1, 0};

        gameCamera = rtGame_createCamera(
                                cameraPosition,
                                cameraTargetPos,
                                up,
                                toRadians(90),
                                16.0/9.0,
                                0.1,
                                100,
                                0.25
                        );

        struct chunk* myChunk = rtGame_createChunk();

        rtGame_chunkCreateMesh(myChunk);

        rter_vbo_t vbo = nullptr;
        rtER_createVertexBuffer(renderer, &vbo);
        rtER_bufferVertexData(renderer, vbo, (void*)rtGame_chunkGetMeshPtr(myChunk), sizeof(struct vertex) * 36 * rtGame_chunkGetMeshSize(myChunk));
        rtER_bindVertexBuffer(renderer, vbo);

        rter_ubo_t ubo = nullptr;
        rtER_createUniformBuffer(renderer, &ubo);

        rtEW_setInputCallback(window, handleInput);

        clock_t start = clock();

        rtELog_log("Beginning main loop");

        rtEW_showWindow(window);  

        while(!rtEW_windowShouldClose(window)) {

                double elapsedTime = ((clock() - start) / (double)CLOCKS_PER_SEC);
                start = clock();

                rtGame_cameraUpdate(gameCamera, elapsedTime);

                rtER_bufferUniformData(renderer, ubo, rtGame_getCameraData(gameCamera), RTEMATH_MAT4_SIZE * 3);

                rtER_drawFrame(renderer);

        }

        rtELog_log("Cleaning up resources");
        rtER_cleanupRenderer(&renderer);
        rtEW_cleanupWindow(&window);
        rtEW_cleanup();
        rtELog_cleanup();
        return EXIT_SUCCESS;
}
