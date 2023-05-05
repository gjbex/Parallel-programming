# Pi

## Purpose

Illustration of
* `omp parallel`,
* `omp for`,
* `omp critical`
* the `reduction` clause,


## Tasks

### Part 1

Write an application that computes pi using a quadrature method to integrate
the function $1/(1 + x^2)$ over the interval $\[0, 1\]$.  Avoid a race condition
by using an `omp critical`.

### Part 2



## What is it?

1. `pi_serial.c`: C implementation of the serial starting point.
1. `pi_serial.f90`: Fortran implementation of the serial starting point.
1. `pi_critical.c`: C implementation, uses `omp critical`.
1. `pi_bad_critical.c`: C implementation, illustration of how *not* to use `omp critical`.
1. `pi_bad_critical.f90`: Fortran implementation, illustration of how *not* to use `omp critical`.
1. `pi_critical.f90`: Fortran implementation, uses `omp critical`.
1. `CMakeLists.txt`: CMake file to build the applications.
