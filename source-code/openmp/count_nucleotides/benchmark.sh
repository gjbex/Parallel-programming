#!/usr/bin/env bash

dna_file="medium_dna.txt"

# Run the benchmark
hyperfine --warmup 3 --export-csv "benchmark.csv" \
    --parameter-scan threads 1 16 \
        "OMP_NUM_THREADS={threads} ./build/count_nucleotides.exe $dna_file > /dev/null"

# Plot the results
mamba activate benchmarks &> /dev/null

./create_benchmark_plots.py benchmark.csv
