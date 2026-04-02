cmake_minimum_required(VERSION 3.9)

project(cancellation LANGUAGES C)

add_compile_options(-Wall -Wextra)

find_package(OpenMP)

add_executable(find_c.exe
    find.c)
add_executable(heat_cancellation_c.exe
    heat_cancellation.c)
if(OpenMP_C_FOUND)
    target_link_libraries(find_c.exe PUBLIC OpenMP::OpenMP_C)
    target_link_libraries(heat_cancellation_c.exe PUBLIC OpenMP::OpenMP_C)
endif()
