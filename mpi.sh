#!/bin/bash
#SBATCH --ntasks=2
#SBATCH --partition=cpar

perf stat -r 5 -e L1-dcache-load-misses mpirun -np 4 ./bin/k_means 10000000 32 10