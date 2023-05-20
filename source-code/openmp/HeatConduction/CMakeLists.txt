cmake_minimum_required(VERSION 3.9)

project(access_clauses LANGUAGES Fortran C)

set(CMAKE_Fortran_MODULE_DIRECTORY
    ${CMAKE_BINARY_DIR}/modules)
file(MAKE_DIRECTORY ${CMAKE_Fortran_MODULE_DIRECTORY})

add_compile_options(-Wall -Wextra)

find_package(OpenMP)

# C applications
add_executable(heat_serial_c.exe
    heat_serial.c)
add_executable(heat_c.exe
    heat.c)
if(OpenMP_C_FOUND)
   target_link_libraries(heat_c.exe PUBLIC OpenMP::OpenMP_C)
endif()

# Fortran applications
add_executable(heat_serial_f90.exe
    heat_serial.f90)
add_executable(heat_f90.exe
    heat.f90)
if(OpenMP_Fortran_FOUND)
    target_link_libraries(heat_f90.exe PUBLIC OpenMP::OpenMP_Fortran)
endif()
