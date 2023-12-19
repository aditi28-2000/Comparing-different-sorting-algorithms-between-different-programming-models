#!/bin/bash
#SBATCH -p class --gres=gpu:1 -A sxg125_csds438

module load nvhpc 

#compile
nvc -o merge mergeSort.c

time ./merge
