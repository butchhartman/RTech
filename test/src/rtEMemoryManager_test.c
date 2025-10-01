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

        RTEST_CASE("A stack allocator can be created from the memory manager and reclaimed") {
                struct rtEMemoryManager* manager;
                struct rtEMMStackAllocator* SA;

                rtEMM_createMemoryManager(&manager, 1000);
                RTEST_ASSERT(rtEMM_allocateStackAllocator(&manager, &SA, 50) == rtEErrorCode_SUCCESS);
                RTEST_ASSERT(SA != nullptr);

                rtEMM_cleanupStackAllocator(&SA);
                RTEST_ASSERT(SA == nullptr);
                rtEMM_cleanupMemoryManager(&manager);
        }

        RTEST_CASE("Multiple stack allocators can be allocated from a manager") {
                struct rtEMemoryManager* manager;
                struct rtEMMStackAllocator* SA;
                struct rtEMMStackAllocator* SA1;
                struct rtEMMStackAllocator* SA2;

                rtEMM_createMemoryManager(&manager, 1000);
                RTEST_ASSERT(rtEMM_allocateStackAllocator(&manager, &SA, 50) == rtEErrorCode_SUCCESS);
                RTEST_ASSERT(SA != nullptr);
                RTEST_ASSERT(rtEMM_allocateStackAllocator(&manager, &SA1, 266) == rtEErrorCode_SUCCESS);
                RTEST_ASSERT(SA1 != nullptr);
                RTEST_ASSERT(rtEMM_allocateStackAllocator(&manager, &SA2, 525) == rtEErrorCode_SUCCESS);
                RTEST_ASSERT(SA2 != nullptr);

                rtEMM_cleanupStackAllocator(&SA);
                rtEMM_cleanupStackAllocator(&SA1);
                rtEMM_cleanupStackAllocator(&SA2);
                RTEST_ASSERT(SA == nullptr);
                RTEST_ASSERT(SA1 == nullptr);
                RTEST_ASSERT(SA2 == nullptr);
                rtEMM_cleanupMemoryManager(&manager);
        }

        RTEST_CASE("Stack allocator with exact size of the memory manager") {
                struct rtEMemoryManager* manager;
                struct rtEMMStackAllocator* SA;

                rtEMM_createMemoryManager(&manager, 42);
                RTEST_ASSERT(rtEMM_allocateStackAllocator(&manager, &SA, 10) == rtEErrorCode_SUCCESS);
                RTEST_ASSERT(SA != nullptr);

                rtEMM_cleanupStackAllocator(&SA);
                RTEST_ASSERT(SA == nullptr);
                rtEMM_cleanupMemoryManager(&manager);
        }

        RTEST_CASE("Stack allocator can be allocated to a recently freed block") {
                struct rtEMemoryManager* manager;
                struct rtEMMStackAllocator* SA;
                struct rtEMMStackAllocator* SA1;

                rtEMM_createMemoryManager(&manager, 47);
                RTEST_ASSERT(rtEMM_allocateStackAllocator(&manager, &SA, 10) == rtEErrorCode_SUCCESS);
                RTEST_ASSERT(SA != nullptr);

                rtEMM_cleanupStackAllocator(&SA);
                RTEST_ASSERT(SA == nullptr);
                
                RTEST_ASSERT(rtEMM_allocateStackAllocator(&manager, &SA1, 10) == rtEErrorCode_SUCCESS);
                RTEST_ASSERT(SA1 != nullptr);

                rtEMM_cleanupStackAllocator(&SA1);
                RTEST_ASSERT(SA1 == nullptr);
                
                rtEMM_cleanupMemoryManager(&manager);
        }

        RTEST_CASE("Memory can be allocated to and freed from a stack allocator") {
                struct rtEMemoryManager* manager;
                struct rtEMMStackAllocator* SA;

                rtEMM_createMemoryManager(&manager, 100);
                RTEST_ASSERT(rtEMM_allocateStackAllocator(&manager, &SA, 10 * sizeof(int)) == rtEErrorCode_SUCCESS);
                RTEST_ASSERT(SA != nullptr);

                int* arrayOfTenInts;
                RTEST_ASSERT(rtEMM_stackMalloc(SA, 10 * sizeof(int), &arrayOfTenInts)== rtEErrorCode_SUCCESS);

                for (size_t i = 0; i < 10; i++) {
                        *(arrayOfTenInts + i) = i;
                }

                for (size_t i = 0; i < 10; i++) {
                        RTEST_ASSERT(*(arrayOfTenInts + i) == i);
                }

                RTEST_ASSERT(rtEMM_stackFreeTo(SA, &arrayOfTenInts) == rtEErrorCode_SUCCESS);
                RTEST_ASSERT(arrayOfTenInts == nullptr);
                rtEMM_cleanupMemoryManager(&manager);
        }
}
