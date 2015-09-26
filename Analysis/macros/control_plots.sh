#!/bin/bash

set -x 

src=$1 && shift
www=~/www/exo/spring15_v850ns
echo $1
[ -n "$1" ] && www=$1 && shift

target=$(basename $src)

[[ ! -d $www ]] && mkdir  $www/
mkdir  $www/$target/

## ./basic_plots.py --load basic_plots.json  -O $www/$target/selection  --input-dir $src 
## ./basic_plots.py --load purity_plots.json -O $www/$target/purity     --input-dir $src 
./eff_plots.py --load eff_plots.json      -O $www/$target/efficiency --input-dir $src 
## ./eff_plots.py --load eff_plots_grav.json    -O $www/$target/efficiency_grav --input-dir $src 

## ./eff_plots.py --load singlepho_eff_plots.json      -O $www/$target/signlepho_efficiency --input-dir $src 
## ./eff_plots.py --load singlepho_eff_plots.json      -O $www/$target/signlepho_efficiency_grav15  --template "%(sample)s/histograms/RSGravToGG_kMpl_001_M_1500_13TeV_%(cat)s%(name)s"  --input-dir $src 
## ./eff_plots.py --load singlepho_eff_plots.json      -O $www/$target/signlepho_efficiency_grav50  --template "%(sample)s/histograms/RSGravToGG_kMpl_001_M_5000_13TeV_%(cat)s%(name)s"  --input-dir $src 

