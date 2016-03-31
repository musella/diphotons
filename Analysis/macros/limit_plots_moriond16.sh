#!/bin/bash

spin=$1 && shift

args="-U --fixed-x-section 1.e-3 --use-fb --load lumi_combined.json --saveas pdf,convert_png,root --y-range 1e-4,0.55 -k 001,01,02 $@"


## --load lumi_limits.json --lumi 2.56

## www=$(echo $target | sed 's%_cic.*%%')
www=limits_76_unblind
target=combination

dir8=results_8TeV_${spin}
dir13=combined_${spin}_wnuis_unblind
dirCmb=combined_813_${spin}_unblind

dir0T=full_analysis_moriond16v1_0T_sync_v6_data_cic0T_default_shapes_${spin}_wnuis_lumi_0.59
dir38T=full_analysis_moriond16v1_sync_v4_data_cic2_default_shapes_${spin}_wnuis_lumi_2.69


### ./limit_plots.py $args --do-comparison --compare-files $dirCmb/graphs_ProfileLikelihood.root,$dir8/graphs_ProfileLikelihood.root,$dir13/graphs_ProfileLikelihood.root  --do-pvalues -O  ~/www/exo/moriond16/$www/$target --load lumi_combined.json -k 001 --legend 0.54,0.15,0.85,0.42 --${spin} --x-range 500,3000 --label combination_full_mass_${spin}_ --compare-labels 'Combined,8TeV,13TeV'
### 
### ./limit_plots.py $args --do-comparison --compare-files $dirCmb/graphs_ProfileLikelihood.root,$dir8/graphs_ProfileLikelihood.root,$dir13/graphs_ProfileLikelihood.root  --do-pvalues -O  ~/www/exo/moriond16/$www/$target --load lumi_combined.json -k 001 --legend 0.24,0.15,0.55,0.42 --${spin} --x-range 500,850 --label combination_low_mass_${spin}_ --compare-labels 'Combined,8TeV,13TeV'
### 
### ./limit_plots.py $args --do-comparison --compare-files $dirCmb/graphs_ProfileLikelihood.root,$dir8/graphs_ProfileLikelihood.root,$dir13/graphs_ProfileLikelihood.root  --do-pvalues -O  ~/www/exo/moriond16/$www/$target --load lumi_combined.json -k 001 --legend 0.54,0.15,0.85,0.42 --${spin} --x-range 850,3000 --label combination_high_mass_${spin}_ --compare-labels 'Combined,8TeV,13TeV'




### target=13TeV
### 
### ./limit_plots.py $args --do-comparison --compare-files $dir13/graphs_ProfileLikelihood.root,$dir0T/graphs_ProfileLikelihood.root,$dir38T/graphs_ProfileLikelihood.root  --do-pvalues -O  ~/www/exo/moriond16/$www/$target --load lumi_combined.json -k 001 --legend 0.54,0.15,0.85,0.42 --${spin} --x-range 500,4500 --label combination_full_mass_${spin}_ --compare-labels 'Combined,0T 0.6 fb^{-1},3.8T 2.7 fb^{-1}' --load lumi_limits.json
### 
### ./limit_plots.py $args --do-comparison --compare-files $dir13/graphs_ProfileLikelihood.root,$dir0T/graphs_ProfileLikelihood.root,$dir38T/graphs_ProfileLikelihood.root  --do-pvalues -O  ~/www/exo/moriond16/$www/$target --load lumi_combined.json -k 001 --legend 0.24,0.15,0.55,0.42 --${spin} --x-range 500,850 --label combination_low_mass_${spin}_ --compare-labels 'Combined,0T 0.6 fb^{-1},3.8T 2.7 fb^{-1}'  --load lumi_limits.json
### 
### ./limit_plots.py $args --do-comparison --compare-files $dir13/graphs_ProfileLikelihood.root,$dir0T/graphs_ProfileLikelihood.root,$dir38T/graphs_ProfileLikelihood.root  --do-pvalues -O  ~/www/exo/moriond16/$www/$target --load lumi_combined.json -k 001 --legend 0.54,0.15,0.85,0.42 --${spin} --x-range 850,4500 --label combination_high_mass_${spin}_ --compare-labels 'Combined,0T 0.6 fb^{-1},3.8T 2.7 fb^{-1}'  --load lumi_limits.json


### spin0=combined_spin0_wnuis_unblind
### spin2=combined_spin2_wnuis_unblind
### target=13TeV
### 
### ./limit_plots.py $args --do-comparison --compare-files $spin0/graphs_ProfileLikelihood.root,$spin2/graphs_ProfileLikelihood.root  --do-pvalues -O  ~/www/exo/moriond16/$www/$target --load lumi_combined.json --legend 0.54,0.15,0.85,0.42 --no-spin-in-header --x-range 500,4500 --label combination_full_mass_ --compare-labels 'J = 0,J = 2' --extra-lines-style 1  --load lumi_limits.json
### 
### ./limit_plots.py $args --do-comparison  --compare-files $spin0/graphs_ProfileLikelihood.root,$spin2/graphs_ProfileLikelihood.root  --do-pvalues -O  ~/www/exo/moriond16/$www/$target --load lumi_combined.json --legend 0.24,0.15,0.55,0.42 --no-spin-in-header --x-range 500,850 --label combination_low_mass_ --compare-labels 'J = 0,J = 2' --extra-lines-style 1  --load lumi_limits.json
### 
### ./limit_plots.py $args --do-comparison --compare-files $spin0/graphs_ProfileLikelihood.root,$spin2/graphs_ProfileLikelihood.root  --do-pvalues -O  ~/www/exo/moriond16/$www/$target --load lumi_combined.json -k 001 --legend 0.54,0.15,0.85,0.42 --${spin} --x-range 850,4500 --label combination_high_mass_ --compare-labels 'J = 0,J = 2' --extra-lines-style 1  --load lumi_limits.json


set -x 
./limit_plots_76.sh $dir0T 0.6
./limit_plots_76.sh $dir38T

./limit_plots_76.sh $dir13 3.3

./limit_plots_combination.sh $dirCmb

