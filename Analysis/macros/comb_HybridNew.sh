#!/bin/bash

#export DISPLAY=""
mydir="/afs/cern.ch/work/k/khoumani/CMSSW_7_1_5/src/diphotons/Analysis/macros"

set -x
cd $mydir

eval `scramv1 runtime -sh`
##equivalent to cmsenv

minMass=300

signame=$1
mass=$2
lum=$3
min=$4
max=$5

i=$LSB_JOBINDEX

point=$( echo " $min + ($i-1) * ($max-$min)/200" | bc -l )
rmin=$( echo "$point - 100 * $point" | bc -l)
rmax=$( echo "$point + 100 * $point" | bc -l)

combine -M HybridNew --frequentist datacards_cic2${lum}/datacard_bkg_bias_${minMass}_${signame}.txt -T 250 -i 2 -s -1 --saveToys --saveHybridResult -n _uplim_grid_${signame}${lum}_${i}_ --clsAcc 0 -v 4 -m $mass --fork 4 --singlePoint $point --rMax $rmax --rMin $rmin