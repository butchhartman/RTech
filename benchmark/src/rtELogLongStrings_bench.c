#include "rtELog/rtELog.h"
#include "rtEMemoryManager/rtEMemoryManager.h"
#include "rtEMemoryManager/rtEStackAllocator.h"
#include "rtEMemoryManager/structs/rtEAllocatorProcs.h"
#include "rTest/rTest.h"

static void* customMalloc(size_t size, void* usr) {
        struct rtEMMStackAllocator* alloc = (struct rtEMMStackAllocator*)(usr);
        void* block;
        if (rtEMM_stackMalloc(alloc, size, &block) != rtEErrorCode_SUCCESS) {
                return nullptr;
        }
        
        return block;
}
static void customFree(void** ptr, void* usr) {
        struct rtEMMStackAllocator* alloc = (struct rtEMMStackAllocator*)(usr);
        rtEMM_stackFreeTo(alloc, ptr);
}

RTEST_SUITE_BEGIN("rtELog benchmarking") {
                struct rtEMemoryManager* manager;
                rtEMM_createMemoryManager(&manager, 2000);
                struct rtEMMStackAllocator* SA;
                rtEMM_allocateStackAllocator(manager, &SA, 1000);


        RTEST_BENCHMARK_RUN("100k With default allocator", 10) {
                rtELog_init("100kBenchDefAlloc");
                for (size_t i = 0; i < 100000; i++) {
                        rtELog_logError("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
                }
                rtELog_cleanup();
        }

        RTEST_BENCHMARK_RUN("100k with custom stack allocator", 10) {

                struct rtEAllocatorProcs procs = {
                        .rtEA_malloc = customMalloc,
                        .rtEA_free = customFree,
                        .usr = SA
                };

                rtELog_setAllocator(procs);

                rtELog_init("100kBenchStackAlloc");
                for (size_t i = 0; i < 100000; i++) {
                        rtELog_logError("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
                }
                rtELog_cleanup();
        }

        rtEMM_cleanupMemoryManager(&manager);
}
