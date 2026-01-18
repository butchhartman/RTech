#ifndef UNICODE
        #define UNICODE
#endif
#include "rtEErrorCodes/rtEErrorCodes.h"
#include <assert.h>
#include "rtELog/rtELog.h"
#include <wchar.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <Windows.h>
#include <wingdi.h>
#include <winuser.h>
#include <windef.h>
#include <WindowsX.h>
#include "rtEW/rtenginewindow.h"
#include "rtEMemoryManager/structs/rtEAllocatorProcs.h"

#define RTEW_WINDOW_CLASS_NAME L"RTEW_WindowClass"

// Global default state for RTEW_GLOBAL_alloc
struct rtEAllocatorProcs RTEW_GLOBAL_alloc = {
        .rtEA_malloc = rtEA_mallocDefault,
        .rtEA_free = rtEA_freeDefault,
        .usr = nullptr
};

enum rtEErrorCode rtEW_setAllocator(struct rtEAllocatorProcs alloc) {
        RTEW_GLOBAL_alloc = alloc;
        return rtEErrorCode_SUCCESS;
}



struct rtEngineWindow{
        char* windowTitle;
        size_t windowTitleLength;

        int windowWidth;
        int windowHeight;

        bool shouldClose;

        // win32
        HWND windowHandle;
        HANDLE msgLoopThread;
        HANDLE shouldCloseMutex;
        HDC DC;

        inputCallback inputCB;
};

struct windowAndSemaphore{
        struct rtEngineWindow* window;
        HANDLE semaphore;
        bool workerThreadSuccess;
};

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

DWORD rtEW_workerThread_runWin32Processes(void* windowAndSemaphore);

static void updateWindowSize(struct rtEngineWindow* window) {
        RECT clientRect;

        GetClientRect(window->windowHandle, &clientRect);

        window->windowWidth = clientRect.right;
        window->windowHeight = clientRect.bottom;
        rtELog_debug_logInfo("WINDOW HEIGHT: %d\n WINDOW WIDTH: %d\n", clientRect.bottom, clientRect.right);

}

static enum rtEErrorCode registerWindowClass() {
        // should only ever be called once
        // TODO: add checking to ensure it is only called once?
        WNDCLASS rtEWWindowClass = {};
        
        rtEWWindowClass.lpfnWndProc = WindowProc;
        rtEWWindowClass.hInstance = GetModuleHandle(NULL);
        rtEWWindowClass.lpszClassName = RTEW_WINDOW_CLASS_NAME;

        ATOM identifier = RegisterClass(&rtEWWindowClass);
        // TODO: Replace with a more relevant error code
        rtELog_logInfo("Registered window class");
        return (identifier != 0) ? rtEErrorCode_SUCCESS : rtEErrorCode_MEMORY_ALLOC_FAILURE;
}

static enum rtEErrorCode initializeWindowMemory(struct rtEngineWindow** window, const char* windowTitle) {
        *window = nullptr;
        // +1 for null terminator. I noticed this when analyzing the memory using my dump file thing.
        // Glad I caught it now instead of in 30 hours when I'm too afraid to touch anything
        // This also caused me to go on a multiple hour long goose hunt of why this change broke everything
        
        // It was alignment. It's always alignment.
        size_t titleLen = strlen(windowTitle) + 1;

        *window = RTEW_GLOBAL_alloc.rtEA_malloc(sizeof(struct rtEngineWindow), RTEW_GLOBAL_alloc.usr);

        if (*window == nullptr) {
                rtELog_logError("Failed to initialize window memory");
                return rtEErrorCode_MEMORY_ALLOC_FAILURE;
        }

        (*window)->windowTitle = RTEW_GLOBAL_alloc.rtEA_malloc(titleLen * sizeof(char), RTEW_GLOBAL_alloc.usr);

        if ((*window)->windowTitle == nullptr) {
                RTEW_GLOBAL_alloc.rtEA_free((void**)window, RTEW_GLOBAL_alloc.usr);
                rtELog_logError("Failed to initialize window memory");
                return rtEErrorCode_MEMORY_ALLOC_FAILURE;
        }

        (*window)->windowTitleLength = titleLen;
        (*window)->shouldClose = false;

        memcpy((*window)->windowTitle, windowTitle, titleLen);

        return rtEErrorCode_SUCCESS;
}

static enum rtEErrorCode initWindowAndSemaphoreStruct(struct windowAndSemaphore* windowAndSemaphore, struct rtEngineWindow* window) {
        windowAndSemaphore->window = window;
        windowAndSemaphore->workerThreadSuccess = true;
        windowAndSemaphore->semaphore = CreateSemaphore(
                NULL,
                0,
                1,
                NULL
        );

        if (windowAndSemaphore->semaphore == nullptr) {
                rtELog_logError("Failed to create windowANDSemaphore struct");
                return rtEErrorCode_MEMORY_ALLOC_FAILURE;
        }

        return rtEErrorCode_SUCCESS;
}

static enum rtEErrorCode createWin32WorkerThread(struct rtEngineWindow* window, struct windowAndSemaphore* windowAndSemaphore) {
        window->msgLoopThread = CreateThread(
                NULL,
                0,
                rtEW_workerThread_runWin32Processes,
                windowAndSemaphore,
                0, // Thread runs immediately
                NULL
        );

        if (window->msgLoopThread == nullptr) {
                CloseHandle(windowAndSemaphore->semaphore);
                rtELog_logError("Failed to create window thread");
                return rtEErrorCode_MEMORY_ALLOC_FAILURE;
        }

        // Waits for the dispatched worker thread to finish window creation and signal the semaphore
        rtELog_debug_logInfo("Waiting for worker thread to signal semaphore");
        WaitForSingleObject(windowAndSemaphore->semaphore, INFINITE);
        rtELog_debug_logInfo("Worker thread signaled semaphore, continuing");

        // TODO: This function can fail, consider error checking in some form
        CloseHandle(windowAndSemaphore->semaphore);

        if (windowAndSemaphore->workerThreadSuccess != true) {
                // TODO: More relevant error code 
                CloseHandle(window->msgLoopThread);
                rtELog_logError("Worker thread did not report success");
                return rtEErrorCode_MEMORY_ALLOC_FAILURE;
        }

        return rtEErrorCode_SUCCESS;
}

static enum rtEErrorCode dispatchWin32WorkerThread(struct rtEngineWindow* window) {
        // TODO: Relevant error codes
        enum rtEErrorCode err;

        struct windowAndSemaphore windowThread_workerAndSemaphore;
        err = initWindowAndSemaphoreStruct(&windowThread_workerAndSemaphore, window); 

        if(err != rtEErrorCode_SUCCESS) {
                return err;
        }

        err = createWin32WorkerThread(window, &windowThread_workerAndSemaphore);

        if(err != rtEErrorCode_SUCCESS) {
                return err;
        }

        return rtEErrorCode_SUCCESS;
}

static enum rtEErrorCode convertWindowTitleToUnicode(struct rtEngineWindow* window, wchar_t** dest) {
        int windowTitleMBCharCount= MultiByteToWideChar(CP_UTF8, 0, window->windowTitle, -1, NULL, 0);
        if (windowTitleMBCharCount <= 0) {
                rtELog_logWarning("Failed to get window title unicode length");
                return rtEErrorCode_MEMORY_ALLOC_FAILURE;
        }

        *dest = RTEW_GLOBAL_alloc.rtEA_malloc(windowTitleMBCharCount * sizeof(wchar_t), RTEW_GLOBAL_alloc.usr);

        if (*dest == nullptr) {
                rtELog_logWarning("Unicode window title memory allocation failed");
                return rtEErrorCode_MEMORY_ALLOC_FAILURE;
        }

        memset(*dest, 0, (windowTitleMBCharCount) * sizeof(wchar_t));

        int convertedChars = MultiByteToWideChar(CP_UTF8, 0, window->windowTitle, -1, *dest, windowTitleMBCharCount);

        if (convertedChars <= 0) {
                RTEW_GLOBAL_alloc.rtEA_free((void**)dest, RTEW_GLOBAL_alloc.usr);
                *dest = nullptr;
                rtELog_logWarning("Failed to convert window title to unicode");
                return rtEErrorCode_MEMORY_ALLOC_FAILURE;
        }

        return rtEErrorCode_SUCCESS;
}

static enum rtEErrorCode createWindowWindowHandle(struct rtEngineWindow* window) {
        enum rtEErrorCode err;
        wchar_t* unicodeWindowName;
        err = convertWindowTitleToUnicode(window, &unicodeWindowName);

        window->windowHandle = CreateWindowEx(
                0,
                RTEW_WINDOW_CLASS_NAME,
                (err == rtEErrorCode_SUCCESS) ? unicodeWindowName : L"err",
                WS_OVERLAPPEDWINDOW, // TODO: this will need to be different for different graphics APIs
                CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

                NULL,
                NULL,
                GetModuleHandle(NULL),
                NULL
        );
        
        window->DC = GetDC(window->windowHandle);
        SetWindowLongPtr(window->windowHandle, GWLP_USERDATA, (LONG_PTR)window);

        if (err == rtEErrorCode_SUCCESS) {
                RTEW_GLOBAL_alloc.rtEA_free((void**)(&unicodeWindowName), RTEW_GLOBAL_alloc.usr);
        }

        if (window->windowHandle == NULL) {
                rtELog_logError("Failed to create window handle");
                return rtEErrorCode_MEMORY_ALLOC_FAILURE;
        }

        // gets window size
        
        updateWindowSize(window);

        return rtEErrorCode_SUCCESS;
}

static enum rtEErrorCode createWindowShouldCloseMutex(struct rtEngineWindow* window) {
        window->shouldCloseMutex = CreateMutex(
                NULL,
                FALSE,
                NULL);

        if (window->shouldCloseMutex == nullptr) {
                rtELog_debug_logError("Failed to create should close mutex");
                return rtEErrorCode_MEMORY_ALLOC_FAILURE;
        }

        return rtEErrorCode_SUCCESS;
}

enum rtEErrorCode rtEW_init() {
        enum rtEErrorCode err = registerWindowClass();
        return err;
}

enum rtEErrorCode rtEW_cleanup() {
        int err =UnregisterClass(RTEW_WINDOW_CLASS_NAME, NULL);
        return (err != 0) ? rtEErrorCode_SUCCESS : rtEErrorCode_MEMORY_ALLOC_FAILURE;
}

enum rtEErrorCode rtEW_createWindow(struct rtEngineWindow** window, const char* windowTitle) {

        enum rtEErrorCode err = initializeWindowMemory(window, windowTitle);

        if (err != rtEErrorCode_SUCCESS) {
                rtELog_debug_logError("Failed to create window");
                return err;
        }


        err = dispatchWin32WorkerThread(*window);

        if (err != rtEErrorCode_SUCCESS) {
                RTEW_GLOBAL_alloc.rtEA_free((void**)&(*window)->windowTitle, RTEW_GLOBAL_alloc.usr);
                RTEW_GLOBAL_alloc.rtEA_free((void**)window, RTEW_GLOBAL_alloc.usr);
                rtELog_debug_logError("Failed to create window");
                return err;
        }


        rtELog_debug_logInfo("Successfully created window");

        return rtEErrorCode_SUCCESS;
}

DWORD rtEW_workerThread_runWin32Processes(void* windowAndSemaphore) {
        enum rtEErrorCode err;
        struct rtEngineWindow* windowPtr = ((struct windowAndSemaphore*)windowAndSemaphore)->window;
        HANDLE semaphore = ((struct windowAndSemaphore*)windowAndSemaphore)->semaphore;
        
        err = createWindowWindowHandle(windowPtr);
        if (err != rtEErrorCode_SUCCESS) {
                ((struct windowAndSemaphore*)windowAndSemaphore)->workerThreadSuccess = false;
                ReleaseSemaphore(semaphore, 1, NULL);
                return EXIT_FAILURE; 
        }
        err = createWindowShouldCloseMutex(windowPtr);
        if (err != rtEErrorCode_SUCCESS) {
                ((struct windowAndSemaphore*)windowAndSemaphore)->workerThreadSuccess = false;
                CloseHandle(windowPtr->windowHandle);
                ReleaseSemaphore(semaphore, 1, NULL);
                return EXIT_FAILURE; 
        }

        // Notifies main thread that it can stop waiting and return the create window function.
        ReleaseSemaphore(semaphore, 1, NULL);
        // Semaphore is void after this line
        // The semaphore AND window struct is also void after this line (heap dealloc)
        rtELog_debug_logInfo("Beginning message Loop");
        MSG msg = { };
        while (GetMessage(&msg, NULL, 0, 0) > 0 && windowPtr->shouldClose == false) {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
        }

        rtELog_debug_logInfo("Ending message Loop");
        
        windowPtr->shouldClose = true;
        return EXIT_SUCCESS;
}


void rtEW_showWindow(struct rtEngineWindow* window) {
        ShowWindow(window->windowHandle, SW_SHOWNORMAL);
        updateWindowSize(window);
}

void rtEW_hideWindow(struct rtEngineWindow* window) {
       ShowWindow(window->windowHandle, SW_HIDE); 
        updateWindowSize(window);
}

enum rtEErrorCode rtEW_cleanupWindow(struct rtEngineWindow** window) {
        struct rtEngineWindow* windowPtr = *window;

        if (windowPtr == nullptr) {
                // TODO: Replace with relevant error code
                return rtEErrorCode_MEMORY_ALLOC_FAILURE;
        }
        if (windowPtr->windowTitle == nullptr) {
                // TODO: Replace with relevant error code
                return rtEErrorCode_MEMORY_ALLOC_FAILURE;
        }
        
        rtELog_log("Waiting for win32 worker thread to exit");
        // TODO:
        // You're really not supposed to use terminate thread, but setting should close did not cause the thread to exit because GetMessage blocks. I dont know what to do as of now, because I can't pump the queue from here since it is on a separate thread.
        windowPtr->shouldClose = true;
        WaitForSingleObject(windowPtr->msgLoopThread, INFINITE);
        rtELog_log("Win32 worker thread exited; freeing resources");
        CloseHandle(windowPtr->msgLoopThread);
        CloseHandle(windowPtr->msgLoopThread);
        CloseHandle(windowPtr->shouldCloseMutex);

        RTEW_GLOBAL_alloc.rtEA_free((void**)&(windowPtr->windowTitle), RTEW_GLOBAL_alloc.usr);
        RTEW_GLOBAL_alloc.rtEA_free((void**)&windowPtr, RTEW_GLOBAL_alloc.usr);

        *window = nullptr;
        return rtEErrorCode_SUCCESS;
}

bool rtEW_windowShouldClose(const struct rtEngineWindow* window) {
        // Does this really need a mutex?
        // I removed it here because the msg loop constantly taking control of the mutex will throttle the main loop speed
        bool returnShouldClose = window->shouldClose;
        return returnShouldClose;
}

void rtEW_setWindowShouldClose(struct rtEngineWindow* window) {
        // Does this really need a mutex?
//        WaitForSingleObject(window->shouldCloseMutex, INFINITE);
        window->shouldClose = true;
 //       ReleaseMutex(window->shouldCloseMutex);
}

void rtEW_setInputCallback(struct rtEngineWindow* window, inputCallback inputCB) {
        window->inputCB = inputCB;
}

#define VK_USE_PLATFORM_WIN32_KHR
#include "rtEW/vulkan/rtEW_VK_createSurface.h"
#include "rtERenderer/vulkan/macros/rtERendererVKMacros.h"

enum VkResult rtEW_VK_createSurface(
        VkSurfaceKHR* dest,
        VkInstance instance,
        const struct rtEngineWindow* window
        ) {

        VkWin32SurfaceCreateInfoKHR info = {
                .sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
                .pNext = nullptr,
                .flags = 0,
                .hinstance = GetModuleHandle(nullptr),
                .hwnd = window->windowHandle,
        };

        VK_ERROR_LOG_AND_RETURN(
                vkCreateWin32SurfaceKHR(instance, &info, nullptr, dest),
                "Failed to create win32 window surface"
                );

        rtELog_debug_logInfo("Created win32 VK surface");
        return VK_SUCCESS;
}
#undef VK_USE_PLATFORM_WIN32_KHR

static void sendKeydownEvent(struct rtEngineWindow* thiswindow, WPARAM wParam) {
        struct inputEvent event = {
                .inputType = RTEW_INPUT_TYPE_KEYBOARD,
                .keystate = RTEW_KEY_DOWN
        };

        switch (wParam) {
                case 'W':
                        event.keycode = RTEW_KEYCODE_W;
                        break;
                case 'A':
                        event.keycode = RTEW_KEYCODE_A;
                        break;
                case 'S':
                        event.keycode = RTEW_KEYCODE_S;
                        break;
                case 'D':
                        event.keycode = RTEW_KEYCODE_D;
                        break;
        }
        
        thiswindow->inputCB(event);
}

static void sendMouseEvent(struct rtEngineWindow* thisWindow, LPARAM lParam) {
        struct inputEvent event = {
                .inputType = RTEW_INPUT_TYPE_MOUSE,
                .mouseXPos = GET_X_LPARAM(lParam) / (float)thisWindow->windowWidth,
                .mouseYPos = GET_Y_LPARAM(lParam) / (float)thisWindow->windowHeight
        };

        thisWindow->inputCB(event);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        struct rtEngineWindow* thiswindow = (struct rtEngineWindow*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

        switch (uMsg) {
                // The microsoft approved way to handle window exiting
                case WM_CLOSE:
                        //if (MessageBox(hwnd, L"Are you sure you want to quit?", L"Confirm", MB_YESNO) == IDYES) {
                                if (DestroyWindow(hwnd) == 0) {
                                        rtELog_debug_logError("HWND destruction failed somehow?");
                                }
                        //}
                        break;
                case WM_DESTROY:
                        PostQuitMessage(0);
                        break;

                case WM_KEYDOWN:
                        sendKeydownEvent(thiswindow, wParam);
                        break;

                case WM_MOUSEMOVE:
                        sendMouseEvent(thiswindow, lParam);
                        break;

                default:
                        return DefWindowProc(hwnd, uMsg, wParam, lParam);
        }

        return 0;
}
