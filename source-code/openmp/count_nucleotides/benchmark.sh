#!/usr/bin/env bash

benchmark_app="./build/count_nucleotides.exe"
dna_file="medium_dna.txt"
if [ ! -e "$dna_file" ]
then
    echo "bench"
    exit 1
fi
app_args="\"$dna_file\""

# Run the benchmark
hyperfine --warmup 3 --export-csv "benchmark.csv" \
    --parameter-scan threads 1 16 \
        "OMP_NUM_THREADS={threads} \"$benchmark_app\"  $app_args > /dev/null"

# Plot the results
mamba activate benchmarks &> /dev/null

./create_benchmark_plots.py benchmark.csv
