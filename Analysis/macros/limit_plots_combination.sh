#!/bin/bash


target=$1 && shift

args="-U --fixed-x-section 1.e-3 --use-fb --load lumi_combined.json --saveas pdf,png,root --x-range 450,4500 --use-mx $@"

if( echo $target | grep spin0 ); then 
    args="$args --spin0"
else
    args="$args --x-sections ../../MetaData/data/cross_sections_gen_grid.json"
fi

## www=$(echo $target | sed 's%_cic.*%%')
www=limits

mkdir -p ~/www/exo/spring16/$www/$target

./limit_plots.py --do-limits -M Asymptotic  $args --input-dir $target -O ~/www/exo/spring16/$www/$target/limits --cleanup -k 01,02

./limit_plots.py --do-limits -M Asymptotic  $args --input-dir $target -O ~/www/exo/spring16/$www/$target/limits_smoothen --smoothen --smoothen-boundary 850. --cleanup -k 001

hadd -f $target/graphs_Asymptotic.root $target/graphs_01_02_Asymptotic.root $target/graphs_001_Asymptotic.root


./limit_plots.py --do-pvalues -M ProfileLikelihood $args --legend 0.56,0.36,0.86,0.61  --y-range 1e-4,0.55  --input-dir $target -O ~/www/exo/spring16/$www/$target/limits 


## ./limit_plots.py $args --do-comparison --compare-files $target/graphs_Asymptotic.root,$target/EBEB/graphs_Asymptotic.root,$target/EBEE/graphs_Asymptotic.root --load lumi_limits.json --lumi 2.56 -O  ~/www/exo/spring15_7415/$www/$target/limits/comparison --compare-expected -k 001,01,02


