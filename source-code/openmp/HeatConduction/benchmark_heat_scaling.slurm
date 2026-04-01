#!/bin/bash -l
#SBATCH --job-name=heat-scaling
#SBATCH --account=lpt2_sysadmin
#SBATCH --time=00:30:00
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=96
#SBATCH --exclusive
#SBATCH --hint=nomultithread
#SBATCH --output=heat_scaling_%j.out
#SBATCH --error=heat_scaling_%j.err

set -euo pipefail

module purge
module load GCC/14.2.0
module load CMake/3.31.3-GCCcore-14.2.0

PROJECT_DIR="${SLURM_SUBMIT_DIR}"
BUILD_DIR="${PROJECT_DIR}/build_slurm"

cd "${PROJECT_DIR}"

cmake -S . -B "${BUILD_DIR}" -DCMAKE_BUILD_TYPE=Release
cmake --build "${BUILD_DIR}" --target heat_c.exe -j "${SLURM_CPUS_PER_TASK}"

THREAD_COUNTS=(1 2 4 8 12 16 24 32 48)
FULL_NODE_THREAD_COUNTS=(1 2 4 8 12 16 24 32 48 64 96)

mapfile -t SOCKET_ZERO_CPUS < <(
    lscpu -p=cpu,socket,core | awk -F, '
        $1 !~ /^#/ && $2 == 0 && !seen[$2 ":" $3]++ { print $1 }
    '
)
mapfile -t SOCKET_ONE_CPUS < <(
    lscpu -p=cpu,socket,core | awk -F, '
        $1 !~ /^#/ && $2 == 1 && !seen[$2 ":" $3]++ { print $1 }
    '
)

join_by_comma() {
    local IFS=,
    echo "$*"
}

cpu_list_prefix() {
    local count="$1"
    shift
    join_by_comma "${@:1:${count}}"
}

interleave_socket_cpus() {
    local count_per_socket="$1"
    local result=()
    local index

    for ((index = 0; index < count_per_socket; index++)); do
        result+=("${SOCKET_ZERO_CPUS[index]}")
        result+=("${SOCKET_ONE_CPUS[index]}")
    done

    join_by_comma "${result[@]}"
}

echo "Benchmark host: $(hostname)"
echo "Start time: $(date)"
echo "Project dir: ${PROJECT_DIR}"
echo "Build dir: ${BUILD_DIR}"
echo "Allocated CPUs: ${SLURM_CPUS_PER_TASK}"
echo "Socket 0 core count: ${#SOCKET_ZERO_CPUS[@]}"
echo "Socket 1 core count: ${#SOCKET_ONE_CPUS[@]}"
echo

echo "Checksum validation runs"
for threads in "${THREAD_COUNTS[@]}"; do
    echo "=== OMP_NUM_THREADS=${threads} checksum ==="
    OMP_PROC_BIND=close \
    OMP_PLACES=cores \
    OMP_NUM_THREADS="${threads}" \
    "${BUILD_DIR}/heat_c.exe" -n 4000 -t 100 -i 32 -j 3998
    echo
done

echo "Thread scaling runs"
for threads in "${FULL_NODE_THREAD_COUNTS[@]}"; do
    echo "=== OMP_NUM_THREADS=${threads} ==="
    OMP_PROC_BIND=close \
    OMP_PLACES=cores \
    OMP_NUM_THREADS="${threads}" \
    hyperfine \
        --warmup 1 \
        --runs 6 \
        "${BUILD_DIR}/heat_c.exe -n 4000 -t 100 -i 32 -j 3998"
    echo
done

echo "Placement sensitivity runs"

SOCKET0_8="$(cpu_list_prefix 8 "${SOCKET_ZERO_CPUS[@]}")"
SOCKET0_16="$(cpu_list_prefix 16 "${SOCKET_ZERO_CPUS[@]}")"
SPREAD_8="$(interleave_socket_cpus 4)"
SPREAD_16="$(interleave_socket_cpus 8)"

echo "=== 8 threads, one socket ==="
GOMP_CPU_AFFINITY="${SOCKET0_8}" \
OMP_NUM_THREADS=8 \
hyperfine \
    --warmup 1 \
    --runs 6 \
    "${BUILD_DIR}/heat_c.exe -n 4000 -t 100 -i 32 -j 3998"
echo

echo "=== 8 threads, both sockets ==="
GOMP_CPU_AFFINITY="${SPREAD_8}" \
OMP_NUM_THREADS=8 \
hyperfine \
    --warmup 1 \
    --runs 6 \
    "${BUILD_DIR}/heat_c.exe -n 4000 -t 100 -i 32 -j 3998"
echo

echo "=== 16 threads, one socket ==="
GOMP_CPU_AFFINITY="${SOCKET0_16}" \
OMP_NUM_THREADS=16 \
hyperfine \
    --warmup 1 \
    --runs 6 \
    "${BUILD_DIR}/heat_c.exe -n 4000 -t 100 -i 32 -j 3998"
echo

echo "=== 16 threads, both sockets ==="
GOMP_CPU_AFFINITY="${SPREAD_16}" \
OMP_NUM_THREADS=16 \
hyperfine \
    --warmup 1 \
    --runs 6 \
    "${BUILD_DIR}/heat_c.exe -n 4000 -t 100 -i 32 -j 3998"
echo

echo "End time: $(date)"
