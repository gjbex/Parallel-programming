# Heat Conduction Benchmark Report

## Overview

This report summarizes the benchmark results for the OpenMP C
implementation in [heat.c](/home/gjb/Projects/Parallel-programming/source-code/openmp/HeatConduction/heat.c).

The target benchmark configuration was:

- grid size: `n = 4000`
- time steps: `t = 100`
- tile sizes: `-i 32 -j 3998`

The main benchmark outputs used here are:

- [heat_scaling_66350730.out](/home/gjb/Projects/Parallel-programming/source-code/openmp/HeatConduction/heat_scaling_66350730.out)
- [heat_scaling_66350815.out](/home/gjb/Projects/Parallel-programming/source-code/openmp/HeatConduction/heat_scaling_66350815.out)

The full-node benchmark in
[heat_scaling_66350815.out](/home/gjb/Projects/Parallel-programming/source-code/openmp/HeatConduction/heat_scaling_66350815.out)
is the most complete and is the basis for the conclusions below.

## System

The target system is an Intel Sapphire Rapids node. The hardware notes in
[sapphirerapids_cpuinfo.txt](/home/gjb/Projects/Parallel-programming/source-code/openmp/HeatConduction/sapphirerapids_cpuinfo.txt)
and
[sapphirerapids_mlc.txt](/home/gjb/Projects/Parallel-programming/source-code/openmp/HeatConduction/sapphirerapids_mlc.txt)
show:

- 2 sockets
- 48 physical cores per socket
- 96 physical cores per node
- AVX-512 support
- measured socket/node bandwidth on the order of `400-490 GB/s` in `mlc`

The full-node Slurm run confirmed:

- `Allocated CPUs: 96`
- `Socket 0 core count: 48`
- `Socket 1 core count: 48`

## Correctness

The full-node benchmark script included checksum validation before timing.
All thread counts produced the same checksum:

```text
checksum: 4.922836993e+04
```

This was true for:

- `1, 2, 4, 8, 12, 16, 24, 32, 48`

from
[heat_scaling_66350730.out](/home/gjb/Projects/Parallel-programming/source-code/openmp/HeatConduction/heat_scaling_66350730.out),
and for the corresponding validation runs in
[heat_scaling_66350815.out](/home/gjb/Projects/Parallel-programming/source-code/openmp/HeatConduction/heat_scaling_66350815.out).

This indicates that the observed scaling is not caused by a correctness
regression.

## Full-Node Scaling Results

Using the 1-thread baseline from
[heat_scaling_66350815.out](/home/gjb/Projects/Parallel-programming/source-code/openmp/HeatConduction/heat_scaling_66350815.out),
the measured runtimes were:

| Threads | Time [ms] | Speedup | Parallel efficiency |
|---:|---:|---:|---:|
| 1  | 923.7 | 1.000 | 1.000 |
| 2  | 560.8 | 1.647 | 0.824 |
| 4  | 409.1 | 2.258 | 0.564 |
| 8  | 341.9 | 2.702 | 0.338 |
| 12 | 325.3 | 2.839 | 0.237 |
| 16 | 221.1 | 4.177 | 0.261 |
| 24 | 112.8 | 8.189 | 0.341 |
| 32 | 59.1  | 15.630 | 0.488 |
| 48 | 49.9  | 18.511 | 0.386 |
| 64 | 52.1  | 17.729 | 0.277 |
| 96 | 32.3  | 28.598 | 0.298 |

## Placement Sensitivity

The benchmark also compared compact placement on one socket with spread
placement across both sockets.

| Case | Time [ms] | Speedup | Parallel efficiency |
|---|---:|---:|---:|
| 8 threads, one socket | 341.1 | 2.708 | 0.339 |
| 8 threads, both sockets | 177.0 | 5.219 | 0.652 |
| 16 threads, one socket | 221.4 | 4.172 | 0.261 |
| 16 threads, both sockets | 104.8 | 8.814 | 0.551 |

These numbers show that placement is a first-order effect for this
kernel. Spreading threads across sockets is much better than packing
them into one socket.

## Interpretation

### Kernel characteristics

The stencil update has low arithmetic intensity. Per interior point, the
kernel performs roughly:

- 4 floating-point loads
- 1 floating-point store
- about 4 FLOP

This implies a memory-bound kernel. The expected behavior is therefore:

- sublinear strong scaling
- strong dependence on socket bandwidth
- much stronger sensitivity to memory placement than to peak FLOP rate

### Observed behavior

The benchmark results match that overall picture:

- early scaling up to a handful of threads is moderate
- using more cores across both sockets gives large gains
- the best measured point in the current benchmark is `96` threads
- one-socket placement underutilizes available bandwidth

The rise in efficiency between some thread counts is unusual for a simple
strong-scaling curve, but the placement experiments explain much of it:
the runtime improves sharply when the job starts making better use of the
second socket.

## Conclusions

1. The code is numerically consistent across thread counts for the tested
   configuration.
2. The kernel is bandwidth-bound, not compute-bound.
3. On Sapphire Rapids, using both sockets matters a lot.
4. One-socket placement is clearly inferior for this stencil.
5. The current best tested launch point is:

   ```bash
   OMP_NUM_THREADS=96 ./heat_c.exe -n 4000 -t 100 -i 32 -j 3998
   ```

6. For moderate thread counts, explicit spread across sockets is more
   effective than compact placement.

## Recommended Next Steps

1. Add a benchmark sweep for `-i 64` in addition to `-i 32`.
2. Compare `OMP_PROC_BIND=spread` against the explicit affinity-based
   placement experiments.
3. Test additional problem sizes to see whether the preferred thread count
   changes with working-set size.
4. If further optimization is needed, try restructuring the inner loop for
   `omp simd` while keeping `j` fully contiguous.
