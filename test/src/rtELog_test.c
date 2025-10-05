
#include <rTest/rTest.h>
#include "rtEErrorCodes/rtEErrorCodes.h"
#include "rtELog/rtELog.h"
#include "rtEMemoryManager/rtEMemoryManager.h"
#include "rtEMemoryManager/rtEStackAllocator.h"
#include "rtEMemoryManager/structs/rtEAllocatorProcs.h"
#include "rtEMemoryManager/structs/rtEStackAllocatorStruct.h"
static void* customMalloc(size_t size, void* usr) {
        struct rtEMMStackAllocator* alloc = (struct rtEMMStackAllocator*)(usr);

        void* dst;
        rtEMM_stackMalloc(alloc, size, &dst);
        return dst;
}

static void customFree(void** ptr, void* usr) {
        struct rtEMMStackAllocator* alloc = (struct rtEMMStackAllocator*)(usr);
        rtEMM_stackFreeTo(alloc, ptr);
}

RTEST_SUITE_BEGIN("rtELog Suite") {
        RTEST_CASE("The rtELog module can start and terminate") {
                RTEST_ASSERT(rtELog_init("rTestCase0") == rtEErrorCode_SUCCESS);
                RTEST_ASSERT(rtELog_cleanup() == rtEErrorCode_SUCCESS);
        }
        RTEST_CASE("The rtELog module can start and terminate repeatedly") {
                RTEST_ASSERT(rtELog_init("rTestCase0") == rtEErrorCode_SUCCESS);
                RTEST_ASSERT(rtELog_cleanup() == rtEErrorCode_SUCCESS);
                RTEST_ASSERT(rtELog_init("rTestCase0") == rtEErrorCode_SUCCESS);
                RTEST_ASSERT(rtELog_cleanup() == rtEErrorCode_SUCCESS);
                RTEST_ASSERT(rtELog_init("rTestCase0") == rtEErrorCode_SUCCESS);
                RTEST_ASSERT(rtELog_cleanup() == rtEErrorCode_SUCCESS);
                RTEST_ASSERT(rtELog_init("rTestCase0") == rtEErrorCode_SUCCESS);
                RTEST_ASSERT(rtELog_cleanup() == rtEErrorCode_SUCCESS);
                RTEST_ASSERT(rtELog_init("rTestCase0") == rtEErrorCode_SUCCESS);
                RTEST_ASSERT(rtELog_cleanup() == rtEErrorCode_SUCCESS);
                RTEST_ASSERT(rtELog_init("rTestCase0") == rtEErrorCode_SUCCESS);
                RTEST_ASSERT(rtELog_cleanup() == rtEErrorCode_SUCCESS);
                RTEST_ASSERT(rtELog_init("rTestCase0") == rtEErrorCode_SUCCESS);
                RTEST_ASSERT(rtELog_cleanup() == rtEErrorCode_SUCCESS);
        }

        RTEST_CASE("A file can be logged to with all logging macros/functions") {
                RTEST_ASSERT(rtELog_init("rTestCase1") == rtEErrorCode_SUCCESS);
                rtELog_log("Normal Log");
                rtELog_logError("Error Log!");
                rtELog_logWarning("Warning Log!");
                rtELog_logInfo("Info Log!");
                rtELog_debug_logError("dbError Log!");
                rtELog_debug_logWarning("dbWarning Log!");
                rtELog_debug_logInfo("dbInfo Log!");
                RTEST_ASSERT(rtELog_cleanup() == rtEErrorCode_SUCCESS);
        }

        RTEST_CASE("A logger can use a custom memory allocator") {
                struct rtEMemoryManager* manager;
                struct rtEMMStackAllocator* sa;
                RTEST_ASSERT(rtEMM_createMemoryManager(&manager, 2000) == rtEErrorCode_SUCCESS);
                RTEST_ASSERT(rtEMM_allocateStackAllocator(manager, &sa, 1000) == rtEErrorCode_SUCCESS);

                struct rtEAllocatorProcs allocator = {
                                .rtEA_malloc = customMalloc,
                                .rtEA_free = customFree,
                                .usr = sa,
                        };
                rtELog_setAllocator(allocator);
                RTEST_ASSERT(rtELog_init("rTestCaseCustomAllocator") == rtEErrorCode_SUCCESS);
                for (size_t i = 0; i < 10000; i++) {
                        rtELog_logWarning("It would be a shame if there was a memory leak!!!");
                }
                rtEMM_dumpBuffer(sa);
                RTEST_ASSERT(rtELog_cleanup() == rtEErrorCode_SUCCESS);
        }
}
