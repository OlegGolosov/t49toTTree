#!/bin/sh

fileList=$1
productionTag=$2
nJobs=$3

[ $# == 0 ] && echo "ERROR!!! NO FILELIST!!!" && exit
[ $# == 1 ] && echo && nJobs=100

workDir=/afs/cern.ch/work/o/ogolosov/public/NA49/converterLogs/$(basename "$fileList")
logDir=$workDir/log
outDir=/eos/user/o/ogolosov/NA49_data

echo workDir=$workDir
echo logDir=$logDir
echo outDir=$outDir
echo fileList=$fileList
echo nJobs=$nJobs

mkdir -p $workDir/fileLists
mkdir -p $workDir/log
mkdir -p $outDir/DST/$productionTag
mkdir -p $outDir/DT
rm -r $workDir/fileLists/*
rm -r $workDir/log/*

split -n l/$nJobs -d -a 3 $fileList $workDir/fileLists/$(basename "$fileList")_

cp -v condor_convert.sub_template condor_convert.sub
sed -i -- "s~WORKDIR~${workDir}~g" condor_convert.sub
sed -i -- "s~OUTDIR~${outDir}~g" condor_convert.sub
sed -i -- "s~PRODUCTIONTAG~${productionTag}~g" condor_convert.sub
      
condor_submit condor_convert.sub
