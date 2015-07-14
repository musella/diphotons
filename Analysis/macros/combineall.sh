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
	-n)
	    label=$2
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
    cards=datacard*_grav_${coup}_*.txt
    outputs=""
    
    set -x
    for card in $cards; do
	echo $card
	binary=$(echo $card | sed 's%.txt$%.root%')
	signame=$(echo $card | sed 's%.*grav_%%; s%.txt%%')
	set $(echo $signame | tr '_' ' ')
	kmpl=$1
	mass=$2
	if [[ -f $binary ]] && [[ $binary -nt $card ]]; then
	    card=$binary
	fi
	combine -L libdiphotonsUtils $args -n "${label}_k${kmpl}" -m $mass $card >& combine_log_${method}_${kmpl}_${mass}.log 
	
	sleep 1
	tail -5 combine_log_${method}_${kmpl}_${mass}.log 
	outputs="$outputs higgsCombine${label}_k${kmpl}.${method}.mH$mass.root"
    done
    if [[ -n $hadd ]]; then
	hadd -f higgsCombine${label}_k${kmpl}.$method.root $outputs
    fi
done

