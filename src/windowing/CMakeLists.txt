# todo: add target platform detection that will select the proper source
if (WIN32)
        target_sources(${PROJECT_NAME} PRIVATE rtenginewindow_implwin32.c)
endif()
