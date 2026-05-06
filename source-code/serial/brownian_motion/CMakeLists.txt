cmake_minimum_required(VERSION 3.10)
project(brownian_motion LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Boost COMPONENTS program_options REQUIRED)
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
else()
    message(FATAL_ERROR "Boost not found")
endif()

add_executable(brownian_motion.exe brownian_motion.cpp)
target_link_libraries(brownian_motion.exe ${Boost_LIBRARIES})
