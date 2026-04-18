#ifndef RTEMATH_H_
#define RTEMATH_H_

#define RTEMATH_VEC3_ZERO {0.0, 0.0, 0.0}
#define RTEMATH_MAT4_IDENTITY {1.0, 0.0, 0.0, 0.0, \
                               0.0, 1.0, 0.0, 0.0, \
                               0.0, 0.0, 1.0, 0.0, \
                               0.0, 0.0, 0.0, 1.0}

#define _USE_MATH_DEFINES

#define RTEMATH_VEC3_SIZE sizeof(float) * 3
#define RTEMATH_MAT4_SIZE sizeof(float) * 16

#include <math.h>

typedef float vec3[3];
typedef float mat4[16];

void rtEMath_vec3MultScalar(const vec3 a, const float factor, vec3 dest);
void rtEMath_vec3DivScalar(const vec3 a, const float divisor, vec3 dest);
void rtEMath_vec3Add(const vec3 a, const vec3 b, vec3 dest);
void rtEMath_vec3Sub(const vec3 a, const vec3 b, vec3 dest);
void rtEMath_vec3Cross(const vec3 a, const vec3 b, vec3 dest);
float rtEMath_vec3Dot(const vec3 a, const vec3 b);
void rtEMath_vec3Normalize(const vec3 a, vec3 dest);
float rtEMath_vec3GetMagnitude(const vec3 a);

void rtEMath_mat4CreateModel(const vec3 a, mat4 dest);
void rtEMath_mat4CreateLookAt(const vec3 position, const vec3 target, const vec3 up, mat4 dest);
void rtEMath_mat4CreatePerspectiveProjection(const float fov, const float near, const float far, const float aspect, mat4 dest);

inline float toRadians(const float deg) {
        return (2 * M_PI * deg)/360.0;
}
#endif //RTEMATH_H_
