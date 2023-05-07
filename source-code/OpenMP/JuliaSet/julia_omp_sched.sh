#!/usr/bin/env bash

JULIA_SIZE=$(( 2**12 ))

echo -n "Serial: "
./julia_serial.exe $JULIA_SIZE > julia_serial.txt

export OMP_NUM_THREADS=8

echo -n "OpenMP default: "
./julia_omp.exe $JULIA_SIZE > julia_omp_default.txt

echo -n "OpenMP static: "
OMP_SCHUDULE=static ./julia_omp.exe $JULIA_SIZE > julia_static_omp.txt

for exponent in 5 6 7 8 9 10
do
    size=$(( 2**$exponent ))
    echo -n "OpenMP dynamic, chunck=$size: "
    OMP_SCHUDULE=dynamic,$size ./julia_omp.exe $JULIA_SIZE > julia_dynamic_omp.txt
done

for exponent in 5 6 7 8 9 10
do
    size=$(( 2**$exponent ))
    echo -n "OpenMP guided, chunk=$size: "
    OMP_SCHUDULE=guided,$size ./julia_omp.exe $JULIA_SIZE > julia_guided_omp.txt
done
