# Pi

## Purpose

Illustration of
* `omp parallel`,
* `omp for`,
* `omp critical`
* the `reduction` clause,


## Tasks

### Part 1


### Part 2



## What is it?

1. `hello_world.c`: C implementation.
1. `hello_world.f90`: Fortran implementation.
1. `hello_world_ordered.c`: C implementation, theads print in order.
1. `hello_world_ordered.f90`: Fortran implementation, threads print in order.
1. `CMakeLists.txt`: CMake file to build the applications.


## How to build manually

To build the applications without CMake, use
```
$ gcc  -fopenmp  hello_world.c  -o hello_world.exe
```
for the C version with OpenMP, or leave out the `fopenmp` flag to build
the serial version.

For the Fortran version, use
```
$ gfortran  -fopenmp  hello_world.c  -o hello_world.exe
```
Leave out the `fopenmp` flag to build the serial version.
