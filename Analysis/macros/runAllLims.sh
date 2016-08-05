#!/bin/bash

dir=$1 && shift
coups="001 01 02"

methods="Asymptotic ProfileLikelihood"
blind=""
cpu=4
echo $@
set -x
while [[ $1 == -* ]]; do
    case $1 in 
	-k) 
	    coups=$(echo $2 | tr ',' ' ')
	    shift
	    ;;
	-j)
	    cpu=$2
	    shift
	    ;;
	--unblind)
	    blind=""
	    ;;
	--blind)
	    blind="1"
	    ;;
	-M)
	    methods="^$(echo $2 | sed  's%,%$|^%g')\$"
	    shift
	    ;;
	*)
	    opts="$opts $1"
	    if [[ $2 != --* ]]; then 
		opts="$opts $2"
		shift
	    fi
	    ;;
    esac
    shift
done

echo $opts


run=expected
if [[ -z "$blind" ]]; then
    run=both
    if ( echo ProfileLikelihood | grep $methods ); then
	parallel -j $cpu --ungroup "./combineall.sh $dir {} -M ProfileLikelihood  --pvalue --significance $opts | egrep -v '(Significance|p-value)'" ::: $coups &
    fi
fi


if ( echo Asymptotic | grep $methods ); then
    parallel -j $cpu --ungroup "./combineall.sh $dir {} -M Asymptotic --run $run $opts | egrep -v 'Observed'" ::: $coups &
fi

if ( echo MaxLikelihoodFit | grep $methods ); then
    parallel -j $cpu --ungroup "./combineall.sh $dir {} -M MaxLikelihoodFit $opts --mass-in-label" ::: $coups &
fi

## parallel -j $cpu --ungroup "./combineall.sh $dir {} -M Asymptotic --run $run --minimizerStrategy 2 $opts | egrep -v 'Observed'" ::: $coups &
## parallel -j $cpu --ungroup "./combineall.sh $dir {} -M Asymptotic --run $run --hadd  --rMax 60 $opts | egrep -v 'Observed'" ::: $coups &
## parallel -j $cpu --ungroup "./combineall.sh $dir {} -M Asymptotic --run $run --hadd $opts | egrep -v 'Observed'" ::: $coups &
## parallel -j $cpu --ungroup "./combineall.sh $dir {} -M Asymptotic --run $run --minimizerStrategy 2 --rMax 60 $opts | egrep -v 'Observed'" ::: $coups &



wait
    
