cmake_minimum_required(VERSION 4.1.1)

function(addTestSuite file)
        get_filename_component(TEST_SUITE_NAME ${file} NAME_WLE)

        message("Configuring test suite ${TEST_SUITE_NAME}")

        project("RTech_Test_Suite_${TEST_SUITE_NAME}"
                LANGUAGES C)

        set(CMAKE_C_STANDARD 23)
        set(CMAKE_C_STANDARD_REQUIRED True)

        add_executable(${PROJECT_NAME} ${file})

        target_link_libraries(${PROJECT_NAME} PRIVATE Vulkan::Vulkan)

        target_include_directories(${PROJECT_NAME} PRIVATE 
        ../src
        "$ENV{VK_SDK_PATH}/Include"
        )

        add_subdirectory(${CMAKE_SOURCE_DIR}/src ${CMAKE_BINARY_DIR}/RTech_Test_Suite_${TEST_SUITE_NAME})

        target_compile_options(${PROJECT_NAME} PRIVATE
                $<$<CONFIG:Debug>:-Wall -Wextra -Wpedantic -gdwarf -g -g3 -O0>)

endfunction()

set(RTECH_ONLY_TEST_SOURCES 1)

addTestSuite(src/rtEW_test.c)
addTestSuite(src/rtEMemoryManager_test.c)
addTestSuite(src/rtEStackAllocator_test.c)
addTestSuite(src/rtELog_test.c)
addTestSuite(src/rtEMath_test.c)

