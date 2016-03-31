#!/bin/bash


target=$1 && shift

args="-U --fixed-x-section 1.e-3 --use-fb --load lumi_combined.json --saveas pdf,png,root --x-range 500,3500 $@"

if( echo $target | grep spin0 ); then 
    args="$args --spin0"
fi

## www=$(echo $target | sed 's%_cic.*%%')
www=limits_76_unblind

mkdir -p ~/www/exo/moriond16/$www/$target

./limit_plots.py --do-limits -M Asymptotic  $args --input-dir $target -O ~/www/exo/moriond16/$www/$target/limits

./limit_plots.py --do-pvalues -M ProfileLikelihood $args --legend 0.56,0.36,0.86,0.61  --y-range 1e-4,0.55  --input-dir $target -O ~/www/exo/moriond16/$www/$target/limits 


## ./limit_plots.py $args --do-comparison --compare-files $target/graphs_Asymptotic.root,$target/EBEB/graphs_Asymptotic.root,$target/EBEE/graphs_Asymptotic.root --load lumi_limits.json --lumi 2.56 -O  ~/www/exo/spring15_7415/$www/$target/limits/comparison --compare-expected -k 001,01,02


