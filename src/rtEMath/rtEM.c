#include "rtEMath/rtEM.h"
#include "rtELog/rtELog.h"
#include <stddef.h>
#include <stdio.h>

void rtEM_mat4Log(union rtEM_mat4 mat) {
        rtELog_logInfo("%.2f %.2f %.2f %.2f\n%.2f %.2f %.2f %.2f\n%.2f %.2f %.2f %.2f\n%.2f %.2f %.2f %.2f", 
        mat.a00, mat.a10, mat.a20, mat.a30, 
        mat.a01, mat.a11, mat.a21, mat.a31,
        mat.a02, mat.a12, mat.a22, mat.a32,
        mat.a03, mat.a13, mat.a23, mat.a33);
}

void rtEM_mat4Print(union rtEM_mat4 mat) {
        printf("%.2f %.2f %.2f %.2f\n%.2f %.2f %.2f %.2f\n%.2f %.2f %.2f %.2f\n%.2f %.2f %.2f %.2f\n", 
        mat.a00, mat.a10, mat.a20, mat.a30, 
        mat.a01, mat.a11, mat.a21, mat.a31,
        mat.a02, mat.a12, mat.a22, mat.a32,
        mat.a03, mat.a13, mat.a23, mat.a33);
}

union rtEM_mat4 rtEM_mat4CreateTranslation(float x, float y, float z) {
        union rtEM_mat4 mat = {
                {
                        1.0, 0.0, 0.0, 0.0,
                        0.0, 1.0, 0.0, 1.0,
                        0.0, 0.0, 1.0, 0.0,
                        x, y, z, 1.0

                }
        };
        return mat;
}

union rtEM_mat4 rtEM_mat4MultiplyMat4(union rtEM_mat4 a, union rtEM_mat4 b) {
        union rtEM_mat4 product;
        for (size_t row = 0; row < 4; row++) {
                for (size_t col = 0; col < 4; col++) {

                        /* a00 = 
                                a.00 * b.00 +
                                a.10 * b.01 +
                                a.20 * b.02 +
                                a.30 * b.03 +
                                */

                        product.mat[col * 4 + row] =

                                a.mat[0 * 4 + row] * b.mat[col * 4 + 0] +
                                a.mat[1 * 4 + row] * b.mat[col * 4 + 1] +
                                a.mat[2 * 4 + row] * b.mat[col * 4 + 2] +
                                a.mat[3 * 4 + row] * b.mat[col * 4 + 3];

                }
        }

        return product;
}
