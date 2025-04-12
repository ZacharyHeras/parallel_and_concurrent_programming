#!/bin/bash
#SBATCH --nodelist=csm-com-001
#SBATCH --job-name=hw2_1_job
#SBATCH --output=../slurm_outputs/hw2_1_output.txt
#SBATCH --error=../slurm_outputs/hw2_1_error.txt
#SBATCH --ntasks=1
#SBATCH --mem=0
#SBATCH --time=24:00:00

../source/hw2 1000 1000 1
