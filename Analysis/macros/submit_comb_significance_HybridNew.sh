#!/bin/bash

# This script calls comb_sig.sh

export DISPLAY=""
mydir="/afs/cern.ch/work/k/khoumani/CMSSW_7_1_5/src/diphotons/Analysis/macros"

set -x

cd $mydir

eval `scramv1 runtime -sh`


minMass=300
signame=$1
mass=$2
lum=_lum_$3
exp=$4

bsub -J "test_sig[1-50]" -q 1nh -n 4 -R "span[hosts=1]" -o _LogFile_signif_grid_sig_${signame}${lum}_%I.log comb_significance_HybridNew.sh $signame $mass $lum $exp