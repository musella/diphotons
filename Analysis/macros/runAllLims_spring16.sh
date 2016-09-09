#!/bin/bash

## Run combine
## --------------------------------------------------------------------------------------------------------------------------------------------------------------

## masses=$(echo $(seq 500 2 750; seq 750 2 1000; seq 1000 4 2100; seq 2100 100 4500;) | sort -n -u  | tr ' ' ',' | sed 's%,$%%')
masses=$(echo $(seq 500 2 750; seq 750 2 1000; seq 1000 4 2100; seq 2100 100 4500;) | sort -n -u  | tr ' ' ',' | sed 's%,$%%')

# coups="001,01,02"
## coups="001,02"
## coups="01"

coups="01"
## coups="02"

dirs_cmb="combined_13TeV_8TeV_cert_ichep_parametic_v2_spin0 combined_13TeV_8TeV_cert_ichep_parametic_v2_spin2"
## dirs="full_analysis_spring16v2_sync_v6_ichep_cic2016_default_shapes_spin0_parametric_v2_lumi_12.9 combined_13TeV_cert_ichep_parametic_v2_spin0 full_analysis_spring16v2_sync_v6_ichep_cic2016_default_shapes_spin2_parametric_v2_lumi_12.9  combined_13TeV_cert_ichep_parametic_v2_spin2"
dirs="full_analysis_spring16v2_sync_v6_ichep_cic2016_default_shapes_spin0_parametric_v2_lumi_12.9 full_analysis_spring16v2_sync_v6_ichep_cic2016_default_shapes_spin2_parametric_v2_lumi_12.9"

## parallel --ungroup -j 4 "./text2wsall.sh {}" ::: $dirs

## parallel --ungroup -j 4 "./runAllLims.sh {} -m $masses -k $coups -j 3 --parametric --hadd --cont" :::  $dirs

parallel --ungroup -j 2 "./runAllLims.sh {} -m $masses -k $coups -j 3 -M Asymptotic --minimizerStrategy 2 --parametric --sub 1nh" :::  $dirs 
parallel --ungroup -j 2 "./runAllLims.sh {} -m $masses -k $coups -j 3 -M ProfileLikelihood --parametric --sub 1nh" :::  $dirs 


### parallel --ungroup -j 4 "./text2wsall.sh {}" ::: $dirs_cmb

## masses=$(echo $(seq 500 2 750; seq 750 2 850;) | sort -n -u  | tr ' ' ',' | sed 's%,$%%')
## parallel --ungroup -j 1 "./runAllLims.sh {} -m $masses -k $coups -j 3 --minimizerStrategy  --sub 1nh" :::  $dirs_cmb

### masses=$(echo $(seq 852 2 1000; seq 1000 4 2100; seq 2100 100 4500;) | sort -n -u  | tr ' ' ',' | sed 's%,$%%')
### parallel --ungroup -j 1 "./runAllLims.sh {} -m $masses -k $coups -j 3 --parametric  --sub 8nm" :::  $dirs_cmb


