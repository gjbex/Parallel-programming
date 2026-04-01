# Heat conduction

## Purpose

Illustration of parallelizing a real application.


## What is it?

1. `heat_serial.c`: C implementation of the serial starting point.
1. `heat_serial.f90`: Fortran implementation of the serial starting point.
1. `heat.c`: OpenMP version for C.
1. `heat_cancel.c`: OpenMP version for C with cancellation.
1. `heat.f90`: OpenMP version for Fortran.
1. `CMakeLists.txt`: CMake file to build the applications.
