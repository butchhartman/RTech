#ifndef RTEST_H_
#define RTEST_H_
#include <stdio.h>
#include <time.h>
// TODO: Add a RTEST_MANUAL_CASE macro which will prompt the user to see if functionality is working

/**
* @file
*
* @note
* Each test file must have one and only one @ref RTEST_SUITE_BEGIN to work.
*
* @warning
* It has not been tested, but is likely that including headers after this one may cause issues.
*/

#define RTEST_BENCHMARK_RUN(testname, trials) \
for ( \
        struct { \
                size_t i; \
                const char* testName; \
                double clk; \
        } testy = {0, (testname), (double)clock()/CLOCKS_PER_SEC}; \
        testy.i < trials; \
        testy.i++, totalTests++, (testy.i == trials) ? printf("%s timing benchmark ran. Process took average of %f seconds\n", testname, ((double)clock()/CLOCKS_PER_SEC-testy.clk)/trials) : 0\
) \

/**
* Initializes state used for managing tests
*
* @param suitename A string to identify the name of the test suite
*/
#define RTEST_SUITE_BEGIN(suitename) \
int assertionsPassed = 0; \
int assertionsFailed = 0; \
int assertionsTotal = 0; \
int totalTests = 0; \
const char* rTestSuiteName = suitename;\
void rtestmain() \

/**
* Defines a test case to run 
*
* @param str - Description of the test case
*/
#define RTEST_CASE(str) \
for ( \
        struct { \
                bool testCaseRan; \
                const char* testName; \
        } testy = {false, (str)}; \
        !testy.testCaseRan; \
        testy.testCaseRan = true, totalTests++\
) \

/**
* Asserts the passed expression is true and prints debug info otherwise
*
* @param statement - The expression to assert 
*/
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
