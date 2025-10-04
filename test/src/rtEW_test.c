#include "rtEErrorCodes/rtEErrorCodes.h"
#include "rtEMemoryManager/rtEMemoryManager.h"
#include "rtEMemoryManager/structs/rtEMemoryManagerStruct.h"
#include "rtEMemoryManager/structs/rtEStackAllocatorStruct.h"
#include "rtEW/rtenginewindow.h"
#include <assert.h>
#include <limits.h>
#include <rTest/rTest.h>
#include <rtEMemoryManager/rtEStackAllocator.h>
#include <string.h>

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

RTEST_SUITE_BEGIN("rtEW Tests") {
/*
        RTEST_CASE("The module can start and cleanup without error") {
                RTEST_ASSERT(rtEW_init() == rtEErrorCode_SUCCESS);

                RTEST_ASSERT(rtEW_cleanup() == rtEErrorCode_SUCCESS);
        }

        RTEST_CASE("The module can start & cleanup repeatedly with no errors") {
                RTEST_ASSERT(rtEW_init() == rtEErrorCode_SUCCESS);

                RTEST_ASSERT(rtEW_cleanup() == rtEErrorCode_SUCCESS);

                RTEST_ASSERT(rtEW_init() == rtEErrorCode_SUCCESS);

                RTEST_ASSERT(rtEW_cleanup() == rtEErrorCode_SUCCESS);

                RTEST_ASSERT(rtEW_init() == rtEErrorCode_SUCCESS);

                RTEST_ASSERT(rtEW_cleanup() == rtEErrorCode_SUCCESS);

                RTEST_ASSERT(rtEW_init() == rtEErrorCode_SUCCESS);

                RTEST_ASSERT(rtEW_cleanup() == rtEErrorCode_SUCCESS);
        }

        rtEW_init(); 

        RTEST_CASE("Windows can be created and cleaned up") {
                struct rtEngineWindow* window;
                RTEST_ASSERT(rtEW_createWindow(&window, "Test") == rtEErrorCode_SUCCESS);
                RTEST_ASSERT(window != nullptr);

                RTEST_ASSERT(rtEW_cleanupWindow(&window) == rtEErrorCode_SUCCESS);
                RTEST_ASSERT(window == nullptr);
        }

        RTEST_CASE("Window can be shown and hidden to user") {
                struct rtEngineWindow* window;
                RTEST_ASSERT(rtEW_createWindow(&window, "Test") == rtEErrorCode_SUCCESS);

                rtEW_showWindow(window);
                rtEW_hideWindow(window);
                rtEW_showWindow(window);
                rtEW_hideWindow(window);
                rtEW_showWindow(window);
                rtEW_hideWindow(window);

                RTEST_ASSERT(rtEW_cleanupWindow(&window) == rtEErrorCode_SUCCESS);
                RTEST_ASSERT(window == nullptr);
        }

        RTEST_CASE("Windows should close state can be queried") {
                struct rtEngineWindow* window;
                RTEST_ASSERT(rtEW_createWindow(&window, "Test") == rtEErrorCode_SUCCESS);

                rtEW_showWindow(window);
                RTEST_ASSERT(rtEW_windowShouldClose(window) == false);

                RTEST_ASSERT(rtEW_cleanupWindow(&window) == rtEErrorCode_SUCCESS);
                RTEST_ASSERT(window == nullptr);
        }

        RTEST_CASE("Windows should close state can be set") {
                struct rtEngineWindow* window;
                RTEST_ASSERT(rtEW_createWindow(&window, "Test") == rtEErrorCode_SUCCESS);

                rtEW_showWindow(window);
                RTEST_ASSERT(rtEW_windowShouldClose(window) == false);

                rtEW_setWindowShouldClose(window);
                RTEST_ASSERT(rtEW_windowShouldClose(window) == true);
                while (!rtEW_windowShouldClose(window)) {

                }

                RTEST_ASSERT(rtEW_cleanupWindow(&window) == rtEErrorCode_SUCCESS);
                RTEST_ASSERT(window == nullptr);
        }

        rtEW_cleanup();
*/
        RTEST_CASE("A custom allocator can be used for rtEW allocations") {
                struct rtEMemoryManager* manager;
                struct rtEMMStackAllocator* stallocator;
                RTEST_ASSERT(rtEMM_createMemoryManager(&manager, 5000) == rtEErrorCode_SUCCESS);
                RTEST_ASSERT(rtEMM_allocateStackAllocator(manager, &stallocator, 4000) == rtEErrorCode_SUCCESS);

               // char* string = customMalloc(25, stallocator);
               // strcpy(string, "contains 25 chars.......");
                int* fart = customMalloc(sizeof(int), stallocator);
                *fart = INT_MAX;


                struct rtEW_Allocator allocator = {
                        .rtEW_malloc = customMalloc,
                        .rtEW_free = customFree,
                        .usr = (void*)(stallocator)
                };

                rtEW_setAllocator(allocator);
                rtEW_init();
                struct rtEngineWindow* windows;
                RTEST_ASSERT(rtEW_createWindow(&windows, "beep") == rtEErrorCode_SUCCESS);
                rtEMM_dumpBuffer(stallocator);

                rtEW_showWindow(windows);
                RTEST_ASSERT(rtEW_windowShouldClose(windows) == false);

                rtEW_setWindowShouldClose(windows);
                RTEST_ASSERT(rtEW_windowShouldClose(windows) == true);
                while (!rtEW_windowShouldClose(windows)) {

                }

                RTEST_ASSERT(rtEW_cleanupWindow(&windows) == rtEErrorCode_SUCCESS);
                RTEST_ASSERT(windows == nullptr);

                RTEST_ASSERT(rtEMM_cleanupStackAllocator(&stallocator) == rtEErrorCode_SUCCESS);
                RTEST_ASSERT(rtEMM_cleanupMemoryManager(&manager) == rtEErrorCode_SUCCESS);
        }

        rtEW_cleanup();
}
