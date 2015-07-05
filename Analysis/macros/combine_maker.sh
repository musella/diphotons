#!/bin/bash

## version=full_analysis_anv1_v19
version=$1 && shift

fitname=2D 
www=~/www/exo/

shapes="default_shapes"

[[ $1 =~ "^[0-9]+$" ]] && lumi=$1 && shift

opts=""
while [[ -n $1 ]]; do
    case $1 in
	--fit-name)
	    fitname=$2
	    shift
	    ;;
	--www)
	    www=$2
	    shift
	    ;;
	--label)
	    addlabel=$2
	    shift
	    ;;
	--bkg-shapes)
	    shapes=$(echo $(basename $2 | sed 's%.json%%'))
	    opts="$opts $1"
	    ;;
	--nuisance-fractions-covariance)
	    covariance=$(echo $(basename $2 | sed 's%.json%%'))
	    opts="$opts $1"
	    ;;
	--generate-ws-bkgnbias)
	    spurious="bias";
	    opts="$opts $1"
	    ;;
	--fwhm-input-file)
	    fwhm="$2"
	    opts="$opts $1"
	    ;;
	--luminosity)
	    lumi=$2
	    shift
	    ;;
	*)
	    opts="$opts $1"
	    ;;	    
    esac
    shift
done
shift

echo $version $lumi

if [[ -z $version ]] || [[ -z $lumi ]]; then
    echo "usage: $0 <analysis_version> <lumi> [run_options]"
    exit 0
fi

label="$shapes"
[[ -n $covariance ]] && label="${label}_${covariance}"
[[ -n $bias ]] && label="${label}_${bias}"
[[ -n $addlabel ]] && label="${label}_${addlabel}"

input=${version}_${fitname}_final_ws.root
input_log=${version}_${fitname}_final_ws.log
treesdir=~musella/public/workspace/exo/
workdir=${version}_${fitname}_${label}_lumi_${lumi}

if [[ -n $bias ]]; then
    if [[ -z $fwhm ]]; then
	opts="--compute-fwhm"
    fi
fi

mkdir $workdir

mkdir $www/$version

if [[ ! -f $input ]]; then
    echo "**************************************************************************************************************************"
    echo "creating $input"
    echo "**************************************************************************************************************************"
    subset=$fitname
    [[ "$fitname" == "2D" ]] && subset="2D,singlePho"
    ./templates_maker.py --load templates_maker.json,templates_maker_fits.json --only-subset $subset --input-dir $treesdir/$version -o $input 2>&1 | tee $input_log
    echo "**************************************************************************************************************************"
fi

echo "**************************************************************************************************************************"
echo "running model creation"
echo "**************************************************************************************************************************"

./combine_maker.py \
    --fit-name $fitname  --luminosity $lumi  \
    --fit-background \
    --generate-signal \
    --generate-datacard \
    --binned-data-in-datacard \
    --read-ws $input \
    --ws-dir $workdir \
    -O $www/$version/$workdir \
    -o $workdir.root  \
    --cardname datacard_${workdir}.txt $opts 2>&1 | tee $workdir/combine_maker.log

echo "**************************************************************************************************************************"
