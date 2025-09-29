if (NOT DEFINED RTECH_ONLY_TEST_SOURCES)
        target_sources(${PROJECT_NAME} PRIVATE main.c)
endif()

add_subdirectory(rtEW)
add_subdirectory(rtELog)
add_subdirectory(rtEMemoryManager)

