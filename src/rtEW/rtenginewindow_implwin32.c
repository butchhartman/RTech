#ifndef UNICODE
        #define UNICODE
#include "rtEErrorCodes/rtEErrorCodes.h"
#include <assert.h>
#endif
#include "rtELog/rtELog.h"
#include <wchar.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <Windows.h>
#include "rtEW/rtenginewindow.h"

#define RTEW_WINDOW_CLASS_NAME L"RTEW_WindowClass"
// TODO: Add error checking for Mutex functions

struct rtEngineWindow{
        char* windowTitle;
        size_t windowTitleLength;

        bool shouldClose;

        // win32
        HWND windowHandle;
        HANDLE msgLoopThread;
        HANDLE shouldCloseMutex;
};

struct windowAndSemaphore{
        struct rtEngineWindow* window;
        HANDLE semaphore;
        bool workerThreadSuccess;
};

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

DWORD rtEW_workerThread_runWin32Processes(void* windowAndSemaphore);

static enum rtEErrorCode registerWindowClass() {
        // should only ever be called once
        // TODO: add checking to ensure it is only called once?
        WNDCLASS rtEWWindowClass = {};
        
        rtEWWindowClass.lpfnWndProc = WindowProc;
        rtEWWindowClass.hInstance = GetModuleHandle(NULL);
        rtEWWindowClass.lpszClassName = RTEW_WINDOW_CLASS_NAME;

        ATOM identifier = RegisterClass(&rtEWWindowClass);
        // TODO: Replace with a more relevant error code
        return (identifier != 0) ? rtEErrorCode_SUCCESS : rtEErrorCode_MEMORY_ALLOC_FAILURE;
}

static enum rtEErrorCode initializeWindowMemory(struct rtEngineWindow** window, const char* windowTitle) {
        *window = nullptr;
        size_t titleLen = strlen(windowTitle);

        *window = malloc(sizeof(struct rtEngineWindow));

        if (*window == nullptr) {
                return rtEErrorCode_MEMORY_ALLOC_FAILURE;
        }

        (*window)->windowTitle = malloc(titleLen * sizeof(char));

        if ((*window)->windowTitle == nullptr) {
                free(window);
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
                return rtEErrorCode_MEMORY_ALLOC_FAILURE;
        }

        // Waits for the dispatched worker thread to finish window creation and signal the semaphore
        rtELog_logInfo("Waiting for worker thread to signal semaphore");
        WaitForSingleObject(windowAndSemaphore->semaphore, INFINITE);
        rtELog_logInfo("Worker thread signaled semaphore, continuing");

        // TODO: This function can fail, consider error checking in some form
        CloseHandle(windowAndSemaphore->semaphore);

        if (windowAndSemaphore->workerThreadSuccess != true) {
                // TODO: More relevant error code 
                CloseHandle(window->msgLoopThread);
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
        // Issues were arising because I dont think the window title string was null terminated LOL
        // Since I specified the string length, the count does NOT include the null terminator
        int windowTitleMBCharCount= MultiByteToWideChar(CP_UTF8, 0, window->windowTitle, window->windowTitleLength, NULL, 0);

        if (windowTitleMBCharCount == 0) {
                return rtEErrorCode_MEMORY_ALLOC_FAILURE;
        }

        // Makes space for the NULL terminator, then zeroes the memory
        *dest = malloc((windowTitleMBCharCount + 1) * sizeof(wchar_t));

        if (*dest == nullptr) {
                return rtEErrorCode_MEMORY_ALLOC_FAILURE;
        }

        memset(*dest, 0, (windowTitleMBCharCount + 1) * sizeof(wchar_t));

        //Yet again, does not include NULL terminator. I assume this works because 0 counts as a terminator.
        int convertedChars = MultiByteToWideChar(CP_UTF8, 0, window->windowTitle, window->windowTitleLength, *dest, windowTitleMBCharCount);

        if (convertedChars == 0) {
                free(*dest);
                *dest = nullptr;
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
        
        if (err == rtEErrorCode_SUCCESS) {
                free(unicodeWindowName);
        }

        if (window->windowHandle == NULL) {
                // TODO: Not sure if windowPtr and window occupy the same memory location. 
                return rtEErrorCode_MEMORY_ALLOC_FAILURE;
        }

        return rtEErrorCode_SUCCESS;
}

static enum rtEErrorCode createWindowShouldCloseMutex(struct rtEngineWindow* window) {
        window->shouldCloseMutex = CreateMutex(
                NULL,
                FALSE,
                NULL);

        if (window->shouldCloseMutex == nullptr) {
                return rtEErrorCode_MEMORY_ALLOC_FAILURE;
        }

        return rtEErrorCode_SUCCESS;
}

enum rtEErrorCode rtEW_init() {
        enum rtEErrorCode err = registerWindowClass();
        return err;
}

enum rtEErrorCode rtEW_createWindow(struct rtEngineWindow** window, const char* windowTitle) {

        enum rtEErrorCode err = initializeWindowMemory(window, windowTitle);

        if (err != rtEErrorCode_SUCCESS) {
                return err;
        }

        err = dispatchWin32WorkerThread(*window);

        if (err != rtEErrorCode_SUCCESS) {
                free((*window)->windowTitle);
                free(*window);
                return err;
        }

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
        rtELog_logInfo("Beginning message Loop");
        MSG msg = { };
        while (GetMessage(&msg, NULL, 0, 0) > 0) {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
        }
        
        WaitForSingleObject(windowPtr->shouldCloseMutex, INFINITE);
        windowPtr->shouldClose = true;
        ReleaseMutex(windowPtr->shouldCloseMutex);

        return EXIT_SUCCESS;
}


void rtEW_showWindow(struct rtEngineWindow* window) {
        ShowWindow(window->windowHandle, SW_SHOWNORMAL);
}

void rtEW_hideWindow(struct rtEngineWindow* window) {
       ShowWindow(window->windowHandle, SW_HIDE); 
}

enum rtEErrorCode rtEW_cleanupWindow(struct rtEngineWindow* window) {
        if (window == NULL) {
                // TODO: Replace with relevant error code
                return rtEErrorCode_MEMORY_ALLOC_FAILURE;
        }
        if (window->windowTitle == NULL) {
                // TODO: Replace with relevant error code
                return rtEErrorCode_MEMORY_ALLOC_FAILURE;
        }
        
        rtELog_log("Waiting for win32 worker thread to exit");
        WaitForSingleObject(window->msgLoopThread, INFINITE);
        rtELog_log("Win32 worker thread exited; freeing resources");
        DestroyWindow(window->windowHandle);
        CloseHandle(window->msgLoopThread);
        CloseHandle(window->shouldCloseMutex);

        free(window->windowTitle);
        free(window);

        return rtEErrorCode_SUCCESS;
}

bool rtEW_windowShouldClose(const struct rtEngineWindow* window) {
        WaitForSingleObject(window->shouldCloseMutex, INFINITE);
        bool returnShouldClose = window->shouldClose;
        ReleaseMutex(window->shouldCloseMutex);
        return returnShouldClose;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        switch (uMsg) {
                case WM_DESTROY:
                        PostQuitMessage(0);
                        return 0;
        }
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
