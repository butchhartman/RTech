#include <rTest/rTest.h>
#include "rtEMemoryManager/rtEMemoryManager.h"
#include "rtEErrorCodes/rtEErrorCodes.h"
#include "rtEMemoryManager/structs/rtEStackAllocatorStruct.h"
#include <rtEMemoryManager/rtEStackAllocator.h>

RTEST_SUITE_BEGIN("rtEStackAllocator Tests") {
        RTEST_CASE("A stack allocator can be allocated from a memory manager") {
                struct rtEMemoryManager* manager;
                RTEST_ASSERT(rtEMM_createMemoryManager(&manager, 1000) == rtEErrorCode_SUCCESS);

                struct rtEMMStackAllocator* stackAlloc;
                RTEST_ASSERT(rtEMM_allocateStackAllocator(manager, &stackAlloc, 250) == rtEErrorCode_SUCCESS);
                RTEST_ASSERT(rtEMM_cleanupStackAllocator(&stackAlloc) == rtEErrorCode_SUCCESS);

                RTEST_ASSERT(rtEMM_cleanupMemoryManager(&manager) == rtEErrorCode_SUCCESS);
        }

        RTEST_CASE("Multiple stack allocators can be allocated from a memory manager") {
                struct rtEMemoryManager* manager;
                RTEST_ASSERT(rtEMM_createMemoryManager(&manager, 1000) == rtEErrorCode_SUCCESS);

                struct rtEMMStackAllocator* stackAlloc;
                RTEST_ASSERT(rtEMM_allocateStackAllocator(manager, &stackAlloc, 250) == rtEErrorCode_SUCCESS);

                struct rtEMMStackAllocator* stackAlloc1;
                RTEST_ASSERT(rtEMM_allocateStackAllocator(manager, &stackAlloc1, 250) == rtEErrorCode_SUCCESS);

                struct rtEMMStackAllocator* stackAlloc2;
                RTEST_ASSERT(rtEMM_allocateStackAllocator(manager, &stackAlloc2, 250) == rtEErrorCode_SUCCESS);


                RTEST_ASSERT(rtEMM_cleanupStackAllocator(&stackAlloc2) == rtEErrorCode_SUCCESS);
                RTEST_ASSERT(rtEMM_cleanupStackAllocator(&stackAlloc1) == rtEErrorCode_SUCCESS);
                RTEST_ASSERT(rtEMM_cleanupStackAllocator(&stackAlloc) == rtEErrorCode_SUCCESS);

                RTEST_ASSERT(rtEMM_cleanupMemoryManager(&manager) == rtEErrorCode_SUCCESS);
        }

        RTEST_CASE("A stack allocator will take the place of a de-allocated stack allocator instead of a new block") {
                struct rtEMemoryManager* manager;
                RTEST_ASSERT(rtEMM_createMemoryManager(&manager, 1000) == rtEErrorCode_SUCCESS);

                struct rtEMMStackAllocator* stackAlloc;
                RTEST_ASSERT(rtEMM_allocateStackAllocator(manager, &stackAlloc, 250) == rtEErrorCode_SUCCESS);
                RTEST_ASSERT(rtEMM_cleanupStackAllocator(&stackAlloc) == rtEErrorCode_SUCCESS);

                RTEST_ASSERT(rtEMM_allocateStackAllocator(manager, &stackAlloc, 250) == rtEErrorCode_SUCCESS);
                RTEST_ASSERT(rtEMM_cleanupStackAllocator(&stackAlloc) == rtEErrorCode_SUCCESS);

                RTEST_ASSERT(rtEMM_cleanupMemoryManager(&manager) == rtEErrorCode_SUCCESS);
        }

        RTEST_CASE("A stack allocator taking the place of a de-allocated stack allocator does not blow up the headers") {
                struct rtEMemoryManager* manager;
                RTEST_ASSERT(rtEMM_createMemoryManager(&manager, 1000) == rtEErrorCode_SUCCESS);

                struct rtEMMStackAllocator* stackAlloc;
                RTEST_ASSERT(rtEMM_allocateStackAllocator(manager, &stackAlloc, 250) == rtEErrorCode_SUCCESS);
                RTEST_ASSERT(rtEMM_cleanupStackAllocator(&stackAlloc) == rtEErrorCode_SUCCESS);

                RTEST_ASSERT(rtEMM_allocateStackAllocator(manager, &stackAlloc, 250) == rtEErrorCode_SUCCESS);

                struct rtEMMStackAllocator* stackAlloc2;
                RTEST_ASSERT(rtEMM_allocateStackAllocator(manager, &stackAlloc2, 251) == rtEErrorCode_SUCCESS);

                RTEST_ASSERT(rtEMM_cleanupStackAllocator(&stackAlloc) == rtEErrorCode_SUCCESS);
                RTEST_ASSERT(rtEMM_cleanupStackAllocator(&stackAlloc2) == rtEErrorCode_SUCCESS);

                RTEST_ASSERT(rtEMM_cleanupMemoryManager(&manager) == rtEErrorCode_SUCCESS);
        }

        RTEST_CASE("A stack allocator can be the same size as a memory manager") {
                struct rtEMemoryManager* manager;
                RTEST_ASSERT(rtEMM_createMemoryManager(&manager, 1000) == rtEErrorCode_SUCCESS);

                struct rtEMMStackAllocator* stackAlloc;
                RTEST_ASSERT(rtEMM_allocateStackAllocator(manager, &stackAlloc, 968) == rtEErrorCode_SUCCESS);
                RTEST_ASSERT(rtEMM_cleanupStackAllocator(&stackAlloc) == rtEErrorCode_SUCCESS);

                RTEST_ASSERT(rtEMM_cleanupMemoryManager(&manager) == rtEErrorCode_SUCCESS);
        }
        
        RTEST_CASE("A stack allocator cannot be allocated from a full memory manager") {
                struct rtEMemoryManager* manager;
                RTEST_ASSERT(rtEMM_createMemoryManager(&manager, 1000) == rtEErrorCode_SUCCESS);

                struct rtEMMStackAllocator* stackAlloc;
                struct rtEMMStackAllocator* stackAlloc2;
                RTEST_ASSERT(rtEMM_allocateStackAllocator(manager, &stackAlloc, 968) == rtEErrorCode_SUCCESS);
                RTEST_ASSERT(rtEMM_allocateStackAllocator(manager, &stackAlloc2, 968) != rtEErrorCode_SUCCESS);
                RTEST_ASSERT(rtEMM_cleanupStackAllocator(&stackAlloc) == rtEErrorCode_SUCCESS);
                RTEST_ASSERT(rtEMM_cleanupStackAllocator(&stackAlloc2) != rtEErrorCode_SUCCESS);

                RTEST_ASSERT(rtEMM_cleanupMemoryManager(&manager) == rtEErrorCode_SUCCESS);
        }

        RTEST_CASE("A stack allocator cannot be larger than a memory manager") {
                struct rtEMemoryManager* manager;
                RTEST_ASSERT(rtEMM_createMemoryManager(&manager, 100) == rtEErrorCode_SUCCESS);

                struct rtEMMStackAllocator* stackAlloc;
                RTEST_ASSERT(rtEMM_allocateStackAllocator(manager, &stackAlloc, 1000) != rtEErrorCode_SUCCESS);
                RTEST_ASSERT(rtEMM_cleanupStackAllocator(&stackAlloc) != rtEErrorCode_SUCCESS);

                RTEST_ASSERT(rtEMM_cleanupMemoryManager(&manager) == rtEErrorCode_SUCCESS);
        }

        RTEST_CASE("Failed allocations do not prevent subsequent allocations") {
                struct rtEMemoryManager* manager;
                RTEST_ASSERT(rtEMM_createMemoryManager(&manager, 100) == rtEErrorCode_SUCCESS);

                struct rtEMMStackAllocator* stackAlloc;
                RTEST_ASSERT(rtEMM_allocateStackAllocator(manager, &stackAlloc, 1000) != rtEErrorCode_SUCCESS);
                RTEST_ASSERT(rtEMM_cleanupStackAllocator(&stackAlloc) != rtEErrorCode_SUCCESS);

                struct rtEMMStackAllocator* stackAlloc2;
                RTEST_ASSERT(rtEMM_allocateStackAllocator(manager, &stackAlloc2, 15) == rtEErrorCode_SUCCESS);
                RTEST_ASSERT(rtEMM_cleanupStackAllocator(&stackAlloc2) == rtEErrorCode_SUCCESS);

                RTEST_ASSERT(rtEMM_cleanupMemoryManager(&manager) == rtEErrorCode_SUCCESS);
        }

        RTEST_CASE("Allocated stack allocators can correctly store values") {
                struct rtEMemoryManager* manager;
                RTEST_ASSERT(rtEMM_createMemoryManager(&manager, 100) == rtEErrorCode_SUCCESS);

                struct rtEMMStackAllocator* stackAlloc;
                RTEST_ASSERT(rtEMM_allocateStackAllocator(manager, &stackAlloc, 20) == rtEErrorCode_SUCCESS);

                unsigned char* arrayOf10Chars;

                RTEST_ASSERT(rtEMM_stackMalloc(stackAlloc, 10, (void**)(&arrayOf10Chars)) == rtEErrorCode_SUCCESS);

                int ind=0;
                for (unsigned char i = 'A'; i < 'A'+10; i++, ind++) {
                        *(arrayOf10Chars + ind) = i;
                }
                ind = 0;
                for (unsigned char i = 'A'; i < 'A'+10; i++, ind++) {
                        RTEST_ASSERT(*(arrayOf10Chars + ind) == i);
                }

                RTEST_ASSERT(rtEMM_stackFreeTo(stackAlloc, (void**)(&arrayOf10Chars)) == rtEErrorCode_SUCCESS);

                RTEST_ASSERT(rtEMM_cleanupStackAllocator(&stackAlloc) == rtEErrorCode_SUCCESS);
                RTEST_ASSERT(rtEMM_cleanupMemoryManager(&manager) == rtEErrorCode_SUCCESS);
        }

        RTEST_CASE("A large stack allocator can be allocated from a large memory manager") {
                struct rtEMemoryManager* manager;
                // 8 gigs
                RTEST_ASSERT(rtEMM_createMemoryManager(&manager, 8000000000) == rtEErrorCode_SUCCESS);

                struct rtEMMStackAllocator* stackAlloc;
                RTEST_ASSERT(rtEMM_allocateStackAllocator(manager, &stackAlloc, 7000000000) == rtEErrorCode_SUCCESS);
                RTEST_ASSERT(rtEMM_cleanupStackAllocator(&stackAlloc) == rtEErrorCode_SUCCESS);

                RTEST_ASSERT(rtEMM_cleanupMemoryManager(&manager) == rtEErrorCode_SUCCESS);
        }
}
