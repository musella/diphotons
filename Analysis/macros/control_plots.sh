#!/bin/bash

src=$1 && shift
www=~/www/exo/phys_14_anv1
[ -n $1 ] && www=$1 && shift

target=$(basename $src)

mkdir  $www/$target/

./basic_plots.py --load basic_plots.json  -O $www/$target/selection  --input-dir $src 
./basic_plots.py --load purity_plots.json -O $www/$target/purity     --input-dir $src 
./eff_plots.py --load eff_plots.json      -O $www/$target/efficiency --input-dir $src 
