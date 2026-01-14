#ifndef RTEMATH_H_
#define RTEMATH_H_

#define RTEMATH_VEC3_ZERO {0.0, 0.0, 0.0}
#define RTEMATH_MAT4_IDENTITY {1.0, 0.0, 0.0, 0.0 \
                               0.0, 1.0, 0.0, 0.0 \
                               0.0, 0.0, 1.0, 0.0 \
                               0.0, 0.0, 0.0, 1.0}

typedef double vec3[3];
typedef double mat4[16];

void rtEMath_vec3MultScalar(vec3 a, double factor, vec3 dest);
void rtEMath_vec3DivScalar(vec3 a, double divisor, vec3 dest);
void rtEMath_vec3Add(vec3 a, vec3 b, vec3 dest);
void rtEMath_vec3Sub(vec3 a, vec3 b, vec3 dest);
void rtEMath_vec3Cross(vec3 a, vec3 b, vec3 dest);
double rtEMath_vec3Dot(vec3 a, vec3 b);
void rtEMath_vec3Normalize(vec3 a, vec3 dest);
double rtEMath_vec3GetMagnitude(vec3 a);

void rtEMath_mat4CreateModel(vec3 a, mat4 dest);
void rtEMath_mat4CreateLookAt(vec3 position, vec3 target, vec3 up, mat4 dest);

#endif //RTEMATH_H_
