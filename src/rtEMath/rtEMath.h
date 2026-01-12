#ifndef RTEMATH_H_
#define RTEMATH_H_

struct vector;

struct vector* rtEM_createVector(unsigned int length);
double rtEM_vectorGetAt(struct vector* vec, unsigned int index);
void rtEM_vectorSetAt(struct vector* vec, unsigned int index, double value);
double* rtEM_getDataPtr(const struct vector* vec);
void rtEM_cleanupVector(struct vector* vec);


#endif //RTEMATH_H_
