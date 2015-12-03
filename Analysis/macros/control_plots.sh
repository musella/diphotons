#!/bin/bash

set -x 

data_file=""
sig_file=""
bkg_file=""
lumi=""
target=""
while [[ $1 == -* ]]; do
    echo $1
    case $1 in 
	--data-file) 
	    data_file="$1 $2"
	    [[ -z "$target" ]] && target=$(basename $(dirname $2))
	    echo $target
	    shift
	    ;;
	--sig-file) 
	    sig_file="$1 $2"
	    shift
	    ;;
	--bkg-file) 
	    bkg_file="$1 $2"
	    shift
	    ;;
	-O)
	    target="$2"
	    shift
	    ;;
	--lumi)
	    lumi="$1 $2"
	    shift
	    ;;
	--)
	    shift
	    break
	    ;;
	*)
	    opts="$opts $1"
	    if [[ $2 != "--*" ]]; then 
		opts="$opts $2"
		shift
	    fi
	    ;;
    esac
    shift
done
[[ $1 == --* ]] && shift

## echo $opts

src=$1 && shift
www=~/www/exo/spring15_7415
echo $1
[ -n "$1" ] && www=$1 && shift

[[ -z "$target" ]] && target=$(basename $src)

if [[ $target != "/*" ]]; then
    [[ ! -d $www ]] && mkdir  $www/
    mkdir  $www/$target/
fi

./basic_plots.py --load basic_plots.json  -O $www/$target/selection  --input-dir $src $lumi $data_file $opts
## ./basic_plots.py --load purity_plots.json -O $www/$target/purity     --input-dir $src 
## ./eff_plots.py --load eff_plots.json      -O $www/$target/efficiency --input-dir $src 

## ./eff_plots.py --load eff_plots_grav.json    -O $www/$target/efficiency_grav --input-dir $src 

## ./eff_plots.py --load singlepho_eff_plots.json      -O $www/$target/signlepho_efficiency --input-dir $src
## ./eff_plots.py --load singlepho_eff_plots.json      -O $www/$target/signlepho_efficiency_grav15  --template "%(sample)s/histograms/RSGravToGG_kMpl_001_M_1500_13TeV_%(cat)s%(name)s"  --input-dir $src 
## ./eff_plots.py --load singlepho_eff_plots.json      -O $www/$target/signlepho_efficiency_grav50  --template "%(sample)s/histograms/RSGravToGG_kMpl_001_M_5000_13TeV_%(cat)s%(name)s"  --input-dir $src 

