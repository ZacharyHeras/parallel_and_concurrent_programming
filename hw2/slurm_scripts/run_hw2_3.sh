#!/bin/bash
#SBATCH --nodelist=csm-com-003
#SBATCH --job-name=hw2_3_job
#SBATCH --output=../slurm_outputs/hw2_3_output.txt
#SBATCH --error=../slurm_outputs/hw2_3_error.txt
#SBATCH --ntasks=1
#SBATCH --mem=0
#SBATCH --time=24:00:00

../source/hw2 1000 1000 4
