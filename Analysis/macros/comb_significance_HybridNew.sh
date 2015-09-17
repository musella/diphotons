
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
exp=$4
i=$LSB_JOBINDEX

rmax=$( echo "$exp+100* $exp" | bc -l )
rmin=$( echo "$exp-100*$exp" | bc -l )

combine -M HybridNew --frequentist datacards_cic2${lum}/datacard_bkg_bias_${minMass}_${signame}.txt --significance -s -1 --saveToys --fullBToys --saveHybridResult -T 25 -i 2 -n _signif_grid_1000sig_${i}_ -m $mass -v 3 --fork 4 --rMax $rmax --rMin $rmin --signalForSignificance $exp