#include "rtEMath/rtEMath.h"
#include "rTest/rTest.h"


RTEST_SUITE_BEGIN("rtEMath Suite") {
        RTEST_CASE("Creating a vector") {
                struct vector* myVec = rtEM_createVector(3);
                rtEM_cleanupVector(myVec);
        }

        RTEST_CASE("Acessing a vector") {
                struct vector* myVec = rtEM_createVector(3);

                rtEM_vectorSetAt(myVec, 0, 69);
                rtEM_vectorSetAt(myVec, 1, 67);
                rtEM_vectorSetAt(myVec, 2, 6742069);

                RTEST_ASSERT(rtEM_vectorGetAt(myVec, 0) == 69);
                RTEST_ASSERT(rtEM_vectorGetAt(myVec, 1) == 67);
                RTEST_ASSERT(rtEM_vectorGetAt(myVec, 2) == 6742069);

                rtEM_cleanupVector(myVec);
        }
}
