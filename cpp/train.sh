#!/bin/bash

# $1 = patNum
# $2 = coarseness
# $3 = number of trees
# $4 = depth
export SGE_LOG_PATH=/data/vision/polina/projects/cardiac/courtney/SGELogs/

QR_PATH=/data/vision/polina/shared_software/sge/qsub-run
DATA_PATH=demo/data/regression/
TRAIN_PATH="${DATA_PATH}train_without${1}_${2}.txt"
TEST_PATH="${DATA_PATH}pat${1}_${2}.txt"
DIR=data/vision/polina/projects/cardiac/courtney/Sherwood-ckguo/cpp

$QR_PATH bin/linux/sw regression $TRAIN_PATH $TEST_PATH $DIR "pat${1}_${2}" /P $1 /T $3 /D $4 /verbose
