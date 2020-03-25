#!/bin/bash
# example use:
# ./submit.sh outputFolder 1000 1000000 settings.cmnd 
# first arguemnt: output folder
# 2nd argument: number of jobs (size of job array)
# 3rd argument: number of events per job
# 4th argument: settings file to use
folder=$1
N=$2
n=$3
c=$4
mkdir TREES/$folder
cd TREES/$folder
cp ../../oniaVsMult .
cp ../../runPythia.sh .
cp ../../$c .
sbatch --time=8:00:00 --mem-per-cpu=3072 --array=1-$N -o log%a.out -e log%a.err runPythia.sh $n $c 
