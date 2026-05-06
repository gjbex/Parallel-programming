cmake_minimum_required(VERSION 3.10)
project(julia_set LANGUAGES CXX Fortran)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(julia_set_cpp.exe julia_set.cpp)
add_executable(julia_set_f90.exe julia_set.f90)
