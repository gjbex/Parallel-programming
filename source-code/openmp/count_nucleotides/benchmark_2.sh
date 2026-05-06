#!/usr/bin/env bash

benchmark_app="./build/count_nucleotides_2.exe"
dna_file="medium_dna.txt"
if [ ! -e "$dna_file" ]
then
    echo "bench"
    exit 1
fi

# Run the benchmark
hyperfine --warmup 3 --export-csv "benchmark_2.csv" \
    --parameter-list threads 1,2,4,8,12,16 \
    --parameter-list buffer 1000,2000,5000,8000,10000,15000,20000,25000 \
        "OMP_NUM_THREADS={threads} \"$benchmark_app\"  $dna_file {buffer} > /dev/null"

# Plot the results
mamba activate benchmarks &> /dev/null

./create_benchmark_plots.py benchmark.csv
