cmake_minimum_required(VERSION 3.10)
project(ising LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Boost REQUIRED COMPONENTS program_options)
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
else()
    message(FATAL_ERROR "Boost not found")
endif()

add_executable(ising.exe ising.cpp)
target_link_libraries(ising.exe ${Boost_LIBRARIES})
