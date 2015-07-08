#!/bin/bash

# set -x

folder=$1 && shift
coupl=$1 && shift

# echo $coupl

cd $folder

args=""
while [[ -n $1 ]]; do
    echo $1;
    case $1 in
	-M)
	    method=$2
	    args="$args $1 $2"
	    shift
	    ;;
	--hadd)
	    hadd="hadd"
	    ;;
	*)	    
	    args="$args $1"
	    ;;	    
    esac
    shift
done
shift

for coup in $(echo $coupl | tr ',' ' '); do
    cards=datacard*_grav_${coup}_*.root
    outputs=""
    
    set -x
    for card in $cards; do
	echo $card
	signame=$(echo $card | sed 's%.*grav_%%; s%.root%%')
	set $(echo $signame | tr '_' ' ')
	kmpl=$1
	mass=$2
	combine $args -n "_k${kmpl}" -m $mass $card >& combine_log_${method}_${kmpl}_${mass}.log 
	sleep 1
	tail -5 combine_log_${method}_${kmpl}_${mass}.log 
	outputs="$outputs higgsCombine_k${kmpl}.${method}.mH$mass.root"
    done
    if [[ -n $hadd ]]; then
	hadd -f higgsCombine_k${kmpl}.$method.root $outputs
    fi
done

