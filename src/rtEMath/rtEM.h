#ifndef RTEMATH_H_
#define RTEMATH_H_

// a00 = col 0, row 0
// a01 = col 0, row 1
// a10 = col 1, row 0

#define RTEM_MAT4_IDENTITY {{1.0, 0.0, 0.0, 0.0,  \
                             0.0, 1.0, 0.0, 0.0,  \
                             0.0, 0.0, 1.0, 0.0,  \
                             0.0, 0.0, 0.0, 1.0}} \

union rtEM_mat4 {
        // [col][row]
        float mat[16];
        struct {
                float a00;
                float a01;
                float a02;
                float a03;
                float a10;
                float a11;
                float a12;
                float a13;
                float a20;
                float a21;
                float a22;
                float a23;
                float a30;
                float a31;
                float a32;
                float a33;
        };
};

void rtEM_mat4Log(union rtEM_mat4 mat);
void rtEM_mat4Print(union rtEM_mat4 mat);

union rtEM_mat4 rtEM_mat4MultiplyMat4(union rtEM_mat4 a, union rtEM_mat4 b);

#endif // RTEMATH_H_
