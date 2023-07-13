cmake_minimum_required(VERSION 3.10)
project(julia_set LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(julia_set.exe julia_set.cpp)
