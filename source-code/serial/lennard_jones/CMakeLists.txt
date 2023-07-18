cmake_minimum_required(VERSION 3.10)
project(lennard_jones LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wpedantic -Werror")

find_package(Boost REQUIRED COMPONENTS program_options)
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
else()
    message(FATAL_ERROR "Boost not found")
endif()

add_executable(lennard_jones.exe lennard_jones.cpp)
target_link_libraries(lennard_jones.exe ${Boost_LIBRARIES})
