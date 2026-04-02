# Heat Serial Tiling Benchmark Report

## Provenance

- git metadata omitted because the worktree is not clean.

## Host System

- hostname: `metatron`
- logical CPUs from `nproc`: `16`
- architecture: `x86_64`
- CPU model: `11th Gen Intel(R) Core(TM) i7-11800H @ 2.30GHz`
- threads per core: `2`
- cores per socket: `8`
- sockets: `1`
- NUMA nodes: `1`

## Overview

This report summarizes serial benchmark results for `/home/gjb/Projects/Parallel-programming/source-code/openmp/HeatConduction/build_benchmark_serial/heat_serial_c.exe`.

The benchmark sweep varied:

- grid sizes `-n`: 100, 250, 500, 1000, 2000, 3000, 4000, 5000
- time steps `-t`: `100`
- tile shapes `(-i, -j)`: `(8, 64)`, `(16, 128)`, `(32, 256)`, `(64, 512)`, `(128, 1024)`, plus an untiled full-interior case per grid size
- warmup runs per case: `1`
- measured runs per case: `6`
- timing tool: `/home/gjb/Projects/Parallel-programming/source-code/openmp/HeatConduction/tools/hyperfine/bin/hyperfine`

All tile shapes were validated against a checksum for each grid size.

## Results

### n = 100

Checksum: `1.156797305e+03`

| i block | j block | mean [s] | stddev [s] | min [s] | max [s] | speedup vs first case |
| ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| 8 | 64 | 0.000774 | 0.000126 | 0.000643 | 0.000985 | 1.000 |
| 16 | 98 | 0.000864 | 0.000267 | 0.000566 | 0.001354 | 0.896 |
| 32 | 98 | 0.000750 | 0.000155 | 0.000573 | 0.000921 | 1.031 |
| 64 | 98 | 0.000860 | 0.000102 | 0.000706 | 0.000946 | 0.900 |
| 98 | 98 | 0.000799 | 0.000102 | 0.000693 | 0.000964 | 0.969 |

Best tile shape for this grid size: `-i 32 -j 98` with mean runtime `0.000750 s`.

### n = 250

Checksum: `3.005703950e+03`

| i block | j block | mean [s] | stddev [s] | min [s] | max [s] | speedup vs first case |
| ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| 8 | 64 | 0.003672 | 0.000113 | 0.003505 | 0.003851 | 1.000 |
| 16 | 128 | 0.003290 | 0.000177 | 0.003062 | 0.003484 | 1.116 |
| 32 | 248 | 0.003428 | 0.000191 | 0.003143 | 0.003693 | 1.071 |
| 64 | 248 | 0.003085 | 0.000138 | 0.002961 | 0.003331 | 1.190 |
| 128 | 248 | 0.003319 | 0.000268 | 0.003116 | 0.003827 | 1.107 |
| 248 | 248 | 0.003205 | 0.000170 | 0.002952 | 0.003412 | 1.146 |

Best tile shape for this grid size: `-i 64 -j 248` with mean runtime `0.003085 s`.

### n = 500

Checksum: `6.087215008e+03`

| i block | j block | mean [s] | stddev [s] | min [s] | max [s] | speedup vs first case |
| ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| 8 | 64 | 0.010700 | 0.000600 | 0.010016 | 0.011699 | 1.000 |
| 16 | 128 | 0.010891 | 0.000295 | 0.010560 | 0.011203 | 0.983 |
| 32 | 256 | 0.010608 | 0.000177 | 0.010457 | 0.010862 | 1.009 |
| 64 | 498 | 0.009845 | 0.000304 | 0.009436 | 0.010346 | 1.087 |
| 128 | 498 | 0.009741 | 0.000403 | 0.009191 | 0.010235 | 1.099 |
| 498 | 498 | 0.010038 | 0.000242 | 0.009796 | 0.010411 | 1.066 |

Best tile shape for this grid size: `-i 128 -j 498` with mean runtime `0.009741 s`.

### n = 1000

Checksum: `1.225023713e+04`

| i block | j block | mean [s] | stddev [s] | min [s] | max [s] | speedup vs first case |
| ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| 8 | 64 | 0.038703 | 0.000553 | 0.037786 | 0.039277 | 1.000 |
| 16 | 128 | 0.038585 | 0.000615 | 0.037786 | 0.039254 | 1.003 |
| 32 | 256 | 0.039216 | 0.001025 | 0.038215 | 0.040528 | 0.987 |
| 64 | 512 | 0.039139 | 0.001727 | 0.037583 | 0.042324 | 0.989 |
| 128 | 998 | 0.035225 | 0.000408 | 0.034641 | 0.035683 | 1.099 |
| 998 | 998 | 0.036302 | 0.001345 | 0.035037 | 0.038444 | 1.066 |

Best tile shape for this grid size: `-i 128 -j 998` with mean runtime `0.035225 s`.

### n = 2000

Checksum: `2.457628141e+04`

| i block | j block | mean [s] | stddev [s] | min [s] | max [s] | speedup vs first case |
| ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| 8 | 64 | 0.192217 | 0.013085 | 0.179386 | 0.212737 | 1.000 |
| 16 | 128 | 0.217722 | 0.019583 | 0.190761 | 0.240618 | 0.883 |
| 32 | 256 | 0.238067 | 0.011500 | 0.228093 | 0.254671 | 0.807 |
| 64 | 512 | 0.268538 | 0.010346 | 0.256486 | 0.286638 | 0.716 |
| 128 | 1024 | 0.252460 | 0.007380 | 0.242525 | 0.261684 | 0.761 |
| 1998 | 1998 | 0.212017 | 0.001557 | 0.210114 | 0.213888 | 0.907 |

Best tile shape for this grid size: `-i 8 -j 64` with mean runtime `0.192217 s`.

### n = 3000

Checksum: `3.690232571e+04`

| i block | j block | mean [s] | stddev [s] | min [s] | max [s] | speedup vs first case |
| ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| 8 | 64 | 0.499551 | 0.009724 | 0.491256 | 0.518507 | 1.000 |
| 16 | 128 | 0.501810 | 0.025723 | 0.479901 | 0.543042 | 0.995 |
| 32 | 256 | 0.542268 | 0.019670 | 0.522073 | 0.570726 | 0.921 |
| 64 | 512 | 0.615105 | 0.074094 | 0.564730 | 0.714746 | 0.812 |
| 128 | 1024 | 0.648629 | 0.044989 | 0.601781 | 0.713074 | 0.770 |
| 2998 | 2998 | 0.514301 | 0.025378 | 0.481364 | 0.558671 | 0.971 |

Best tile shape for this grid size: `-i 8 -j 64` with mean runtime `0.499551 s`.

### n = 4000

Checksum: `4.922836993e+04`

| i block | j block | mean [s] | stddev [s] | min [s] | max [s] | speedup vs first case |
| ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| 8 | 64 | 0.932065 | 0.034892 | 0.897529 | 0.988140 | 1.000 |
| 16 | 128 | 0.914727 | 0.035389 | 0.886802 | 0.969924 | 1.019 |
| 32 | 256 | 1.033449 | 0.136652 | 0.965263 | 1.311704 | 0.902 |
| 64 | 512 | 1.108813 | 0.134567 | 1.032579 | 1.378248 | 0.841 |
| 128 | 1024 | 1.134861 | 0.063456 | 1.080803 | 1.235489 | 0.821 |
| 3998 | 3998 | 0.961468 | 0.126726 | 0.874453 | 1.212383 | 0.969 |

Best tile shape for this grid size: `-i 16 -j 128` with mean runtime `0.914727 s`.

### n = 5000

Checksum: `6.155441423e+04`

| i block | j block | mean [s] | stddev [s] | min [s] | max [s] | speedup vs first case |
| ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| 8 | 64 | 1.439922 | 0.029533 | 1.411185 | 1.478794 | 1.000 |
| 16 | 128 | 1.422104 | 0.030813 | 1.404919 | 1.484574 | 1.013 |
| 32 | 256 | 1.551248 | 0.009260 | 1.540719 | 1.564872 | 0.928 |
| 64 | 512 | 1.614573 | 0.033431 | 1.591687 | 1.679371 | 0.892 |
| 128 | 1024 | 1.679343 | 0.023724 | 1.661025 | 1.721054 | 0.857 |
| 4998 | 4998 | 1.408078 | 0.030147 | 1.383902 | 1.456814 | 1.023 |

Best tile shape for this grid size: `-i 4998 -j 4998` with mean runtime `1.408078 s`.

## Summary

| n | best i | best j | best mean [s] |
| ---: | ---: | ---: | ---: |
| 100 | 32 | 98 | 0.000750 |
| 250 | 64 | 248 | 0.003085 |
| 500 | 128 | 498 | 0.009741 |
| 1000 | 128 | 998 | 0.035225 |
| 2000 | 8 | 64 | 0.192217 |
| 3000 | 8 | 64 | 0.499551 |
| 4000 | 16 | 128 | 0.914727 |
| 5000 | 4998 | 4998 | 1.408078 |
