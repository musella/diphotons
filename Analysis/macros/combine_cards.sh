#!/bin/bash

## set -x

src1=$1 && shift
src2=$1 && shift
dst=$1 && shift

mkdir $dst

for card1 in $src1/datacard*.txt; do
    binary=$(echo $card1 | sed 's%.txt$%.root%')
    signame=$(echo $card1 | sed 's%.*grav_%%; s%.txt%%')
    set $(echo $signame | tr '_' ' ')
    kmpl=$1
    mass=$2
    
    card2="$src2/datacard*grav_${kmpl}_${mass}*.txt"
    
    ## # ls $card2
    ## if [[ -f $card2 ]]; then
    ## combineCards.py $PWD/$card1 $PWD/$card2 > $dst/datacard_${dst}_grav_${kmpl}_${mass}.txt
    combineCards.py $PWD/$card1 $PWD/$card2 | sed 's%/afs.*/afs%/afs%' > $dst/datacard_${dst}_grav_${kmpl}_${mass}.txt
    ## fi
done
