#!/bin/bash
if [ -z "$SLURM_ARRAY_TASK_ID" ] 
  then I=0
  else
  I=$SLURM_ARRAY_TASK_ID
fi
n=$1
c=$2
mkdir $I
cd $I
echo "SLURM_JOB_NODELIST"=$SLURM_JOB_NODELIST  > ../log$I.out
cp ../$c .
../oniaVsMult $n 1 0 $c onia $I 
mv ../log$I.* .
