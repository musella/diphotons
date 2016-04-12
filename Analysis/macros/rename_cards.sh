#!/bin/bash

folder=$1 && shift
coupl=$1 && shift

cd $folder

for card in HighMass-hgg_8TeV_m*.txt; do
    mass=$(echo $card | sed 's%HighMass-hgg_8TeV_m%%; s%\.0.*%%')
    mv -v $card datacard_${folder}_grav_${coupl}_${mass}.txt
done
