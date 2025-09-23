#include "windowing/rtenginewindow.h"
#include <Windows.h>
#include <stdlib.h>
#include <string.h>

struct rtEngineWindow{
        char* windowTitle;
        size_t windowTitleLength;

        bool isActive;
};

struct rtEngineWindow* rtEW_createWindow(const char* windowTitle) {
        size_t titleLen = strlen(windowTitle);

        struct rtEngineWindow* window= malloc(sizeof(struct rtEngineWindow));

        if (window == NULL) {
                return NULL;
        }

        window->windowTitle = malloc(titleLen * sizeof(char));

        if (window->windowTitle == NULL) {
                free(window);
                return NULL;
        }

        window->windowTitleLength = titleLen;
        window->isActive = false;

        memcpy(window->windowTitle, windowTitle, titleLen);
        return window;
}

void rtEW_cleanupWindow(struct rtEngineWindow* window) {
        if (window == NULL) {
                return;
        }
        if (window->windowTitle == NULL) {
                return;
        }

        free(window->windowTitle);
        free(window);
}

inline bool rtEw_windowIsActive(const struct rtEngineWindow* window) {
        return window->isActive;
}

