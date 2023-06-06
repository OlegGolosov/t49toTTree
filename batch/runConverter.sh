#!/bin/bash

date
source ${rootConfig}
source ${na49rootConfig}

jobId=$SLURM_ARRAY_TASK_ID
list=$(sed "${jobId}q;d" $fileList)

treeFile=$treeDir/$(basename $list)
treeFile=${treeFile/.list/.tree.root}

echo Processing list: $list
echo Tree file: $treeFile 

cd $treeDir/src
time ./t49toTTree $list $treeFile $productionTag

exit $?
