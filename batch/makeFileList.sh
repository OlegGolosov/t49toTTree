#!/bin/bash

dataPath=$1
listsDir=$2

mkdir -pv $listsDir

#for f in $(cat $dataPath) #common list, e.g. CTA
for f in $dataPath/*.root
do 
  echo $f >> $listsDir/$(echo $f|grep -E '[[:digit:]]{4}' -o).list
done
