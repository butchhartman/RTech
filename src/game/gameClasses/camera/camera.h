#ifndef GAME_CAMERA_H_
#define GAME_CAMERA_H_
#include "rtEW/rtenginewindow.h"
#include "rtEMath/rtEMath.h"

struct rtGame_camera;

struct rtGame_camera* rtGame_createCamera(
                vec3 position, 
                vec3 targetPos,
                vec3 up,
                float fov,
                float aspectRatio,
                float nearPlaneDistance,
                float farPlaneDistance,
                float sensitivity
                );

void rtGame_cameraHandleInput(struct rtGame_camera* camera, const struct inputEvent event);

void rtGame_cameraUpdate(struct rtGame_camera* camera, double delta);

mat4* rtGame_getCameraData(struct rtGame_camera* camera);

void rtGame_destroyCamera(struct rtGame_camera** camera);

#endif // GAME_CAMERA_H_
