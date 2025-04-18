#!/bin/bash
#SBATCH --nodelist=csm-com-007
#SBATCH --job-name=hw2_7_job
#SBATCH --output=../slurm_outputs/hw2_7_output.txt
#SBATCH --error=../slurm_outputs/hw2_7_error.txt
#SBATCH --ntasks=1
#SBATCH --mem=0
#SBATCH --time=24:00:00

../source/hw2 5000 5000 20
