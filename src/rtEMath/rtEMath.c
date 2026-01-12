#include "rtEMath/rtEMath.h"
#include <assert.h>
#include <stdarg.h>
#include <stdlib.h>

struct vector {
        unsigned int totalLength;
        double* data;
};

struct vector* rtEM_createVector(unsigned int length) {
        struct vector* newVector = malloc(sizeof(struct vector));

        if (newVector == nullptr) {
                abort();
        }

        newVector->totalLength = length;
        newVector->data = malloc(sizeof(double) * length);

        if (newVector->data == nullptr) {
                abort();
        }

        return newVector;
}

double rtEM_vectorGetAt(struct vector* vec, unsigned int index) {
        if (index <= vec->totalLength- 1) {
                return vec->data[index];
        } else {
                // I think this is the way to go, not sure proper form here, should probably log somehow as well
                abort();
        }
}

void rtEM_vectorSetAt(struct vector* vec, unsigned int index, double value) {
        if (index <= vec->totalLength - 1) {
                vec->data[index] = value;
        } else {
                abort();
        }
}

double* rtEM_getDataPtr(const struct vector* vec) {
        return vec->data;
}

void rtEM_cleanupVector(struct vector* vec) {
        free(vec->data);
        free(vec);
}




