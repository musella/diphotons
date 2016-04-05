#!/bin/basj

## Prepare workspaces and datacards
## --------------------------------------------------------------------------------------------------------------------------------------------------------------

# nutples folders
dir0=full_analysis_moriond16v1_0T_sync_v6_data
dir38=full_analysis_moriond16v1_sync_v4_data

# eos folders w/ signal model
eosdir0=~/eos/cms/store/user/crovelli/WSdiphoton76x_0T_v6
eosdir38=~/eos/cms/store/user/crovelli/WSdiphoton76x_v4

# coups="001 005 007 01 015 02"
## coups="02"
## coups="001"
coups=001

common_opts="--plot-fit-bands --rescale-signal-to 1e-3 --minos-bands --compute-fwhm --generate-ws-bkgnbias --prepare-data --do-parametric-signal-nuisances"

## 0T workspaces 
parallel --ungroup -j 1 "./combine_maker.sh $dir0 --lumi 0.59 --fit-name cic0T $common_opts --parametric-signal $eosdir0/signalModel76x_{}_500to998_0T_v6.root --parametric-signal $eosdir0/signalModel76x_{}_1000to*_0T_v6.root --parametric-signal ${eosdir0}_smearUp/signalModel76x_{}_500to998_0T_v6_smearUp.root --parametric-signal ${eosdir0}_smearUp/signalModel76x_{}_1000to*_0T_v6_smearUp.root --parametric-signal ${eosdir0}_smearDown/signalModel76x_{}_500to998_0T_v6_smearDown.root --parametric-signal ${eosdir0}_smearDown/signalModel76x_{}_1000to*_0T_v6_smearDown.root --parametric-signal-acceptance  acceptance_0T.json --load lumi.json --only-coups {} --label spin2_wnuis" ::: $coups  
 
parallel --ungroup -j 1 "./combine_maker.sh $dir0 --lumi 0.59 --fit-name cic0T $common_opts --parametric-signal $eosdir0/signalModel76x_{}_500to998_0T_v6.root --parametric-signal $eosdir0/signalModel76x_{}_1000to*_0T_v6.root --parametric-signal ${eosdir0}_smearUp/signalModel76x_{}_500to998_0T_v6_smearUp.root --parametric-signal ${eosdir0}_smearUp/signalModel76x_{}_1000to*_0T_v6_smearUp.root --parametric-signal ${eosdir0}_smearDown/signalModel76x_{}_500to998_0T_v6_smearDown.root --parametric-signal ${eosdir0}_smearDown/signalModel76x_{}_1000to*_0T_v6_smearDown.root --parametric-signal-acceptance  acceptance_0T_spin0.json --load lumi.json --only-coups {} --label spin0_wnuis" ::: $coups 

## 38T workspaces
parallel --ungroup -j 1 "./combine_maker.sh $dir38 --lumi 2.69 --fit-name cic2 $common_opts --parametric-signal $eosdir38/signalModel76x_{}_500to998__resolv4.root  --parametric-signal ${eosdir38}_smearUp/signalModel76x_{}_500to998__resolv4_smearUp.root --parametric-signal ${eosdir38}_smearDown/signalModel76x_{}_500to998__resolv4_smearDown.root --parametric-signal $eosdir38/signalModel76x_{}_1000to*__resolv4.root  --parametric-signal ${eosdir38}_smearUp/signalModel76x_{}_1000to*__resolv4_smearUp.root --parametric-signal ${eosdir38}_smearDown/signalModel76x_{}_1000to*__resolv4_smearDown.root  --parametric-signal-acceptance  acceptance_76.json --load lumi.json --only-coups {}   --label spin2_wnuis --do-parametric-signal-nuisances" ::: $coups &

parallel --ungroup -j 1 "./combine_maker.sh $dir38 --lumi 2.69 --fit-name cic2 $common_opts --parametric-signal $eosdir38/signalModel76x_{}_500to998__resolv4.root  --parametric-signal ${eosdir38}_smearUp/signalModel76x_{}_500to998__resolv4_smearUp.root --parametric-signal ${eosdir38}_smearDown/signalModel76x_{}_500to998__resolv4_smearDown.root --parametric-signal $eosdir38/signalModel76x_{}_1000to*__resolv4.root  --parametric-signal ${eosdir38}_smearUp/signalModel76x_{}_1000to*__resolv4_smearUp.root --parametric-signal ${eosdir38}_smearDown/signalModel76x_{}_1000to*__resolv4_smearDown.root  --parametric-signal-acceptance  acceptance_76_spin0.json --load lumi.json --only-coups {}   --label spin0_wnuis --do-parametric-signal-nuisances" ::: $coups &

wait


## Run combine
## --------------------------------------------------------------------------------------------------------------------------------------------------------------

coups="001 01 02"
## coups="02"


## parallel -j 4 --ungroup "./combineall.sh ${dir0}_cic0T_default_shapes_spin2_wnuis_lumi_0.59 {} -M Asymptotic --run both --hadd  --rMax 60" ::: $coups &
## parallel -j 4 --ungroup "./combineall.sh ${dir0}_cic0T_default_shapes_spin0_wnuis_lumi_0.59 {} -M Asymptotic --run both --hadd  --rMax 60" ::: $coups &
## parallel -j 4 --ungroup "./combineall.sh ${dir38}_cic2_default_shapes_spin2_wnuis_lumi_2.69 {} -M Asymptotic --run both --hadd --rMax 60" ::: $coups &
## parallel -j 4 --ungroup "./combineall.sh ${dir38}_cic2_default_shapes_spin0_wnuis_lumi_2.69 {} -M Asymptotic --run both --hadd  --rMax 60" ::: $coups &

## parallel -j 4 --ungroup "./combineall.sh ${dir0}_cic0T_default_shapes_spin2_wnuis_lumi_0.59 {} -M ProfileLikelihood --pvalue --significance --hadd --rMax 60" ::: $coups &
## parallel -j 4 --ungroup "./combineall.sh ${dir0}_cic0T_default_shapes_spin0_wnuis_lumi_0.59 {} -M ProfileLikelihood --pvalue --significance --hadd --rMax 60" ::: $coups &
## parallel -j 4 --ungroup "./combineall.sh ${dir38}_cic2_default_shapes_spin2_wnuis_lumi_2.69 {} -M ProfileLikelihood --pvalue --significance --hadd --rMax 60" ::: $coups &
## parallel -j 4 --ungroup "./combineall.sh ${dir38}_cic2_default_shapes_spin0_wnuis_lumi_2.69 {} -M ProfileLikelihood --pvalue --significance --hadd --rMax 60" ::: $coups &

### ./combine_cards.sh  ${dir38}_cic2_default_shapes_spin0_wnuis_lumi_2.69 ${dir0}_cic0T_default_shapes_spin0_wnuis_lumi_0.59 combined_spin0_wnuis_unblind
### ./combine_cards.sh  ${dir38}_cic2_default_shapes_spin2_wnuis_lumi_2.69 ${dir0}_cic0T_default_shapes_spin2_wnuis_lumi_0.59 combined_spin2_wnuis_unblind
### 
### parallel -j 4 --ungroup "./combineall.sh combined_spin0_wnuis_unblind {} -M ProfileLikelihood --pvalue --significance --hadd --rMax 60" ::: $coups &
### parallel -j 4 --ungroup "./combineall.sh combined_spin2_wnuis_unblind {} -M ProfileLikelihood --pvalue --significance --hadd --rMax 60" ::: $coups &
### 
### parallel -j 4 --ungroup "./combineall.sh combined_spin0_wnuis_unblind {} -M Asymptotic --run both --hadd  --rMax 60" ::: $coups &
### parallel -j 4 --ungroup "./combineall.sh combined_spin2_wnuis_unblind {} -M Asymptotic --run both --hadd  --rMax 60" ::: $coups &


wait

### coups="02"
### 
### parallel -j 4 --ungroup "./combineall.sh ${dir0}_cic0T_default_shapes_spin2_wnuis_lumi_0.59 {} -M Asymptotic --run both --hadd  --rMax 60 " ::: $coups &
### parallel -j 4 --ungroup "./combineall.sh ${dir0}_cic0T_default_shapes_spin0_wnuis_lumi_0.59 {} -M Asymptotic --run both --hadd  --rMax 60 " ::: $coups &
### 
### 
### parallel -j 4 --ungroup "./combineall.sh ${dir0}_cic0T_default_shapes_spin2_wnuis_lumi_0.59 {} -M ProfileLikelihood --pvalue --significance --hadd --rMax 60 " ::: $coups &
### parallel -j 4 --ungroup "./combineall.sh ${dir0}_cic0T_default_shapes_spin0_wnuis_lumi_0.59 {} -M ProfileLikelihood --pvalue --significance --hadd --rMax 60 " ::: $coups &
### 
### parallel -j 4 --ungroup "./combineall.sh combined_spin0_wnuis_unblind {} -M ProfileLikelihood --pvalue --significance --hadd --rMax 60 " ::: $coups &
### parallel -j 4 --ungroup "./combineall.sh combined_spin2_wnuis_unblind {} -M ProfileLikelihood --pvalue --significance --hadd --rMax 60 " ::: $coups &
### 
### parallel -j 4 --ungroup "./combineall.sh combined_spin0_wnuis_unblind {} -M Asymptotic --run both --hadd  --rMax 60 " ::: $coups &
### parallel -j 4 --ungroup "./combineall.sh combined_spin2_wnuis_unblind {} -M Asymptotic --run both --hadd  --rMax 60 " ::: $coups &
