#!/bin/bash
/bin/echo $SLURM_JOB_NAME > /nfs/jobinfo.txt
/bin/echo $SLURM_SUBMIT_DIR >> /nfs/jobinfo.txt
/bin/echo $SLURM_JOB_ID >> /nfs/jobinfo.txt
