cmake_minimum_required(VERSION 3.10)
project(game_of_life LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(game_of_life.exe game_of_life.cpp)
