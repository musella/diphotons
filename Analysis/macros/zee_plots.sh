#!/bin/bash


data_file=""
sig_file=""
bkg_file=""
lumi=""
target=""
while [[ $1 == -* ]]; do
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



src=$1 && shift
www=~/www/exo/spring15_7415
echo $1
[ -n "$1" ] && www=$1 && shift

[[ -z "$target" ]] && target=$(basename $src)

if [[ $target != "/*" ]]; then
    [[ ! -d $www ]] && mkdir  $www/
    mkdir  $www/$target/
fi

./basic_plots.py --load zee_plots.json  -O $www/$target/selection  --input-dir $src  $lumi $data_file $opts

