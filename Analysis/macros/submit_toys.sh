#!/bin/bash

queue=$1 && shift
input=$1 && shift
output=$1 && shift
ntoys=$1 && shift
perjob=1
[ -n $1 ] && perjob=$1 && shift

mkdir $output

for toy in $(seq 0 $perjob $ntoys); do
    bsub -q $queue -o $output/log_$toy.log run_toy.sh $PWD/$input $PWD/$output/toy_$toy.root $toy $perjob $@
done
