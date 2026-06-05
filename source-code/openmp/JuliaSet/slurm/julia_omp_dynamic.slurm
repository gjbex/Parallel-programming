#!/bin/bash -l
#SBATCH --job-name=julia-omp-dynamic
#SBATCH --account=lpt2_sysadmin
#SBATCH --clusters=wice
#SBATCH --partition=batch_icelake
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=36
#SBATCH --hint=nomultithread
#SBATCH --distribution=block:block
#SBATCH --time=00:30:00
#SBATCH --array=0-4%5
#SBATCH --output=slurm-%x-%A_%a.out

set -euo pipefail

module purge
module load GCC/14.3.0
module load CMake/3.31.8-GCCcore-14.3.0

export JULIA_OMP_SCHEDULE_KIND=dynamic
bash "${SLURM_SUBMIT_DIR}/slurm/run_julia_omp_benchmark.sh"
