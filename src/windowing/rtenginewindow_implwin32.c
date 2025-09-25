#ifndef UNICODE
        #define UNICODE
#endif

#include <wchar.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <Windows.h>
#include "windowing/rtenginewindow.h"

#define RTEW_WINDOW_CLASS_NAME L"RTEW_WindowClass"

struct rtEngineWindow{
        char* windowTitle;
        size_t windowTitleLength;

        bool shouldClose;

        // win32
        HWND windowHandle;
        HANDLE msgLoopThread;
};

struct windowAndSemaphore{
        struct rtEngineWindow* window;
        HANDLE semaphore;
};

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

DWORD rtEW_workerThread_runWin32Processes(void* windowAndSemaphore);

bool rtEW_init() {
        WNDCLASS rtEWWindowClass = {};
        
        rtEWWindowClass.lpfnWndProc = WindowProc;
        rtEWWindowClass.hInstance = GetModuleHandle(NULL);
        rtEWWindowClass.lpszClassName = RTEW_WINDOW_CLASS_NAME;

        ATOM identifier = RegisterClass(&rtEWWindowClass);
        return (identifier != 0);
}

struct rtEngineWindow* rtEW_createWindow(const char* windowTitle) {
        struct rtEngineWindow* window = nullptr;
        size_t titleLen = strlen(windowTitle);

        window = malloc(sizeof(struct rtEngineWindow));

        if (window == nullptr) {
                return nullptr;
        }

        window->windowTitle = malloc(titleLen * sizeof(char));

        if (window->windowTitle == nullptr) {
                free(window);
                return nullptr;
        }

        window->windowTitleLength = titleLen;
        window->shouldClose = false;

        memcpy(window->windowTitle, windowTitle, titleLen);

        struct windowAndSemaphore windowThread_workerAndSemaphore;
        windowThread_workerAndSemaphore.window = window;
        windowThread_workerAndSemaphore.semaphore = CreateSemaphore(
                NULL,
                0,
                1,
                NULL
        );


        window->msgLoopThread = CreateThread(
                NULL,
                0,
                rtEW_workerThread_runWin32Processes,
                &windowThread_workerAndSemaphore,
                0, // Thread runs immediately
                NULL
        );

        // Waits for the dispatched worker thread to finish window creation and signal the semaphore
        WaitForSingleObject(windowThread_workerAndSemaphore.semaphore, INFINITE);

        // TODO: This function can fail, consider error checking in some form
        CloseHandle(windowThread_workerAndSemaphore.semaphore);

        return window;
}

DWORD rtEW_workerThread_runWin32Processes(void* windowAndSemaphore) {
        struct rtEngineWindow* windowPtr = ((struct windowAndSemaphore*)windowAndSemaphore)->window;
        HANDLE semaphore = ((struct windowAndSemaphore*)windowAndSemaphore)->semaphore;
        
        // Issues were arising because I dont think the window title string was null terminated LOL
        // Since I specified the string length, the count does NOT include the null terminator
        int windowTitleMBCharCount= MultiByteToWideChar(CP_UTF8, 0, windowPtr->windowTitle, windowPtr->windowTitleLength, NULL, 0);

        // Makes space for the NULL terminator, then zeroes the memory
        wchar_t* unicodeWindowName = malloc((windowTitleMBCharCount + 1) * sizeof(wchar_t));
        memset(unicodeWindowName, 0, (windowTitleMBCharCount + 1) * sizeof(wchar_t));

        //Yet again, does not include NULL terminator. I assume this works because 0 counts as a terminator.
        MultiByteToWideChar(CP_UTF8, 0, windowPtr->windowTitle, windowPtr->windowTitleLength, unicodeWindowName, windowTitleMBCharCount);

        windowPtr->windowHandle = CreateWindowEx(
                0,
                RTEW_WINDOW_CLASS_NAME,
                unicodeWindowName,
                WS_OVERLAPPEDWINDOW, // TODO: this will need to be different for different graphics APIs
                CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

                NULL,
                NULL,
                GetModuleHandle(NULL),
                NULL
        );
        
        if (unicodeWindowName != nullptr) {
                free(unicodeWindowName);
        }

        if (windowPtr->windowHandle == NULL) {
                free(windowPtr->windowTitle);
                // TODO: Not sure if windowPtr and window occupy the same memory location. 
                free(windowPtr);
                return 1;
        }

        // Notifies main thread that it can stop waiting and return the create window function.
        ReleaseSemaphore(semaphore, 1, NULL);
        // Semaphore is void after this line
        // The semaphore AND window struct is also void after this line (heap dealloc)

        MSG msg = { };
        while (GetMessage(&msg, NULL, 0, 0) > 0) {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
        }

        windowPtr->shouldClose = true;

        return 0;
}


void rtEW_showWindow(struct rtEngineWindow* window) {
        ShowWindow(window->windowHandle, SW_SHOWNORMAL);
}

void rtEW_hideWindow(struct rtEngineWindow* window) {
       ShowWindow(window->windowHandle, SW_HIDE); 
}

void rtEW_cleanupWindow(struct rtEngineWindow* window) {
        if (window == NULL) {
                return;
        }
        if (window->windowTitle == NULL) {
                return;
        }

        TerminateThread(window->msgLoopThread, 0);
        WaitForSingleObject(window->msgLoopThread, INFINITE);
        DestroyWindow(window->windowHandle);
        CloseHandle(window->msgLoopThread);

        free(window->windowTitle);
        free(window);
}

bool rtEW_windowShouldClose(const struct rtEngineWindow* window) {
        return window->shouldClose;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        switch (uMsg) {
                case WM_DESTROY:
                        PostQuitMessage(0);
                        return 0;
        }
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
