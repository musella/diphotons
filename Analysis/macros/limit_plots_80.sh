#!/bin/bash


target=$1 && shift
if [[ -n $1 ]]; then 
    lumi=$1
else
    lumi=$(echo $target | sed 's%.*lumi_%%')
fi

##args="-U --x-sections ../../../../../EXO_7_4_15/src/diphotons/MetaData/data/cross_sections_gen_grid.json --fixed-x-section 0.9375e-3 --use-fb --load lumi_limits.json --lumi 2.56 --saveas pdf,convert_png,root"
args="-U --fixed-x-section 1.e-3 --use-fb --load lumi_internal.json --lumistr $lumi --saveas pdf,png,root --use-mx"

if( echo $target | grep spin0 ); then 
    args="$args --spin0"
else
    args="$args --x-sections ../../MetaData/data/cross_sections_gen_grid.json"
fi

## www=$(echo $target | sed 's%_cic.*%%')
www=limits

mkdir -p ~/www/exo/spring16/$www/$target

### args="--blind --fixed-x-section 1.e-3 --use-fb --load lumi_internal.json --lumi $lumi --saveas pdf,png,root "
### ./limit_plots.py --do-limits -M Asymptotic  $args --input-dir $target -O ~/www/exo/spring16/$www/$target/expected_limits
### 
### exit

set -x
./limit_plots.py --do-limits -M Asymptotic  $args --input-dir $target -O ~/www/exo/spring16/$www/$target/limits --cleanup -k 01,02
./limit_plots.py --do-limits -M Asymptotic  $args --input-dir $target -O ~/www/exo/spring16/$www/$target/limits_smoothen --smoothen --cleanup -k 001

hadd -f $target/graphs_Asymptotic.root $target/graphs_01_02_Asymptotic.root $target/graphs_001_Asymptotic.root

## ./limit_plots.py --do-limits -M Asymptotic  $args --label minimStrat2 --input-dir $target -O ~/www/exo/spring16/$www/$target/limits_minimStrat2 --smoothen
## ./limit_plots.py --do-limits -M Asymptotic  $args --label minimStrat2 --input-dir $target -O ~/www/exo/spring16/$www/$target/limits_minimStrat2
## ./limit_plots.py --do-limits -M Asymptotic  $args --label freezeEScaleNu --input-dir $target -O ~/www/exo/spring16/$www/$target/limits_minimStrat2

./limit_plots.py --do-pvalues -M ProfileLikelihood $args --input-dir $target -O ~/www/exo/spring16/$www/$target/limits
## ./limit_plots.py --do-pvalues -M ProfileLikelihood $args --input-dir $target --label freezeNu -O ~/www/exo/spring16/$www/$target/limits_freezeNu
## ./limit_plots.py --do-pvalues -M ProfileLikelihood $args --input-dir $target --label freezeEScaleNu -O ~/www/exo/spring16/$www/$target/limits_freezeNu


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


