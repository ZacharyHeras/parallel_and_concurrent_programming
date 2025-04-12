#!/bin/bash
#SBATCH --nodelist=csm-com-001
#SBATCH --job-name=hw2_1_job
#SBATCH --output=hw2_1_output.txt
#SBATCH --error=hw2_1_error.txt
#SBATCH --ntasks=1
#SBATCH --mem=0
#SBATCH --time=24:00:00

./hw2 1000 1000 1
