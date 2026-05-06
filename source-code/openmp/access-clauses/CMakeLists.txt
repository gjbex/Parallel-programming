cmake_minimum_required(VERSION 3.9)

project(access_clauses LANGUAGES Fortran C)

set(CMAKE_Fortran_MODULE_DIRECTORY
    ${CMAKE_BINARY_DIR}/modules)
file(MAKE_DIRECTORY ${CMAKE_Fortran_MODULE_DIRECTORY})

add_compile_options(-Wall -Wextra)

find_package(OpenMP)

# C applications
add_executable(firstprivate.exe
    firstprivate.c)
add_executable(threadprivate.exe
    threadprivate.c)
if(OpenMP_C_FOUND)
    target_link_libraries(firstprivate.exe PUBLIC OpenMP::OpenMP_C)
    target_link_libraries(threadprivate.exe PUBLIC OpenMP::OpenMP_C)
endif()
