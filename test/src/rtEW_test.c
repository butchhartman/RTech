#include "rtEErrorCodes/rtEErrorCodes.h"
#include "rtEW/rtenginewindow.h"
#include <assert.h>
#include <rTest/rTest.h>

RTEST_SUITE_BEGIN("rtEW Tests") {

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

        RTEST_CASE("Window's should close state can be queried") {
                // TODO : The should close state will NEVER be changed unless the message loop terminates 
                // or the window is cleaned-up (in which case there is no longer a 'should close' variable to access. Do I need to test for this?
                struct rtEngineWindow* window;
                RTEST_ASSERT(rtEW_createWindow(&window, "Test") == rtEErrorCode_SUCCESS);

                rtEW_showWindow(window);
                RTEST_ASSERT(rtEW_windowShouldClose(window) == false);

                RTEST_ASSERT(rtEW_cleanupWindow(&window) == rtEErrorCode_SUCCESS);
                RTEST_ASSERT(window == nullptr);
        }
        rtEW_cleanup();
}
