#!/bin/bash

# set -x

folder=$1 && shift
coupl=$1 && shift

# echo $coupl

cd $folder

args=""
masses=""
sub=""
while [[ -n $1 ]]; do
    case $1 in
	-M)
	    method=$2
	    args="$args $1 $2"
	    shift
	    ;;
	-m)
	    # masses="^$(echo $2 | sed  's%,%$|^%g')\$"
	    masses="$2"
	    shift
	    ;;
	--parametric)
	    isparametric="1"
	    ;;
	-n)
	    label=$2
	    shift
	    ;;
	--mass-in-label)
	    mass_in_label="1"
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
	--sub)
	    sub="$2"
	    shift
	    ;;
	*)	    
	    args="$args $1"
	    ;;	    
    esac
    shift
done
shift

if [[ -n $masses ]]; then
    if [[ -n $isparametric ]]; then
	masses="$(echo $masses | sed  's%,% %g')"
    else
    	masses="^$(echo $masses | sed  's%,%$|^%g')\$"
    fi
fi

if [[ -n $isparametric ]] && [[ -z $masses ]]; then
    echo "workspace is parametric but no list of masses specified... baliing out"
    exit -1
fi


echo $masses
echo $outfolder

if [[ -n $outfolder ]] && [[ ! -d $outfolder ]]; then
    mkdir -p $outfolder
fi

if [[ -n $sub ]]; then
    cat > env.sh <<EOF
export PATH=$PATH
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH
EOF
fi

libs="-L libdiphotonsUtils"
rootversion=$(root-config --version| tr '.' ' ')
##[[ $rootversion -gt 5 ]] && libs="-L libdiphotonsRooUtils"
for coup in $(echo $coupl | tr ',' ' '); do
    if [[ -n $isparametric ]]; then
	cards=datacard*_grav_${coup}.txt
    else
	cards=datacard*_grav_${coup}_*.txt
    fi
    outputs=""
    if [[ -n $seed ]]; then 
	hout=higgsCombine${label}_k${coup}.$method.$seed.root
    else
	hout=higgsCombine${label}_k${coup}.$method.root
    fi
    
    for card in $cards; do
	# echo $card
	binary=$(echo $card | sed 's%.txt$%.root%')
	signame=$(echo $card | sed 's%.*grav_%%; s%.txt%%')
	set $(echo $signame | tr '_' ' ')
	kmpl=$1
	if [[ $isparametric ]]; then
	    cardmasses=$masses
	else
	    if [[ -n $masses ]]; then
		if  ! ( echo $2 | egrep $masses >/dev/null ) ; then 
		    continue
		fi
	    fi
	    cardmasses=$2
	fi
	for mass in $cardmasses; do
	    log=combine_log_${method}_${label}_${kmpl}_${mass}.log
	    set -x
	    jlabel="${label}_k${kmpl}"
	    if [[ $mass_in_label ]]; then
		jlabel="${jlabel}_${mass}"
	    fi
	    if [[ -n $seed ]]; then 
		filename=higgsCombine${jlabel}.${method}.mH$mass.$seed.root 
	    else
		filename=higgsCombine${jlabel}.${method}.mH$mass.root 
	    fi
	    if [[ -z $dry ]] && ( [[ -z $cont ]] || [[ ! -f $filename ]] ); then 
		if [[ -f $binary ]] && [[ $binary -nt $card ]]; then
		    card=$binary
		fi
		if [[ -z $sub ]]; then 
		    echo combine $libs $args -n $jlabel -m $mass $card > $log
		    combine $libs $args -n $jlabel -m $mass $card 2>&1 | tee -a $log
		else
		    rm $log
		    bsub -o $log -q $sub run.sh -env $PWD/env.sh -copy $filename -outdir $PWD  -- combine $libs $args -n $jlabel -m $mass $PWD/$card
		fi    
		
	    elif [[ -n $outfolder ]]; then
		filename=$outfolder/$filename
	    fi
	    [[ -f $filename ]] && outputs="$outputs $filename"
	    set +x
	    tail -5 $log 
	done
    done
    
    
    if [[ -n $hadd ]]; then
	hadd -f $hout $outputs
	outputs="$hout $outputs"
    fi
    if [[ -n $outfolder ]]; then
	cp -p $outputs $outfolder
    fi
    
done

