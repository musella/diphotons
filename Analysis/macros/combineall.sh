#!/bin/bash

# set -x

folder=$1 && shift
coupl=$1 && shift

# echo $coupl

cd $folder

args=""
while [[ -n $1 ]]; do
    case $1 in
	-M)
	    method=$2
	    args="$args $1 $2"
	    shift
	    ;;
	-n)
	    label=$2
	    shift
	    ;;
	-s)
	    seed=$2
	    args="$args $1 $2"
	    shift
	    ;;
	--output)
	    outfolder="$2"
	    shift
	    ;;
	--hadd)
	    hadd="hadd"
	    ;;
	--dry-run)
	    dry="1"
	    ;;
	--cont)
	    cont="1"
	    ;;
	*)	    
	    args="$args $1"
	    ;;	    
    esac
    shift
done
shift

if [[ ! -d $outfolder ]]; then
    mkdir -p $outfolder
fi

libs="-L libdiphotonsUtils"
rootversion=$(root-config --version| tr '.' ' ')
##[[ $rootversion -gt 5 ]] && libs="-L libdiphotonsRooUtils"
for coup in $(echo $coupl | tr ',' ' '); do
    cards=datacard*_grav_${coup}_*.txt
    outputs=""
    if [[ -n $seed ]]; then 
	hout=higgsCombine${label}_k${coup}.$method.$seed.root
    else
	hout=higgsCombine${label}_k${coup}.$method.root
    fi
    
    for card in $cards; do
	echo $card
	binary=$(echo $card | sed 's%.txt$%.root%')
	signame=$(echo $card | sed 's%.*grav_%%; s%.txt%%')
	set $(echo $signame | tr '_' ' ')
	kmpl=$1
	mass=$2
	log=combine_log_${method}_${label}_${kmpl}_${mass}.log
	set -x
	if [[ -n $seed ]]; then 
	    filename=higgsCombine${label}_k${kmpl}.${method}.mH$mass.$seed.root 
	else
	    filename=higgsCombine${label}_k${kmpl}.${method}.mH$mass.root 
	fi
	if [[ -z $dry ]] && ( [[ -z $cont ]] || [[ ! -f $filename ]] ); then 
	    if [[ -f $binary ]] && [[ $binary -nt $card ]]; then
		card=$binary
	    fi
	    echo combine $libs $args -n "${label}_k${kmpl}" -m $mass $card > $log
	    combine $libs $args -n "${label}_k${kmpl}" -m $mass $card 2>&1 | tee -a $log
	    ## sleep 1
	elif [[ -n $outfolder ]]; then
	    filename=$outfolder/$filename
	fi
	set +x
	tail -5 $log 
	[[ -f $filename ]] && outputs="$outputs $filename"
    done
    
    
    if [[ -n $hadd ]]; then
	hadd -f $hout $outputs
	outputs="$hout $outputs"
    fi
    if [[ -n $outfolder ]]; then
	cp -p $outputs $outfolder
    fi
    
done

