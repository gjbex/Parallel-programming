cmake_minimum_required(VERSION 3.9)

project(julia_set LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS NO)

add_compile_options(-Wall -Wextra)

find_package(OpenMP REQUIRED)

add_executable(julia_serial.exe
    julia_serial.cpp)

add_executable(julia_omp.exe
    julia_omp.cpp)
add_executable(julia_tasks_omp.exe
    julia_tasks_omp.cpp)
if(OpenMP_CXX_FOUND)
    target_link_libraries(julia_omp.exe PUBLIC OpenMP::OpenMP_CXX)
    target_link_libraries(julia_tasks_omp.exe PUBLIC OpenMP::OpenMP_CXX)
endif()
