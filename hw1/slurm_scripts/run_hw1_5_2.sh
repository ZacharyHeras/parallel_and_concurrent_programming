#!/bin/bash
#SBATCH --nodelist=csm-com-003
#SBATCH --job-name=hw1_5_2_job
#SBATCH --output=hw1_5_2_output.txt
#SBATCH --error=hw1_5_2_error.txt
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=1
#SBATCH --mem=0
#SBATCH --time=24:00:00

./hw1 10000 10000
