cmake_minimum_required(VERSION 3.9)

project(pi LANGUAGES Fortran C)

set(CMAKE_Fortran_MODULE_DIRECTORY
    ${CMAKE_BINARY_DIR}/modules)
file(MAKE_DIRECTORY ${CMAKE_Fortran_MODULE_DIRECTORY})

add_compile_options(-Wall -Wextra)

find_package(OpenMP)

# C applications
add_executable(pi_serial_c.exe
    pi_serial.c)

# if(OpenMP_C_FOUND)
#     target_link_libraries(hello_world_c.exe PUBLIC OpenMP::OpenMP_C)
#     target_link_libraries(hello_world_ordered_c.exe PUBLIC OpenMP::OpenMP_C)
# endif()

# Fortran applications
add_executable(pi_serial_f90.exe
    pi_serial.f90)

# if(OpenMP_Fortran_FOUND)
#     target_link_libraries(hello_world_f90.exe PUBLIC OpenMP::OpenMP_Fortran)
#     target_link_libraries(hello_world_ordered_f90.exe PUBLIC OpenMP::OpenMP_Fortran)
# endif()
