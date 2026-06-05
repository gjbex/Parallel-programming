#!/usr/bin/env bash

set -euo pipefail

chunk_sizes=(64 128 256 512 1024)
array_id="${SLURM_ARRAY_TASK_ID:-0}"

if (( array_id < 0 || array_id >= ${#chunk_sizes[@]} )); then
    echo "Invalid SLURM_ARRAY_TASK_ID=${array_id}" >&2
    exit 2
fi

repo_dir="${SLURM_SUBMIT_DIR:-$(pwd)}"
schedule_kind="${JULIA_OMP_SCHEDULE_KIND:?set schedule kind}"
chunk_size="${chunk_sizes[$array_id]}"
schedule="${schedule_kind},${chunk_size}"

julia_size="${JULIA_SIZE:-4096}"
warmups="${JULIA_WARMUPS:-1}"
runs="${JULIA_RUNS:-5}"
threads="${SLURM_CPUS_PER_TASK:-36}"

export OMP_NUM_THREADS="${threads}"
export OMP_PROC_BIND="${OMP_PROC_BIND:-close}"
export OMP_PLACES="${OMP_PLACES:-cores}"
export OMP_SCHEDULE="${schedule}"

job_tag="${SLURM_ARRAY_JOB_ID:-local}_${SLURM_ARRAY_TASK_ID:-0}"
scratch_root="${VSC_SCRATCH_NODE:-${TMPDIR:-/tmp}}"
work_dir="${scratch_root}/julia_omp_${job_tag}"
build_dir="${work_dir}/build"
result_dir="${repo_dir}/results/julia_omp_schedule"
result_csv="${result_dir}/${schedule_kind}_${chunk_size}_${job_tag}.csv"

mkdir -p "${work_dir}" "${build_dir}" "${result_dir}"

cleanup() {
    rm -rf "${work_dir}"
}
trap cleanup EXIT

cp "${repo_dir}/CMakeLists.txt" \
   "${repo_dir}/julia_omp.cpp" \
   "${repo_dir}/julia_serial.cpp" \
   "${repo_dir}/julia_tasks_omp.cpp" \
   "${work_dir}/"

echo "Building julia_omp.exe in ${build_dir}"
cmake -S "${work_dir}" -B "${build_dir}" -DCMAKE_BUILD_TYPE=Release
cmake --build "${build_dir}" --target julia_omp.exe -j "${threads}"

{
    echo "# host=$(hostname)"
    echo "# date=$(date --iso-8601=seconds)"
    echo "# slurm_job_id=${SLURM_JOB_ID:-}"
    echo "# slurm_array_job_id=${SLURM_ARRAY_JOB_ID:-}"
    echo "# slurm_array_task_id=${SLURM_ARRAY_TASK_ID:-}"
    echo "# slurm_cpus_per_task=${SLURM_CPUS_PER_TASK:-}"
    echo "# omp_num_threads=${OMP_NUM_THREADS}"
    echo "# omp_proc_bind=${OMP_PROC_BIND}"
    echo "# omp_places=${OMP_PLACES}"
    echo "# omp_schedule=${OMP_SCHEDULE}"
    echo "# julia_size=${julia_size}"
    echo "schedule,chunk_size,threads,julia_size,run,seconds"
} > "${result_csv}"

lscpu > "${result_dir}/lscpu_${job_tag}.txt"
if command -v numactl >/dev/null 2>&1; then
    numactl --hardware > "${result_dir}/numactl_${job_tag}.txt"
fi

run_once() {
    local stderr_file="$1"

    if ! srun --ntasks=1 \
              --cpus-per-task="${threads}" \
              --cpu-bind=cores \
              --mem-bind=local \
              "${build_dir}/julia_omp.exe" "${julia_size}" \
              > /dev/null 2> "${stderr_file}"; then
        cat "${stderr_file}" >&2
        return 1
    fi
}

echo "Warmup runs: ${warmups}"
for (( run = 1; run <= warmups; run++ )); do
    run_once "${work_dir}/warmup_${run}.stderr"
done

echo "Measured runs: ${runs}, OMP_SCHEDULE=${OMP_SCHEDULE}"
for (( run = 1; run <= runs; run++ )); do
    stderr_file="${work_dir}/run_${run}.stderr"
    run_once "${stderr_file}"
    seconds="$(awk '/^time:/ {print $2}' "${stderr_file}")"
    if [[ -z "${seconds}" ]]; then
        echo "Could not parse timing from ${stderr_file}" >&2
        cat "${stderr_file}" >&2
        exit 1
    fi
    printf '%s,%s,%s,%s,%s,%s\n' \
        "${schedule_kind}" "${chunk_size}" "${threads}" "${julia_size}" \
        "${run}" "${seconds}" >> "${result_csv}"
done

echo "Wrote ${result_csv}"
