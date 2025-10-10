# cmake -S ${source_path} -B ${binary_path} -G ${generator}
# cmake --build ${binary_path} --coonfig ${config}

cmake_minimum_required(VERSION 4.1.1)

option(BUILD_TESTS "Build all test suites" OFF)

project(RTech
        VERSION 0.1.0
        DESCRIPTION "A game about technological progression"
        LANGUAGES C)

set(RTECH_DEVELOPMENT_STAGE "pre-alpha")
# Extracts major.minor.patch versions into separate variables via a bit of list minging. Taken from stack overflow :)
set(STRING_PROJECT_VERSION "${CMAKE_PROJECT_VERSION}")
string(REPLACE "." ";" VERSION_LIST ${STRING_PROJECT_VERSION})
list(GET VERSION_LIST 0 RTECH_VERSION_MAJOR)
list(GET VERSION_LIST 1 RTECH_VERSION_MINOR)
list(GET VERSION_LIST 2 RTECH_VERSION_PATCH)

add_compile_definitions(
        RTECH_VERSION="${CMAKE_PROJECT_VERSION}-${RTECH_DEVELOPMENT_STAGE}" 
        RTECH_APPLICATION_NAME="RTech"
        RTECH_VERSION_MAJOR=${RTECH_VERSION_MAJOR}
        RTECH_VERSION_MINOR=${RTECH_VERSION_MINOR}
        RTECH_VERSION_PATCH=${RTECH_VERSION_PATCH}
        )

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED True)

add_executable(${PROJECT_NAME})

find_package(Vulkan REQUIRED)

target_include_directories(${PROJECT_NAME} PRIVATE 
        src 
        "$ENV{VK_SDK_PATH}/Include"
        )

target_link_libraries(${PROJECT_NAME} PRIVATE Vulkan::Vulkan)

add_subdirectory(src)

target_compile_options(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Debug>:-Wall -Wextra -Wpedantic -gdwarf -g -g3 -O0>)

if (WIN32)
        # adds compiler options to remove the console on windows. the LINKER: prefix makes it portable across compilers
        target_link_options(${PROJECT_NAME} PRIVATE LINKER:/SUBSYSTEM:WINDOWS,/ENTRY:mainCRTStartup)
endif()


if (BUILD_TESTS)
        add_subdirectory(test)
        add_subdirectory(benchmark)
endif()
