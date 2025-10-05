cmake_minimum_required(VERSION 4.1.1)

function(addBenchmark file)
        get_filename_component(BENCHMARK_NAME ${file} NAME_WLE)

        message("Configuring benchmark ${BENCHMARK_NAME}")

        project("RTech_Benchmark_${BENCHMARK_NAME}"
                LANGUAGES C)

        set(CMAKE_C_STANDARD 23)
        set(CMAKE_C_STANDARD_REQUIRED True)

        add_executable(${PROJECT_NAME} ${file})

        target_include_directories(${PROJECT_NAME} PRIVATE ../src)

        add_subdirectory(${CMAKE_SOURCE_DIR}/src ${CMAKE_BINARY_DIR}/RTech_Benchmark_${BENCHMARK_NAME})

        target_compile_options(${PROJECT_NAME} PRIVATE
                $<$<CONFIG:Debug>:-Wall -Wextra -Wpedantic -gdwarf -g -g3 -O0>
                $<$<CONFIG:Release>:-Ofast>)

endfunction()

set(RTECH_ONLY_TEST_SOURCES 1)
addBenchmark(src/rtELog_bench.c)
