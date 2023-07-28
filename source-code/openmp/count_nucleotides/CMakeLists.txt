cmake_minimum_required(VERSION 3.10)
project(count_nucleotides LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenMP REQUIRED)
find_package(Boost REQUIRED COMPONENTS program_options)
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
endif()

add_executable(count_nucleotides.exe count_nucleotides.cpp)
target_link_libraries(count_nucleotides.exe PRIVATE OpenMP::OpenMP_CXX)

add_executable(count_nucleotides_2.exe count_nucleotides_2.cpp)
target_link_libraries(count_nucleotides_2.exe PRIVATE OpenMP::OpenMP_CXX)

add_executable(generate_dna.exe generate_dna.cpp)
target_link_libraries(generate_dna.exe PRIVATE Boost::program_options)
