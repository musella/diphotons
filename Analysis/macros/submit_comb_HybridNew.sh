#!/bin/bash

export DISPLAY=""
mydir="/afs/cern.ch/work/k/khoumani/CMSSW_7_1_5/src/diphotons/Analysis/macros"

set -x

cd $mydir

eval `scramv1 runtime -sh`
##equivalent to cmsenv


minMass=300
signame=$1
mass=$2
lum=_lum_$3
min=$4
max=$5

#massdir=Folder_${signame}${lum}
#mkdir -p $massdir
#cd $massdir

#ln -s ../workspace_cic2_dijet_lum_5 workspace_cic2_dijet_lum_5
#ln -s ../workspace_cic2_dijet_bkgnbias_lum_5 workspace_cic2_dijet_bkgnbias_lum_5
#ln -s ../workspace_cic2_signals_lum_5 workspace_cic2_signals_lum_5
#cp ../comb.sh ./

#point=$(awk -v i=$LSB_JOBINDEX -v max=$max 'BEGIN { print i*(max/2.0) }')
bsub -J "test[1-300]" -q 1nh -n 4 -R "span[hosts=1]" -o _Logfile_uplim_grid_${signame}${lum}_%I.log comb_HybridNew.sh $signame $mass $lum $min $max

#mv higgsCombine_TEST_Grid_${signame}${lum}_*.root $massdir