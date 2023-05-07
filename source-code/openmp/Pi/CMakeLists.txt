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

add_executable(pi_race_condition_c.exe
    pi_race_condition.c)
add_executable(pi_critical_c.exe
    pi_critical.c)
add_executable(pi_bad_critical_c.exe
    pi_bad_critical.c)
add_executable(pi_reduction_c.exe
    pi_reduction.c)
if(OpenMP_C_FOUND)
    target_link_libraries(pi_race_condition_c.exe PUBLIC OpenMP::OpenMP_C)
    target_link_libraries(pi_critical_c.exe PUBLIC OpenMP::OpenMP_C)
    target_link_libraries(pi_bad_critical_c.exe PUBLIC OpenMP::OpenMP_C)
    target_link_libraries(pi_reduction_c.exe PUBLIC OpenMP::OpenMP_C)
endif()

# Fortran applications
add_executable(pi_serial_f90.exe
    pi_serial.f90)

add_executable(pi_race_condition_f90.exe
    pi_race_condition.f90)
add_executable(pi_critical_f90.exe
    pi_critical.f90)
add_executable(pi_bad_critical_f90.exe
    pi_bad_critical.f90)
add_executable(pi_reduction_f90.exe
    pi_reduction.f90)
if(OpenMP_Fortran_FOUND)
    target_link_libraries(pi_race_condition_f90.exe PUBLIC OpenMP::OpenMP_Fortran)
    target_link_libraries(pi_critical_f90.exe PUBLIC OpenMP::OpenMP_Fortran)
    target_link_libraries(pi_bad_critical_f90.exe PUBLIC OpenMP::OpenMP_Fortran)
    target_link_libraries(pi_reduction_f90.exe PUBLIC OpenMP::OpenMP_Fortran)
endif()
