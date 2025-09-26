cmake_minimum_required(VERSION 4.1.1)

function(addTestSuite file)
        get_filename_component(TEST_SUITE_NAME ${file} NAME_WLE)

        message("Configuring test suite ${TEST_SUITE_NAME}")

        project("RTech_Test_Suite_${TEST_SUITE_NAME}"
                LANGUAGES C)

        set(CMAKE_C_STANDARD 23)
        set(CMAKE_C_STANDARD_REQUIRED True)

        add_executable(${PROJECT_NAME} ${file})

        target_include_directories(${PROJECT_NAME} PRIVATE ../src)

        add_subdirectory(${CMAKE_SOURCE_DIR}/src ${CMAKE_BINARY_DIR}/RTech_Test_Suite_${TEST_SUITE_NAME})


endfunction()

set(RTECH_ONLY_TEST_SOURCES 1)

addTestSuite(src/rtEW_test.c)

