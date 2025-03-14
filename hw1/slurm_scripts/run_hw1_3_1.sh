#!/bin/bash
#SBATCH --nodelist=csm-com-004
#SBATCH --job-name=hw1_3_1_job
#SBATCH --output=hw1_3_1_output.txt
#SBATCH --error=hw1_3_1_error.txt
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=1
#SBATCH --mem=0
#SBATCH --time=24:00:00

./hw1 5000 5000
