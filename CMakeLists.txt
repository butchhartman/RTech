# cmake -S ${source_path} -B ${binary_path} -G ${generator}
# cmake --build ${binary_path} --coonfig ${config}

cmake_minimum_required(VERSION 4.1.1)

project(RTech
        VERSION 1.0.0
        DESCRIPTION "A 3D voxel game about technological progression"
        LANGUAGES C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED True)

add_executable(${PROJECT_NAME} main.c)
