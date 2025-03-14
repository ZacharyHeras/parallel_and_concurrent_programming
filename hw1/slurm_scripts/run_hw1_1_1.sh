#!/bin/bash
#SBATCH --nodelist=csm-com-010
#SBATCH --job-name=hw1_1_1_job
#SBATCH --output=hw1_1_1_output.txt
#SBATCH --error=hw1_1_1_error.txt
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=1
#SBATCH --mem=0
#SBATCH --time=24:00:00

./hw1 1000 1000
