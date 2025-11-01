#include "rTest/rTest.h"
#include "rtEMath/rtEM.h"

RTEST_SUITE_BEGIN("rtEMath Tests") {
        RTEST_CASE("Identity mat4s are assigned correctly") {
                union rtEM_mat4 mat = RTEM_MAT4_IDENTITY; 
                RTEST_ASSERT(mat.a00 == 1.0);
                RTEST_ASSERT(mat.a11 == 1.0);
                RTEST_ASSERT(mat.a22 == 1.0);
                RTEST_ASSERT(mat.a33 == 1.0);
                mat.a21 = 6.0;
                rtEM_mat4Print(mat);
        }

        RTEST_CASE("mat4 multiplication works") {
                union rtEM_mat4 mata = {{2.0, 3.0, 0.0, 0.0,
                                        6.0, 1.0, 2.0, 7.0,
                                        0.0, 0.0, 0.0, 0.0,
                                        3.0, 1.0, 2.0, 6.0}};

                union rtEM_mat4 matb = {{7.0, 3.0, 3.0, 8.0,
                                        1.0, 3.0, 2.0, 7.0,
                                        2.0, 0.0, 0.1, 9.0,
                                        3.0, 4.0, 2.0, 6.0}};
                rtEM_mat4Print(rtEM_mat4MultiplyMat4(mata, matb));

                }
}
