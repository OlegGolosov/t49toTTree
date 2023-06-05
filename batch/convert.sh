#!/bin/bash

system=pbpb
pbeam=40
production=central_pos
postfix=
fileListDir=$PWD/filelists/${system}/${pbeam}agev/${production}
export productionTag=${system}${pbeam}_${production}
export treeDir=/mnt/pool/nica/7/ovgol/na61_data/${system}/${pbeam}agev/${production}/${postfix}
logDir=/mnt/pool/nica/7/ovgol/log

export rootConfig=/mnt/pool/nica/7/parfenovpeter/Soft/root-6-26/install/bin/thisroot.sh
convExe=/mnt/pool/nica/5/ovgol/soft/t49toTTree/build/t49toTTree
executable=./runConverter.sh

mkdir -pv $logDir

mkdir -pv $treeDir/src
cp -v $0 $treeDir/src
cp -v $executable $treeDir/src
cp -v $convExe $treeDir/src
executable=$treeDir/src/$(basename $executable)

export fileList=${treeDir}/src/list
ls $fileListDir/* > $fileList 
nFiles=$(cat $fileList|wc -w)

sbatch -p fast -a 1-$nFiles -o ${logDir}/conv_%A_%a.out $executable

#export SLURM_ARRAY_TASK_ID=1
#$executable
