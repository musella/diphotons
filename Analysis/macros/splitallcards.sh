#!/bin/bash

# set -x

folder=$1 && shift
cat=$1 && shift

cd $folder

mkdir $cat

cd $cat

parallel -j 8 "combineCards.py --ic=.*$cat.* {} > \$(basename {})" ::: ../*.txt 

ls *.txt