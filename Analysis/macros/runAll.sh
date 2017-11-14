#!/bin/bash

version=$1
if [[ -z $version ]]; then
   echo "usage: $0 <version>"
   exit -1
fi

set -x 


fggRunJobs.py --load ../config/jobs_dielectron_92.json   ../config/high_mass_analysis.py applyDiphotonCorrections=0 -d double_ele_${version}_92_data maxEvents=100  -H --no-use-tarball &
## maxEvents=-1  -n 50 -H -q 8nh &

fggRunJobs.py --load ../config/jobs_dielectron_92_mc.json   ../config/high_mass_analysis.py applyDiphotonCorrections=0 -d double_ele_${version}_92_mc maxEvents=100  -H --no-use-tarball &
## maxEvents=-1 -n 200 -H -q 1nd &


## fggRunJobs.py --load ../config/jobs_diphoton_80_cert_274443.json     ../config/high_mass_analysis.py applyDiphotonCorrections=1 maxEvents=-1  -d full_analysis_spring16v1_sync_${version}_cert_274443 -n 50 -H -q 8nh  &

## fggRunJobs.py --load ../config/jobs_diphoton_80_topup_275125.json     ../config/high_mass_analysis.py applyDiphotonCorrections=1 maxEvents=-1  -d full_analysis_spring16v1_sync_${version}_topup_275125 -n 40 -H -q 8nh  &

## fggRunJobs.py --load ../config/jobs_diphoton_80_cert_275125.json     ../config/high_mass_analysis.py applyDiphotonCorrections=1 maxEvents=-1  -d full_analysis_spring16v1_sync_${version}_cert_275125 -n 40 -H -q 8nh &


## fggRunJobs.py --load ../config/jobs_diphoton_80_topup_276097.json     ../config/high_mass_analysis.py applyDiphotonCorrections=1 maxEvents=-1  -d full_analysis_spring16v1_sync_${version}_topup_276097 -n 40 -H -q 8nh   &


## fggRunJobs.py --load ../config/jobs_diphoton_80_ichep.json     ../config/high_mass_analysis.py applyDiphotonCorrections=1 maxEvents=-1  -d full_analysis_spring16v2_sync_${version}_ichep -n 50 -H -q 8nh &
 
## fggRunJobs.py --load ../config/jobs_diphoton_80.json     ../config/high_mass_analysis.py applyDiphotonCorrections=1 maxEvents=-1  -d full_analysis_spring16v2_sync_${version}_mc -n 50 -H -q 8nh &

## fggRunJobs.py --load ../config/jobs_dielectron_80_ichep_tauseed.json     ../config/high_mass_analysis.py applyDiphotonCorrections=1 maxEvents=-1  -d double_ele_spring16v2_sync_${version}_ichep_tauseed  -n 50 -H -q 8nh &
## 

## q-Regression
## fggRunJobs.py --load ../config/jobs_dielectron_80_ichep.json     ../config/high_mass_analysis.py applyDiphotonCorrections=1 maxEvents=-1  -d double_ele_spring16v2_sync_${version}_ichep  -n 50 -H -q 8nh &
 
## fggRunJobs.py --load ../config/jobs_dielectron_80.json     ../config/high_mass_analysis.py applyDiphotonCorrections=1 maxEvents=-1  -d double_ele_spring16v2_sync_${version}_mc -n 75 -H -q 8nh &

## fggRunJobs.py --load ../config/jobs_dielectron_80_moriond17.json   ../config/high_mass_analysis.py applyDiphotonCorrections=1 maxEvents=10000  -d double_ele_${version}_moriond17_data -H  --no-use-tarball &
## fggRunJobs.py --load ../config/jobs_dielectron_80_moriond17.json   ../config/high_mass_analysis.py applyDiphotonCorrections=1 maxEvents=-1  -d double_ele_${version}_moriond17_data  -n 50 -H -q 8nh &

## fggRunJobs.py --load ../config/jobs_dielectron_80_moriond17_mc.json   ../config/high_mass_analysis.py applyDiphotonCorrections=1 maxEvents=1000  -d double_ele_${version}_moriond17_mc  -H --no-use-tarball &
## fggRunJobs.py --load ../config/jobs_dielectron_80_moriond17_mc.json   ../config/high_mass_analysis.py applyDiphotonCorrections=1 maxEvents=-1  -d double_ele_${version}_moriond17_mc -n 200 -H -q 1nd &


## fggRunJobs.py --load ../config/jobs_diphoton_80_cert_274421.json     ../config/high_mass_analysis.py applyDiphotonCorrections=1 maxEvents=-1  -d full_analysis_spring16v1_sync_${version}_cert_274421 -n 50 -H -q 8nh  &

## fggRunJobs.py --load ../config/jobs_diphoton_80_dcsonly_274422.json     ../config/high_mass_analysis.py applyDiphotonCorrections=1 maxEvents=-1  -d full_analysis_spring16v1_sync_${version}_dcsonly_274422 -n 30 -H -q 8nh  &

## fggRunJobs.py --load ../config/jobs_dielectron_80.json      ../config/high_mass_analysis.py applyDiphotonCorrections=1 maxEvents=-1 histosOnly=1 -d double_ele_spring16v1_sync_${version}_mc -n 60 -q 8nh -H &

## fggRunJobs.py --load ../config/jobs_dielectron_80_cert_274421.json     ../config/high_mass_analysis.py applyDiphotonCorrections=1 maxEvents=-1 histosOnly=1 -d double_ele_spring16v1_sync_${version}_cert_274421 -n 50 -H -q 8nh  &

## fggRunJobs.py --load ../config/jobs_dielectron_80_hgg.json     ../config/high_mass_analysis.py applyDiphotonCorrections=1 maxEvents=-1 histosOnly=0 -d double_ele_spring16v1_sync_${version}_hgg doeleId=1 dol1Match=1 dohltMatch=1 -n 100 -H -q 8nh

## fggRunJobs.py --load ../config/jobs_dielectron_80_dcsonly_274422.json     ../config/high_mass_analysis.py applyDiphotonCorrections=1 maxEvents=-1 histosOnly=1 -d double_ele_spring16v1_sync_${version}_dcsonly_274422 -n 30 -H -q 8nh  &


### fggRunJobs.py --load ../config/jobs_diphoton_80.json     ../config/high_mass_analysis.py applyDiphotonCorrections=1 maxEvents=-1  -d full_analysis_spring16v1_sync_${version} -n 50 -H -q 8nh  &
### 
### fggRunJobs.py --load ../config/jobs_dielectron_80.json      ../config/high_mass_analysis.py applyDiphotonCorrections=1 maxEvents=-1  -d double_ele_spring16v1_sync_${version} -n 40 -q 8nd -H &

# fggRunJobs.py --load ../config/jobs_diphoton_80.json     ../config/high_mass_analysis.py applyDiphotonCorrections=1 maxEvents=10  -d full_analysis_spring16v1_sync_${version} -n 1  &


## fggRunJobs.py --load ../config/jobs_diphoton_80.json     ../config/high_mass_analysis.py applyDiphotonCorrections=1 maxEvents=-1  -d full_analysis_spring16v1_sync_${version}_data -n 50 -H -q 1nh  &

## fggRunJobs.py --load ../config/jobs_diphoton_80.json     ../config/high_mass_analysis.py applyDiphotonCorrections=1 maxEvents=-1  -d full_analysis_spring16v1_sync_${version}_gjets -n 50 -H -q 1nh  &

## fggRunJobs.py --load ../config/jobs_dielectron_80.json      ../config/high_mass_analysis.py applyDiphotonCorrections=1 maxEvents=10  -d double_ele_spring16v1_sync_${version}_data  &

## fggRunJobs.py --load ../config/jobs_dielectron_80.json      ../config/high_mass_analysis.py applyDiphotonCorrections=1 maxEvents=-1  -d double_ele_spring16v1_sync_${version}_data -n 40 -q 1nd -H &


## fggRunJobs.py --load ../config/jobs_diphoton_76.json     ../config/high_mass_analysis.py applyDiphotonCorrections=1 maxEvents=-1  -d full_analysis_moriond16v1_sync_${version}_data -n 50 -H -q 1nh  &

## fggRunJobs.py --load ../config/jobs_diphoton_76.json     ../config/high_mass_analysis.py applyDiphotonCorrections=1 maxEvents=-1 useVtx0=1 -d full_analysis_moriond16v1_sync_${version}_data -n 100 -H -q 1nh  &

## fggRunJobs.py --load ../config/jobs_diphoton_76.json     ../config/high_mass_analysis.py applyDiphotonCorrections=1 maxEvents=-1 useVtx0=1 -d full_analysis_moriond16v1_sync_${version}_data -n 50 -H -q 1nd --no-use-tarball  &

## fggRunJobs.py --load ../config/jobs_diphoton_76.json     ../config/high_mass_analysis.py applyDiphotonCorrections=1 maxEvents=10 useVtx0=1 -d full_analysis_moriond16v1_sync_${version}_data_test -n 1 -H  &

## fggRunJobs.py --load ../config/jobs_diphoton_qcd.json     ../config/high_mass_analysis.py maxEvents=-1  -d full_analysis_moriond16v1_sync_${version}_qcd -n 20 -q 1nd -H &

## fggRunJobs.py --load ../config/jobs_diphoton_runD.json     ../config/high_mass_analysis.py addRegressionInput=1 maxEvents=-1  -d full_analysis_moriond16v1_sync_${version}_data -n 30 -q 1nd -H &
## fggRunJobs.py --load ../config/jobs_diphoton.json     ../config/high_mass_analysis.py applyDiphotonCorrections=1  maxEvents=-1  -d full_analysis_moriond16v1_sync_${version} -n 30 -q 1nd -H &

# fggRunJobs.py --load ../config/jobs_photon_runD.json       ../config/high_mass_analysis.py maxEvents=-1  -d single_photon_moriond16v1_sync_${version}_data -n 20 -q 1nd -H &
## fggRunJobs.py --load ../config/jobs_photon.json       ../config/high_mass_analysis.py maxEvents=-1  -d single_photon_moriond16v1_sync_${version} -n 20 -q 1nd -H &

### fggRunJobs.py --load ../config/jobs_dielectron_runD.json      ../config/high_mass_analysis.py applyDiphotonCorrections=1 maxEvents=-1  -d double_ele_moriond16v1_sync_${version}_data -n 40 -q 1nd -H &
### fggRunJobs.py --load ../config/jobs_dielectron.json      ../config/high_mass_analysis.py  applyDiphotonCorrections=1 maxEvents=-1  -d double_ele_moriond16v1_sync_${version} -n 40 -q 1nd -H &

## fggRunJobs.py --load ../config/jobs_diphoton_rereco.json     ../config/high_mass_analysis.py addRegressionInput=1 maxEvents=-1  -d full_analysis_moriond16v1_sync_${version}_data -n 200 -q 1nd -H &

## fggRunJobs.py --load ../config/jobs_dielectron_rereco.json      ../config/high_mass_analysis.py applyDiphotonCorrections=1 maxEvents=-1  -d double_ele_moriond16v1_sync_${version}_data -n 200 -q 1nd -H &

## fggRunJobs.py --load ../config/jobs_dielectron_76.json      ../config/high_mass_analysis.py applyDiphotonCorrections=1 maxEvents=-1  -d double_ele_moriond16v1_sync_${version}_data -n 50 -q 1nd -H &

## fggRunJobs.py --load ../config/jobs_dielectron_76.json      ../config/high_mass_analysis.py applyDiphotonCorrections=1 maxEvents=10000  -d double_ele_moriond16v1_sync_${version}_data -n 1 --no-use-tarball &

## fggRunJobs.py --load ../config/jobs_gen_spin0.json     ../config/gen_only_analysis.py useAAA=1 maxEvents=-1  -d full_analysis_gen_only_spin0_v1 -n 2 -q 8nh --no-copy-proxy -H &

# 


## fggRunJobs.py  ../config/photonIdDumper.py maxEvents=-1 --load ../config/jobs_photon_id.json  -d photon_id_$version -n 20 -q 8nh -H & 

wait
