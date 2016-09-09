#!/bin/bash

## set -x

src1=$1 && shift
src2=$1 && shift
dst=$1 && shift

if ( echo $src1 | grep '=' >/dev/null ); then
    set $(echo $src1 | tr '=' ' ')
    lab1="$1="
    src1="$2"
fi

if ( echo $src2 | grep '=' >/dev/null ); then
    set $(echo $src2 | tr '=' ' ')
    lab2="$1="
    src2="$2"
fi


mkdir $dst

for card1 in $src1/datacard*.txt; do
    binary=$(echo $card1 | sed 's%.txt$%.root%')
    signame=$(echo $card1 | sed 's%.*grav_%%; s%.txt%%')
    set $(echo $signame | tr '_' ' ')
    kmpl=$1
    mass=$2
    
    ## card2="$(echo $src2/datacard*grav_${kmpl}_${mass}*.txt)"
    card2="$(echo $src2/datacard*grav_${kmpl}*.txt)"
    
    ## # ls $card2
    ## if [[ -f $card2 ]]; then
    ## combineCards.py $PWD/$card1 $PWD/$card2 > $dst/datacard_${dst}_grav_${kmpl}_${mass}.txt
    combineCards.py ${lab1}$PWD/$card1 ${lab2}$PWD/$card2 | sed 's%/afs.*/afs%/afs%' > $dst/datacard_${dst}_grav_${kmpl}_${mass}.txt
    ## fi
done
