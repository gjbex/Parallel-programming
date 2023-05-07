# Julia set

## Purpose

Illustration of
* `omp parallel`, `omp for`,
* the `schedule` clause,
# the `OMP_SCHEDULE` environment variable,
* `omp taskloop`


## Tasks

### Part 1

Write an application that computes the julia set and parallelize it using `omp for`
with a `schedule` clause set to `runtime`.  Time with various values of `OMP_SCHEDULE`
to find the most efficient scheduling policy for this application.

### Part 2

Replace the `omp for` with an `omp taskloop` and compare the timings.


## What is it?

1. `julia_serial.c`: implementation of the serial starting point.
1. `julia_omp.cpp.c`: implementation using OpenMP `for` directive.
1. `julia_omp.taskloop_cpp.c`: implementation using OpenMP `taskloop` directive.
1. `CMakeLists.txt`: CMake file to build the applications.
1. `julia_omp_sched.sh`: Bash script to do some timings.
1. `plot_julia.sh`: Bash script to plot the Julia set using gnuplot.
