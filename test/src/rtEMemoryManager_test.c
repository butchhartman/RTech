#include <rTest/rTest.h>
#include "rtEErrorCodes/rtEErrorCodes.h"
#include "rtEMemoryManager/rtEMemoryManager.h"

RTEST_SUITE_BEGIN("rtEMemoryManager Tests") {
        RTEST_CASE("rtEMemory manager starts up and shuts down without error") {
                struct rtEMemoryManager* manager;
                RTEST_ASSERT(rtEMM_createMemoryManager(&manager, 150) == rtEErrorCode_SUCCESS);

                RTEST_ASSERT(manager != nullptr);

                RTEST_ASSERT(rtEMM_cleanupMemoryManager(&manager) == rtEErrorCode_SUCCESS);

                RTEST_ASSERT(manager == nullptr);
        }

        RTEST_CASE("Very large blocks of memory can be allocated") {
                struct rtEMemoryManager* manager;
                // 1 gigabyte
                RTEST_ASSERT(rtEMM_createMemoryManager(&manager, 1000000000) == rtEErrorCode_SUCCESS);

                RTEST_ASSERT(manager != nullptr);

                RTEST_ASSERT(rtEMM_cleanupMemoryManager(&manager) == rtEErrorCode_SUCCESS);

                RTEST_ASSERT(manager == nullptr);
        }

}
