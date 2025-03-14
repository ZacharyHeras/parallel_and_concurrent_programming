#!/bin/bash
#SBATCH --nodelist=csm-com-008
#SBATCH --job-name=hw1_2_2_job
#SBATCH --output=hw1_2_2_output.txt
#SBATCH --error=hw1_2_2_error.txt
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=1
#SBATCH --mem=0
#SBATCH --time=24:00:00

./hw1 5000 1000
