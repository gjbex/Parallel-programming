# Pi

## Purpose

Illustration of
* race conditions
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

Adapt your code to have the critical region inside of the iteration.

### Part 3

Use a `reduction` clause rather than `omp critical`.


## What is it?

1. `pi_serial.c`: C implementation of the serial starting point.
1. `pi_serial.f90`: Fortran implementation of the serial starting point.
1. `pi_race_condition.c`: C implementation using OpenMP with a race condition.
1. `pi_race_condition.f90`: Fortran implementation using OpenMP with a race condition.
1. `pi_critical.c`: C implementation, uses `omp critical`.
1. `pi_critical.f90`: Fortran implementation, uses `omp critical`.
1. `pi_bad_critical.c`: C implementation, illustration of how *not* to use `omp critical`.
1. `pi_bad_critical.f90`: Fortran implementation, illustration of how *not* to use `omp critical`.
1. `pi_reduction.c`: C implementation, uses `reduction`.
1. `pi_reduction.f90`: Fortran implementation, uses `reduction`.
1. `CMakeLists.txt`: CMake file to build the applications.
