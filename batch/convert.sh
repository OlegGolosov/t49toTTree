#!/bin/bash

executable=$1
fileList=$2
productionTag=$3
outDir=$4

. /cvmfs/sft.cern.ch/lcg/app/releases/ROOT/6.16.00/x86_64-centos7-gcc48-opt/bin/thisroot.sh
. /afs/cern.ch/work/o/ogolosov/public/NA49/ROOT_NA49/set49_local.sh

filePath=`cat ${fileList}`
for file in ${filePath}
do
  echo "xrdcp root://castorpublic.cern.ch/${file} ${outDir}/DST/${productionTag}/"
  xrdcp root://castorpublic.cern.ch/${file} ${outDir}/DST/${productionTag}/
done

sed -i -- "s~/castor/cern.ch/na49/mini-dst~${outDir}/DST~g" $fileList 

echo "Running ${executable} ${fileList} ${productionTag} ${outDir}/DT"

${executable} ${fileList} ${productionTag} ${outDir}/DT

filePath=`cat ${fileList}`
for file in $filePath
do
  rm -v $file 
done

echo Done!
