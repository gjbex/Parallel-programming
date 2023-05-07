cmake_minimum_required(VERSION 3.9)

project(julia_set LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS NO)

add_compile_options(-Wall -Wextra)

find_package(TBB REQUIRED)

add_executable(julia_serial.exe
    julia_serial.cpp)

add_executable(julia_tbb.exe
    julia_tbb.cpp)
if(TBB_FOUND)
    target_link_libraries(julia_tbb.exe PUBLIC TBB::tbb)
endif()
