#!/bin/bash
#SBATCH --nodes=1
#SBATCH --cpus-per-task=2
#SBATCH -p classc
#SBATCH -A sxg125_csds438


export OMP_NUM_THREADS=2

gcc -std=c99 -fopenmp quickopenmp.c -o quickOMP
time ./quickOMP

