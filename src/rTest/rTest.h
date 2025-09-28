// TODO: Add a RTEST_MANUAL_CASE macro which will prompt the user to see if functionality is working
#ifndef RTEST_H_
#define RTEST_H_
#include <stdio.h>

#define RTEST_SUITE_BEGIN(suitename) \
int assertionsPassed = 0; \
int assertionsFailed = 0; \
int assertionsTotal = 0; \
int totalTests = 0; \
const char* rTestSuiteName = suitename;\
void rtestmain() \

#define RTEST_CASE(str) \
for ( \
        struct { \
                bool testCaseRan; \
                const char* testName; \
        } testy = {false, (str)}; \
        !testy.testCaseRan; \
        testy.testCaseRan = true, totalTests++\
) \

#define RTEST_ASSERT(statement) \
do { \
        assertionsTotal++;\
        if (!(statement)) { \
                printf("Assertion failure in test '%s':\n\t{%s} on line %d\n\n", testy.testName, (#statement), __LINE__);\
                assertionsFailed++; \
        } else { \
                assertionsPassed++; \
        } \
} while(0) \

extern int assertionsPassed; 
extern int assertionsFailed;
extern int assertionsTotal;
extern int totalTests;
extern const char* rTestSuiteName;
extern void rtestmain();

int main() {
        rtestmain();

        printf("%s Results: %d assertion(s) failed, %d assertion(s) passed (%d assertion(s) in %d test(s))", rTestSuiteName, assertionsFailed, assertionsPassed, assertionsTotal, totalTests);
        return 0;
}

#endif // RTEST_H_
