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

outname=$(basename $output)
outdir=$(dirname $output)

$mydir/bkg_bias.py --n-toys $ntoys \
    --store-new-only \
    --components pp --models dijet \
    --read-ws $input  -o $outname \
    --fit-toys \
    --fit-range 300,3000 --saveas png --test-range 1000,3000 --test-range 500,600 --test-range 600,800 --test-range 800,1000 --first-toy $toy \
    $@


mkdir -p $outdir
cp -p $outname $outdir
