#!/bin/bash



www=limits_12p9invfb
target=comparison


### dir2016=full_analysis_spring16v2_sync_v6_ichep_cic2016_default_shapes_spin0_parametric_v2_lumi_12.9
### dirXcheck=xcheck_12p9fbinv
### 
### ./limit_plots.py --do-simple-summary --lumistr 12.9 \
###     --compare-file $dir2016/graphs_Asymptotic.root   --compare-label 'ref' \
###     --compare-file $dirXcheck/graphs_Asymptotic.root   --compare-label 'x-check' \
###     -O ~/www/exo/spring16/$www/$target --legend 0.55,0.36,0.85,0.85 --x-range 450,4100  \
###     --no-spin-in-header --load lumi_internal.json  --saveas convert_png,pdf,root --savebw --label xcheck \
###     --use-fb --use-mx -k 001
### 
### ./limit_plots.py $args --do-comparison --lumistr 12.9 \
###     --compare-file $dir2016/graphs_ProfileLikelihood.root   --compare-label 'ref' \
###     --compare-file $dirXcheck/graphs_ProfileLikelihood.root   --compare-label 'x-check' \
###     --do-pvalues -O  ~/www/exo/spring16/$www/$target --load lumi_internal.json -k 001 \
###     --legend 0.55,0.15,0.85,0.47 --x-range 450,4500 --label pval_xcheck \
###     --spin0  --use-mx
### 
### 
### exit 

### ./limit_plots.py --do-summary \
###     --compare-file combined_813_spin0_ext/graphs_Asymptotic.root   --compare-label 'J=0' \
###     --compare-file combined_813_spin2_ext/graphs_Asymptotic.root   --compare-label 'J=2' \
###     -O ~/www/exo/moriond16/$www/ --legend 0.6,0.37,0.9,0.87 --x-range 450,4100  \
###     --no-spin-in-header --load lumi_summary.json  --saveas convert_png,pdf,root --savebw --label combined \
###     --x-sections ../../MetaData/data/cross_sections_gen_grid.json --use-fb --use-mx


## args="-U --fixed-x-section 1.e-3 --use-fb --saveas pdf,convert_png,root --y-range 1e-4,0.55 -k 001,01,02 --lumistr 7.3  --use-mx $@"
## args="-U --fixed-x-section 1.e-3 --use-fb --saveas pdf,convert_png,root --y-range 1e-4,0.55 -k 001,01,02 --use-mx $@"
args="-U --fixed-x-section 1.e-3 --use-fb --saveas pdf,png,root --y-range 1e-4,0.55 -k 001,01,02  --use-mx $@"

target=paper
last_args="--load-merge lumi_paper.json"

[[ ! -d  ~/www/exo/spring16/$www ]] && mkdir  ~/www/exo/spring16/$www


dir2016_spin0=full_analysis_spring16v2_sync_v6_ichep_cic2016_default_shapes_spin0_parametric_v2_lumi_12.9
dir2016_spin2=full_analysis_spring16v2_sync_v6_ichep_cic2016_default_shapes_spin2_parametric_v2_lumi_12.9
./limit_plots.py $args --do-comparison --compare-files $dir2016_spin0/graphs_ProfileLikelihood.root,$dir2016_spin2/graphs_ProfileLikelihood.root  --do-pvalues -O  ~/www/exo/spring16/$www/$target --load lumi_summary_spring16.json --legend 0.32,0.18,0.62,0.47 --legend-head-maxw 0.2 --legend-head-maxh 0.11 --no-spin-in-header  --x-range 450,4500 --label 2016_both_ --compare-labels 'Observed J=0, Observed J=2' --lumistr 12.9 $last_args

# exit 

./limit_plots.py $args --do-summary \
    --compare-file full_analysis_spring16v2_sync_v6_ichep_cic2016_default_shapes_spin0_parametric_v2_lumi_12.9/graphs_Asymptotic.root   --compare-label 'J=0' \
    --compare-file full_analysis_spring16v2_sync_v6_ichep_cic2016_default_shapes_spin2_parametric_v2_lumi_12.9/graphs_Asymptotic.root   --compare-label 'J=2' \
    -O ~/www/exo/spring16/$www/$target/ --legend 0.67,0.43,0.93,0.95 --x-range 450,4500  \
    --no-spin-in-header --load lumi_summary_13TeV.json  --saveas convert_png,pdf,root --label 2016 \
    --x-sections ../../MetaData/data/cross_sections_gen_grid.json --use-fb --use-mx --lumistr 12.9 $last_args

./limit_plots.py $args --do-summary \
    --compare-file combined_13TeV_8TeV_cert_ichep_parametic_v2_spin0/graphs_Asymptotic.root   --compare-label 'J=0' \
    --compare-file combined_13TeV_8TeV_cert_ichep_parametic_v2_spin2/graphs_Asymptotic.root   --compare-label 'J=2' \
    -O ~/www/exo/spring16/$www/$target/ --legend 0.67,0.43,0.93,0.95 --x-range 450,4500  \
    --no-spin-in-header --load lumi_summary_13TeV_8TeV.json  --saveas convert_png,pdf,root --label 13TeV_8TeV \
    --x-sections ../../MetaData/data/cross_sections_gen_grid.json --use-fb --use-mx --lumistr 16.2 $last_args

./limit_plots.py $args --do-summary \
    --compare-file combined_13TeV_cert_ichep_parametic_v2_spin0/graphs_Asymptotic.root   --compare-label 'J=0' \
    --compare-file combined_13TeV_cert_ichep_parametic_v2_spin2/graphs_Asymptotic.root   --compare-label 'J=2' \
    -O ~/www/exo/spring16/$www/$target/ --legend 0.67,0.43,0.93,0.95 --x-range 450,4500  \
    --no-spin-in-header --load lumi_summary_13TeV.json  --saveas convert_png,pdf,root --label 13TeV \
    --x-sections ../../MetaData/data/cross_sections_gen_grid.json --use-fb --use-mx --lumistr 16.2 $last_args

rm ~/www/exo/spring16/$www/$target/*summary*-[0-9].png

## exit

for spin in spin0 spin2; do 
    dir2015=combined_${spin}_wnuis_unblind
    dir2016=full_analysis_spring16v2_sync_v6_ichep_cic2016_default_shapes_${spin}_parametric_v2_lumi_12.9
    dirCmb13=combined_13TeV_cert_ichep_parametic_v2_${spin}

    
    ./limit_plots.py $args --do-comparison --compare-files $dirCmb13/graphs_ProfileLikelihood.root,$dir2015/graphs_ProfileLikelihood.root,$dir2016/graphs_ProfileLikelihood.root  --do-pvalues -O  ~/www/exo/spring16/$www/$target --load lumi_summary_spring16.json --legend 0.32,0.15,0.58,0.47 --${spin} --x-range 450,4500 --label comparison13TeV_${spin}_ --compare-labels 'Combined,2015(3.3fb^{-1}),2016(12.9fb^{-1})' --add-inset  --lumistr 16.2 $last_args
    
        
done


## exit

for spin in spin0 spin2; do 
    dirCmb=combined_13TeV_8TeV_cert_ichep_parametic_v2_${spin}
    dirCmb13=combined_13TeV_cert_ichep_parametic_v2_${spin}
    dir8=results_8TeV_${spin}
    ## dir2015=combined_${spin}_wnuis_unblind
    ## dir2016=full_analysis_spring16v2_sync_v6_ichep_cic2016_default_shapes_${spin}_parametric_v2_lumi_12.9
    
    ./limit_plots.py $args --do-comparison --compare-files $dirCmb/graphs_ProfileLikelihood.root,$dirCmb13/graphs_ProfileLikelihood.root,$dir8/graphs_ProfileLikelihood.root  --do-pvalues -O  ~/www/exo/spring16/$www/$target --load lumi_combined_spring16.json --legend 0.32,0.15,0.58,0.47 --${spin} --x-range 450,4500 --label comparison_${spin}_ --compare-labels 'Combined,13TeV(16.2fb^{-1}),8TeV(19.7fb^{-1})' --add-inset  --lumistr 16.2 $last_args
    
    
done


for spin in spin0 spin2; do 
    dirCmb=combined_13TeV_8TeV_cert_ichep_parametic_v2_${spin}
    dirCmb13=combined_13TeV_cert_ichep_parametic_v2_${spin}
    dir8=results_8TeV_${spin}
    ## dir2015=combined_${spin}_wnuis_unblind
    dir2016=full_analysis_spring16v2_sync_v6_ichep_cic2016_default_shapes_${spin}_parametric_v2_lumi_12.9

    ## ./limit_plots.py $args --do-comparison --compare-files $dirCmb13/graphs_ProfileLikelihood.root,$dirCmb13/graphs_freezeEScaleNuProfileLikelihood.root,$dirCmb13/graphs_freezeNuProfileLikelihood.root --do-pvalues -O  ~/www/exo/spring16/$www/$target --load lumi_combined_spring16.json --legend 0.32,0.15,0.58,0.47 --${spin} --x-range 450,4500 --label nuis_${spin}_ --compare-labels 'float,freeze E-scale,freeze E-scale and bias' --add-inset  --lumistr 16.2 $last_args

    ## ./limit_plots.py $args --do-comparison --compare-files $dir2016/graphs_ProfileLikelihood.root,$dir2016/graphs_freezeNuProfileLikelihood.root --do-pvalues -O  ~/www/exo/spring16/$www/$target --load lumi_combined_spring16.json --legend 0.32,0.15,0.58,0.47 --${spin} --x-range 450,4500 --label nuis_2016_${spin}_ --compare-labels 'float,freeze E-scale and bias' --add-inset  --lumistr 12.9 $last_args
    
    ./limit_plots.py $args --do-comparison --compare-files $dirCmb/graphs_ProfileLikelihood.root,$dirCmb13/graphs_ProfileLikelihood.root,$dir8/graphs_ProfileLikelihood.root  --do-pvalues -O  ~/www/exo/spring16/$www/$target --load lumi_combined_spring16.json --legend 0.32,0.15,0.58,0.47 --${spin} --x-range 450,4500 --label comparison_${spin}_ --compare-labels 'Combined,13TeV(16.2fb^{-1}),8TeV(19.7fb^{-1})' --add-inset  --lumistr 16.2 $last_args
    
    
done

