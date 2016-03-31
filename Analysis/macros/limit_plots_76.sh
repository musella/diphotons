#!/bin/bash


target=$1 && shift
if [[ -n $1 ]]; then 
    lumi=$1
else
    lumi=$(echo $target | sed 's%.*lumi_%%')
fi

## args="-U --x-sections ../../../../../EXO_7_4_15/src/diphotons/MetaData/data/cross_sections_gen_grid.json --fixed-x-section 0.9375e-3 --use-fb --load lumi_limits.json --lumi 2.56 --saveas pdf,convert_png,root"
args="-U --fixed-x-section 1.e-3 --use-fb --load lumi_limits.json --lumi $lumi --saveas pdf,png,root "

if( echo $target | grep spin0 ); then 
    args="$args --spin0"
fi

## www=$(echo $target | sed 's%_cic.*%%')
www=limits_76_unblind

mkdir -p ~/www/exo/moriond16/$www/$target

./limit_plots.py --do-limits -M Asymptotic  $args --input-dir $target -O ~/www/exo/moriond16/$www/$target/limits

./limit_plots.py --do-pvalues -M ProfileLikelihood $args --input-dir $target -O ~/www/exo/moriond16/$www/$target/limits

### ###  
### ### for cat in EBEB EBEE; do
### ###     ./limit_plots.py --do-pvalues -M ProfileLikelihood $args --input-dir $target/$cat -O ~/www/exo/spring15_7415/$www/$target/limits/$cat --label $cat
### ### done 
### ### 
### ### 
## d.root --do-pvalues --load lumi_limits.json --lumi 2.56 -O  ~/www/exo/spring15_7415/$www/$target/limits -k 001,01,02
### 
### ### for cat in EBEB EBEE; do
### ###     ./limit_plots.py --do-limits -M Asymptotic $args --input-dir $target/$cat -O ~/www/exo/spring15_7415/$www/$target/limits/$cat --label $cat
### ### done 


## ./limit_plots.py $args --do-comparison --compare-files $target/graphs_Asymptotic.root,$target/EBEB/graphs_Asymptotic.root,$target/EBEE/graphs_Asymptotic.root --load lumi_limits.json --lumi 2.56 -O  ~/www/exo/spring15_7415/$www/$target/limits/comparison --compare-expected -k 001,01,02


