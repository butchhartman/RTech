#include <stdlib.h>
#include <string.h>

#include "game/gameClasses/camera.h"
#include "rtEMath/rtEMath.h"

struct rtGame_camera {
        vec3 cameraPos;
        vec3 cameraLookDirection;
        vec3 up;
        mat4 camera;
        mat4 proj;

        float fov;
        float aspectRatio;
        float nearPlaneDistance;
        float farPlaneDistance;

        float sensitivity;

        float pitch;
        float yaw;
        float mouseLastX;
        float mouseLastY;

        bool forward;
        bool backward;
        bool right;
        bool left;

        mat4 cameraData[3];
};

struct rtGame_camera* rtGame_createCamera(
                vec3 position, 
                vec3 targetPos,
                vec3 up,
                float fov,
                float aspectRatio,
                float nearPlaneDistance,
                float farPlaneDistance,
                float sensitivity
                ) {

        struct rtGame_camera* camera = malloc(sizeof(struct rtGame_camera));
        
        memcpy(camera->cameraPos, position, sizeof(float) * 3);
        memcpy(camera->up, up, sizeof(float) * 3);
        rtEMath_vec3Sub(targetPos, position, camera->cameraLookDirection);
        rtEMath_vec3Normalize(camera->cameraLookDirection, camera->cameraLookDirection);

        camera->fov = fov;
        camera->aspectRatio = aspectRatio;
        camera->nearPlaneDistance = nearPlaneDistance;
        camera->farPlaneDistance = farPlaneDistance;

        camera->sensitivity = sensitivity;

        camera->pitch = 0;
        camera->yaw = -90;

        camera->mouseLastX = 0;
        camera->mouseLastY = 0;

        camera->forward = false;
        camera->backward = false;
        camera->right = false;
        camera->left = false;

        return camera;
}

void rtGame_cameraHandleInput(struct rtGame_camera* camera, const struct inputEvent event) {
        switch (event.inputType) {
                case (RTEW_INPUT_TYPE_KEYBOARD):
                        if (event.keycode == RTEW_KEYCODE_W && event.keystate == RTEW_KEY_DOWN ) {
                                camera->forward = true;
                        } else if (event.keycode == RTEW_KEYCODE_A && event.keystate == RTEW_KEY_DOWN ) {
                                camera->left = true;
                        } else if (event.keycode == RTEW_KEYCODE_S && event.keystate == RTEW_KEY_DOWN ) {
                                camera->backward = true;
                        } else if (event.keycode == RTEW_KEYCODE_D && event.keystate == RTEW_KEY_DOWN ) {
                                camera->right = true;
                        }

                        if (event.keycode == RTEW_KEYCODE_W && event.keystate == RTEW_KEY_UP) {
                                camera->forward = false;
                        } else if (event.keycode == RTEW_KEYCODE_A && event.keystate == RTEW_KEY_UP) {
                                camera->left = false;
                        } else if (event.keycode == RTEW_KEYCODE_S && event.keystate == RTEW_KEY_UP) {
                                camera->backward = false;
                        } else if (event.keycode == RTEW_KEYCODE_D && event.keystate == RTEW_KEY_UP) {
                                camera->right = false;
                        }

                        break;

                case (RTEW_INPUT_TYPE_MOUSE):
                        // the offset is calculated in the window proc... not sure how happy I am about that. Consider changing to raw input api
                        float xOffset = event.mouseXPos ;//- mouseLastX;
                        float yOffset = /**mouseLastY - */event.mouseYPos; // since y is down we do this

                        camera->yaw += xOffset * camera->sensitivity;
                        camera->pitch += yOffset * camera->sensitivity;

                        if (camera->pitch >= 89) {
                                camera->pitch = 89;
                        } else if (camera->pitch < -89) {
                                camera->pitch = -89;
                        }


                        camera->mouseLastX = event.mouseXPos;
                        camera->mouseLastY = event.mouseYPos;

                        vec3 tmpcameraLookDirection = {
                                cos(toRadians(camera->yaw)) * cos(toRadians(camera->pitch)), 
                                sin(toRadians(camera->pitch)),
                                sin(toRadians(camera->yaw)) * cos(toRadians(camera->pitch))
                        };

                        rtEMath_vec3Normalize(tmpcameraLookDirection, camera->cameraLookDirection);

                        break;
                default:
                        return;
        }
}

void rtGame_cameraUpdate(struct rtGame_camera* camera, double delta) {
        if (camera->forward) {
                vec3 move;
                memcpy(move, camera->cameraLookDirection, sizeof(float) * 3); 
                rtEMath_vec3MultScalar(move, delta, move);
                rtEMath_vec3Add(camera->cameraPos, move, camera->cameraPos);
        }

        if (camera->backward) {
                vec3 move;
                memcpy(move, camera->cameraLookDirection, sizeof(float) * 3); 
                // delta is negative here to reverse move direction more succinctly
                rtEMath_vec3MultScalar(move, -delta, move);
                rtEMath_vec3Add(camera->cameraPos, move, camera->cameraPos);
        }

        if (camera->right) {
                vec3 move;
                rtEMath_vec3Cross(camera->up, camera->cameraLookDirection, move);
                rtEMath_vec3MultScalar(move, -delta, move);
                rtEMath_vec3Add(camera->cameraPos, move, camera->cameraPos);
        }
        if (camera->left) {
                vec3 move;
                rtEMath_vec3Cross(camera->up, camera->cameraLookDirection, move);
                rtEMath_vec3MultScalar(move, delta, move);
                rtEMath_vec3Add(camera->cameraPos, move, camera->cameraPos);
        }
}

mat4* rtGame_getCameraData(struct rtGame_camera* camera) {

//        rtEMath_vec3Add(camera->cameraPos, camera->cameraLookDirection, camera->cameraData[0]);
        mat4 tmp = RTEMATH_MAT4_IDENTITY;
        memcpy(camera->cameraData[0], tmp, sizeof(float) * 16);

        vec3 tmpv3;
        rtEMath_vec3Add(camera->cameraPos, camera->cameraLookDirection, tmpv3);
        rtEMath_mat4CreateLookAt(camera->cameraPos, tmpv3, camera->up, camera->cameraData[1]);

        rtEMath_mat4CreatePerspectiveProjection(camera->fov, camera->nearPlaneDistance, camera->farPlaneDistance, camera->aspectRatio, camera->cameraData[2]);

        return camera->cameraData;
}

void rtGame_destroyCamera(struct rtGame_camera** camera) {
        free(*camera);
        *camera = nullptr;
}
