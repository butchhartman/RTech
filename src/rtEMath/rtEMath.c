#include "rtEMath/rtEMath.h"
#include <math.h>
#include <string.h>

void rtEMath_vec3MultScalar(vec3 a, double factor, vec3 dest) {
        dest[0] = a[0]*factor;
        dest[1] = a[1]*factor;
        dest[2] = a[2]*factor;
}
void rtEMath_vec3DivScalar(vec3 a, double divisor, vec3 dest) {
        dest[0] = a[0]/divisor;
        dest[1] = a[1]/divisor;
        dest[2] = a[2]/divisor;
}
void rtEMath_vec3Add(vec3 a, vec3 b, vec3 dest) {
        dest[0] = a[0]+b[0];
        dest[1] = a[1]+b[1];
        dest[2] = a[2]+b[2];
}
void rtEMath_vec3Sub(vec3 a, vec3 b, vec3 dest) {
        dest[0] = a[0]-b[0];
        dest[1] = a[1]-b[1];
        dest[2] = a[2]-b[2];
}
void rtEMath_vec3Cross(vec3 a, vec3 b, vec3 dest) {
        dest[0] = a[1]*b[2] - a[2]*b[1];
        dest[1] = a[2]*b[0] - a[0]*b[2];
        dest[2] = a[0]*b[1] - a[1]*b[0];
}
double rtEMath_vec3Dot(vec3 a, vec3 b) {
        return a[0]*b[0] + a[1]*b[1] + a[2]*b[2];
}
void rtEMath_vec3Normalize(vec3 a, vec3 dest) {
        rtEMath_vec3DivScalar(a, rtEMath_vec3GetMagnitude(a), dest);
}

double rtEMath_vec3GetMagnitude(vec3 a) {
        return sqrt(
                        a[0]*a[0] +
                        a[1]*a[1] +
                        a[2]*a[2]
                );
}

void rtEMath_mat4CreateModel(vec3 a, mat4 dest) {
        mat4 tmpModel = {
                1.0, 0.0, 0.0, 0.0,
                0.0, 1.0, 0.0, 0.0,
                0.0, 0.0, 1.0, 0.0,
                a[0],a[1],a[2], 1.0
        };
        
        memcpy(dest, tmpModel, sizeof(double) * 16);
}
void rtEMath_mat4CreateLookAt(vec3 position, vec3 target, vec3 up, mat4 dest) {
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
                rightVector[0], finalUp[0], invDirection[0], -position[0],
                rightVector[1], finalUp[1], invDirection[1], -position[1],
                rightVector[2], finalUp[2], invDirection[2], -position[2],
                0.0, 0.0, 0.0, 1.0
        };

        memcpy(dest, tmpMat, sizeof(double) * 16);
}

void rtEMath_mat4CreatePerspectiveProjection(double fov, double near, double far, double aspect, mat4 dest) {
        // big thanks to https://learnwebgl.brown37.net/08_projections/projections_perspective.html
        double top = near * tan(fov/2);
        double bottom = -top;
        double right = top*aspect;
        double left = -top;

        double midX = (left + right)/2.0;
        double midY = (bottom + top)/2.0;

        double scaleX = 2.0 / (right - left);
        double scaleY = 2.0 / (top - bottom);

        
        double c1 = (2*far*near) / (near - far);
        double c2 = (far + near) / (far - near);

        mat4 tmpMat = {
                near*scaleX, 0          , 0 ,  0,
                0          , near*scaleY, 0 ,  0,
                0          , 0          ,-c1, -1,
                midX       , midY       , c2,  0
        };

        memcpy(dest, tmpMat, sizeof(double) * 16);
}




