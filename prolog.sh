#!/bin/bash
/bin/echo $SLURM_JOB_NAME >> /nfs/testing.txt
export SLURM_JOB_SCONTROL_INFO=$(/usr/bin/scontrol show job $SLURM_JOB_ID)
export SLURM_JOB_DIR=$( /usr/bin/scontrol show job $SLURM_JOB_ID | grep -i "WorkDir" | cut -d '=' -f 2-)
export SLURM_JOB_COMMAND=$( /usr/bin/scontrol show job $SLURM_JOB_ID | grep -i "Command" | cut -d '=' -f 2-)
export SLURM_JOB_STDIN=$( /usr/bin/scontrol show job $SLURM_JOB_ID | grep -i "StdIn" | cut -d '=' -f 2-)
export SLURM_JOB_STDERR=$( /usr/bin/scontrol show job $SLURM_JOB_ID | grep -i "StdErr" | cut -d '=' -f 2-)
export SLURM_JOB_STDOUT=$( /usr/bin/scontrol show job $SLURM_JOB_ID | grep -i "StdOut" | cut -d '=' -f 2-)
/bin/echo $SLURM_JOB_DIR > /nfs/jobinfo.txt
/bin/echo $SLURM_JOB_COMMAND >> /nfs/jobinfo.txt
/bin/echo $SLURM_JOB_STDIN >> /nfs/jobinfo.txt
/bin/echo $SLURM_JOB_STDERR >> /nfs/jobinfo.txt
/bin/echo $SLURM_JOB_STDOUT >> /nfs/jobinfo.txt


