#include <rTest/rTest.h>
#include <stdio.h>

STANDARD_TEST_CASE(Pooping) {
        printf("Sample Test");
}

int main() {
        RUN_STANDARD_TEST_CASE(Pooping);
        return 0;
}
