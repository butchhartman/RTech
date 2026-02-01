#include "rtEMath/rtEMath.h"
#include <math.h>
#include <string.h>

void rtEMath_vec3MultScalar(const vec3 a, const float factor, vec3 dest) {
        dest[0] = a[0]*factor;
        dest[1] = a[1]*factor;
        dest[2] = a[2]*factor;
}
void rtEMath_vec3DivScalar(const vec3 a, const float divisor, vec3 dest) {
        dest[0] = a[0]/divisor;
        dest[1] = a[1]/divisor;
        dest[2] = a[2]/divisor;
}
void rtEMath_vec3Add(const vec3 a, const vec3 b, vec3 dest) {
        dest[0] = a[0]+b[0];
        dest[1] = a[1]+b[1];
        dest[2] = a[2]+b[2];
}
void rtEMath_vec3Sub(const vec3 a, const vec3 b, vec3 dest) {
        dest[0] = a[0]-b[0];
        dest[1] = a[1]-b[1];
        dest[2] = a[2]-b[2];
}
void rtEMath_vec3Cross(const vec3 a, const vec3 b, vec3 dest) {
        dest[0] = a[1]*b[2] - a[2]*b[1];
        dest[1] = a[2]*b[0] - a[0]*b[2];
        dest[2] = a[0]*b[1] - a[1]*b[0];
}
float rtEMath_vec3Dot(const vec3 a, const vec3 b) {
        return a[0]*b[0] + a[1]*b[1] + a[2]*b[2];
}
void rtEMath_vec3Normalize(const vec3 a, vec3 dest) {
        rtEMath_vec3DivScalar(a, rtEMath_vec3GetMagnitude(a), dest);
}

float rtEMath_vec3GetMagnitude(const vec3 a) {
        return sqrt(
                        a[0]*a[0] +
                        a[1]*a[1] +
                        a[2]*a[2]
                );
}

void rtEMath_mat4CreateModel(const vec3 a, mat4 dest) {
        mat4 tmpModel = {
                1.0, 0.0, 0.0, 0.0,
                0.0, 1.0, 0.0, 0.0,
                0.0, 0.0, 1.0, 0.0,
                a[0],a[1],a[2], 1.0
        };
        
        memcpy(dest, tmpModel, sizeof(float) * 16);
}

void rtEMath_mat4CreateLookAt(const vec3 position, const vec3 target, const vec3 up, mat4 dest) {
        vec3 invDirection = RTEMATH_VEC3_ZERO; 
        vec3 rightVector = RTEMATH_VEC3_ZERO;  
        vec3 finalUp = RTEMATH_VEC3_ZERO; 

        // Get Z axis unit vector
        rtEMath_vec3Sub(position, target, invDirection);
        rtEMath_vec3Normalize(invDirection, invDirection);

        // Cross with up vector to get X axis unit vector
        rtEMath_vec3Cross(up, invDirection, rightVector);
        rtEMath_vec3Normalize(rightVector, rightVector);

        // Cross Z and X axes to get the real Y axis unit vector
        rtEMath_vec3Cross(invDirection, rightVector, finalUp);
        rtEMath_vec3Normalize(finalUp, finalUp);

        mat4 tmpMat = {
                rightVector[0], finalUp[0], invDirection[0], 0,
                rightVector[1], finalUp[1], invDirection[1], 0,
                rightVector[2], finalUp[2], invDirection[2], 0,
                -(rtEMath_vec3Dot(rightVector, position)), -(rtEMath_vec3Dot(finalUp, position)), -(rtEMath_vec3Dot(invDirection, position)), 1.0
        };

        memcpy(dest, tmpMat, sizeof(float) * 16);
}

void rtEMath_mat4CreatePerspectiveProjection(const float fov, const float near, const float far, const float aspect, mat4 dest) {
        // big thanks to https://learnwebgl.brown37.net/08_projections/projections_perspective.html
        float top = near * tan(fov/2);
        float bottom = -top;
        float right = top*aspect;
        float left = -right;

        mat4 tmpMat = {
                2*near/(right-left), 0          , 0 ,  0,
                0          , 2*near/(top-bottom), 0 ,  0,
                0          , 0          ,-(far+near)/(far-near), -1,
                -near*(right+left)/(right-left), -near*(top+bottom)/(top-bottom), 2*far*near/(near-far),  0
        };

        memcpy(dest, tmpMat, sizeof(float) * 16);
}




