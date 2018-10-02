#!/bin/bash

#SBATCH job-name="hello_test"
#SBATCH nodes=1
#CCC STAGE_OUT MySlurm.out mine.out
#CCC STAGE_OUT tfile tfile
#CCC STAGE_IN callCCC tfile

sleep 60

hostname > MySlurm.out
hostname
ls >> MySlurm.out
echo "Hello?"


