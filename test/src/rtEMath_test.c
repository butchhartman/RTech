#include "rtELog/rtELog.h"
#include "rtEMath/rtEMath.h"
#include "rTest/rTest.h"
#include <stdio.h>
#define _USE_MATH_DEFINES
#include <math.h>


RTEST_SUITE_BEGIN("rtEMath Suite") {
        rtELog_init("RTEMATH_TEST_LOG");
        RTEST_CASE("vector operations") {
                vec3 vec1 = {1.0, 2.0, 3.0};
                vec3 vec2= {1.5, 7.0, 1.0};

                vec3 destVec = RTEMATH_VEC3_ZERO;

                rtEMath_vec3MultScalar(vec1, 6, destVec);

                RTEST_ASSERT(destVec[0] == 6.0 && destVec[1] == 12.0 && destVec[2] == 18.0);

                rtEMath_vec3DivScalar(vec1, 2, destVec);

                RTEST_ASSERT(destVec[0] == 0.5 && destVec[1] == 1.0 && destVec[2] == 1.5);

                rtEMath_vec3Add(vec1, vec2, destVec);

                RTEST_ASSERT(destVec[0] == 2.5 && destVec[1] == 9.0 && destVec[2] == 4.0);

                rtEMath_vec3Sub(vec1, vec2, destVec);

                RTEST_ASSERT(destVec[0] == -0.5 && destVec[1] == -5.0 && destVec[2] == 2.0);

                rtEMath_vec3Cross(vec1, vec2, destVec);
                rtELog_logInfo("Cross prod: %f, %f, %f", destVec[0], destVec[1], destVec[2]) ;
                RTEST_ASSERT(destVec[0] == -19 && destVec[1] == 3.5 && destVec[2] == 4.0);

                rtELog_logInfo("Dot prod: %f", rtEMath_vec3Dot(vec1, vec2));
                RTEST_ASSERT(rtEMath_vec3Dot(vec1, vec2) == 18.5);

                rtEMath_vec3Normalize(vec1, destVec);
                rtELog_logInfo("Normalized vec 1: %f, %f, %f", destVec[0], destVec[1], destVec[2]) ;
                rtELog_logInfo("Magnitude: %f", rtEMath_vec3GetMagnitude(vec1));
        }

        RTEST_CASE("Matrix operations") {
                mat4 identity = RTEMATH_MAT4_IDENTITY;
                mat4 destMat;
                vec3 zero = RTEMATH_VEC3_ZERO;
                vec3 up = {0.0, 1.0, 0.0};
                vec3 point = {0.0, 0.0, 5.0};

                rtEMath_mat4CreateModel(point, destMat);
                printf("%f, %f, %f, %f\n %f, %f, %f, %f\n, %f, %f, %f, %f\n, %f, %f, %f, %f\n\n",
                        destMat[0], destMat[4], destMat[8], destMat[12],
                        destMat[1], destMat[5], destMat[9], destMat[13],
                        destMat[2], destMat[6], destMat[10], destMat[14],
                        destMat[3], destMat[7], destMat[11], destMat[15]);

                rtEMath_mat4CreateLookAt(zero, point, up, destMat);
                printf("%f, %f, %f, %f\n %f, %f, %f, %f\n, %f, %f, %f, %f\n, %f, %f, %f, %f\n\n",
                        destMat[0], destMat[4], destMat[8], destMat[12],
                        destMat[1], destMat[5], destMat[9], destMat[13],
                        destMat[2], destMat[6], destMat[10], destMat[14],
                        destMat[3], destMat[7], destMat[11], destMat[15]);

                rtEMath_mat4CreatePerspectiveProjection(45.0*M_PI/360, 0.1, 100, 16.0/9.0, destMat);
                printf("%f, %f, %f, %f\n %f, %f, %f, %f\n, %f, %f, %f, %f\n, %f, %f, %f, %f\n\n",
                        destMat[0], destMat[4], destMat[8], destMat[12],
                        destMat[1], destMat[5], destMat[9], destMat[13],
                        destMat[2], destMat[6], destMat[10], destMat[14],
                        destMat[3], destMat[7], destMat[11], destMat[15]);

        }

        rtELog_cleanup();
}
