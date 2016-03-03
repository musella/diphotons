#!/bin/bash

set -x
mydir=$(dirname $(which $0))

cd $mydir

set +x
eval `scram ru -sh`
cd -

set -x

## full_analysis_anv1_v13/bias_study_toys_from_fit_unbinned.root
input=$1 && shift
output=$1 && shift
toy=$1 && shift
ntoys=$1 && shift

#     --fit-name 2D \


outname=$(basename $output)
outdir=$(dirname $output)

$mydir/bkg_bias.py --n-toys $ntoys \
    --store-new-only \
    --components data --models dijet \
    --read-ws $input  -o $outname \
    --fit-toys \
    --fit-name fit_v0 \
    --saveas png \
    --test-range 200,220 --test-range 220,240 --test-range 240,260 --test-range 260,300 --test-range 300,350 --test-range 350,400 --test-range 400,450 --test-range 450,500 \
    --test-range 1000,1200 --test-range 1200,1800 --test-range 1800,2500 --test-range 500,550 --test-range 550,600 --test-range 600,650 --test-range 650,700 \
    --test-range 700,750 --test-range 750,800 --test-range 800,900 --test-range 900,1000 --first-toy $toy \
    $@


##     --fit-range 300,3000 --saveas png --test-range 1000,3000 --test-range 500,550 --test-range 550,600 --test-range 600,650 --test-range 650,700 --test-range 700,750 --test-range 750,800 --test-range 800,900 --test-range 900,1000 --first-toy $toy \

##    --test-range 1000,1200 --test-range 1200,1800 --test-range 1800,2500 --test-range 2500,3500 --test-range 3500,4500 --test-range 500,550 --test-range 550,600 --test-range 600,650 --test-range 650,700 --test-range 700,750 --test-range 750,800 --test-range 800,900 --test-range 900,1000 --test-range 4500,5500 --first-toy $toy \

mkdir -p $outdir
cp -p $outname $outdir
