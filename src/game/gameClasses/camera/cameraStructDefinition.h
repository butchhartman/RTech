#ifndef RTGAME_CAMERA_STRUCT_DEFINITION_H_
#define RTGAME_CAMERA_STRUCT_DEFINITION_H_
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

#endif // RTGAME_CAMERA_STRUCT_DEFINITION_H_
