#!/bin/bash


version=$1
if [[ -z $version ]]; then
   echo "usage: $0 <version>"
   exit -1
fi

set -x 

## fggRunJobs.py --load ../config/jobs_diphoton_qcd.json     ../config/high_mass_analysis.py maxEvents=-1  -d full_analysis_spring15_7415v2_sync_${version}_qcd -n 20 -q 8nh -H &

fggRunJobs.py --load ../config/jobs_diphoton_runD.json     ../config/high_mass_analysis.py maxEvents=-1  -d full_analysis_spring15_7415v2_sync_${version}_data -n 20 -q 8nh -H &
fggRunJobs.py --load ../config/jobs_diphoton.json     ../config/high_mass_analysis.py maxEvents=-1  -d full_analysis_spring15_7415v2_sync_${version} -n 20 -q 8nh -H &

fggRunJobs.py --load ../config/jobs_photon_runD.json       ../config/high_mass_analysis.py maxEvents=-1  -d single_photon_spring15_7415v2_sync_${version}_data -n 20 -q 8nh -H &
fggRunJobs.py --load ../config/jobs_photon.json       ../config/high_mass_analysis.py maxEvents=-1  -d single_photon_spring15_7415v2_sync_${version} -n 20 -q 8nh -H &

fggRunJobs.py --load ../config/jobs_dielectron_runD.json      ../config/high_mass_analysis.py maxEvents=-1  -d double_ele_spring15_7415v2_sync_${version}_data -n 30 -q 8nh -H &
fggRunJobs.py --load ../config/jobs_dielectron.json      ../config/high_mass_analysis.py maxEvents=-1  -d double_ele_spring15_7415v2_sync_${version} -n 30 -q 8nh -H &



wait
