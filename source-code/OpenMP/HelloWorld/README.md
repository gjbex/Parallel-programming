# Hello world

## Purpose

Illustration of
* `omp parallel`, the `default`, `private` and `shared` clauses,
* the functions `omp_get_thread_num()` and `omp_get_num_threads()`,
* conditional compilation,
* environment variable `OMP_NUM_THREADS`,
* `omp for` and the `ordered` clause, `omp ordered`.


## Goal

### Part 1

Write an application that can be built even with a compiler that
doesn't support OpenMP.

Each thread of the application should print its thread number, and
thread number 0 should print the total number of threads.

If the application is built without OpenMP, the number of threads is 1,
and the thread number is 0.

### Part 2

You will notice that the order in which the threads print their number is
non-deterministic.  Write a version of this application so that he threads
print their number in order.


## What is it?

1. `hello_world_serial.c`: original serial implementation in C.
1. `hello_world_serial.f90`: original serial implementation in Fortran.
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
