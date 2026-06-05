# Julia OpenMP schedule benchmark on wICE

These scripts benchmark `julia_omp.cpp` with OpenMP runtime schedules for
chunk sizes `64`, `128`, `256`, `512`, and `1024`.

The three Slurm job scripts are job arrays:

- `julia_omp_static.slurm`
- `julia_omp_dynamic.slurm`
- `julia_omp_guided.slurm`

Each array task builds `julia_omp.exe` in node-local scratch when
`VSC_SCRATCH_NODE` is available, runs one schedule/chunk-size pair, and writes a
CSV file under `results/julia_omp_schedule/`.

The benchmark runner sets `JULIA_PRINT_RESULTS=0` so the program times the
kernel without spending allocation time formatting the full matrix after the
timed section.

The default target is wICE Ice Lake:

- `--clusters=wice`
- `--partition=batch_icelake`
- `--cpus-per-task=36`
- `OMP_NUM_THREADS=36`
- `OMP_PROC_BIND=close`
- `OMP_PLACES=cores`

This uses one socket worth of Ice Lake cores. The executable is launched with
`srun --cpu-bind=cores --mem-bind=local` so the thread team and memory placement
stay within the local NUMA domain selected by Slurm.

The scripts charge `lpt2_sysadmin`. To use Sapphire Rapids instead, change the
partition to `batch_sapphirerapids` and `--cpus-per-task` to `48`.

The batch scripts pin these module versions, verified on the `genius` profile:

- `GCC/14.3.0`
- `CMake/3.31.8-GCCcore-14.3.0`

Validate after the repository is available on the cluster. Set
`remote_dir` to the JuliaSet checkout path on wICE; if this local path is also
visible on the cluster, `remote_dir="$PWD"` is sufficient.

```bash
remote_dir=/path/to/JuliaSet

hpc --profile genius submit --test-only slurm/julia_omp_static.slurm \
    --directory "${remote_dir}"
hpc --profile genius submit --test-only slurm/julia_omp_dynamic.slurm \
    --directory "${remote_dir}"
hpc --profile genius submit --test-only slurm/julia_omp_guided.slurm \
    --directory "${remote_dir}"
```

Submit with:

```bash
hpc --profile genius submit slurm/julia_omp_static.slurm \
    --directory "${remote_dir}"
hpc --profile genius submit slurm/julia_omp_dynamic.slurm \
    --directory "${remote_dir}"
hpc --profile genius submit slurm/julia_omp_guided.slurm \
    --directory "${remote_dir}"
```

Optional runtime overrides:

```bash
export JULIA_SIZE=16384
export JULIA_WARMUPS=1
export JULIA_RUNS=5
```
