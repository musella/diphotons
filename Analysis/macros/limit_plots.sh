#!/bin/bash


target=$1 && shift

args="-U --x-sections ../../../../../EXO_7_4_15/src/diphotons/MetaData/data/cross_sections_gen_grid.json --fixed-x-section 1.02e-3 --use-fb --load lumi_limits.json --lumi 2.45"

www=$(echo $target | sed 's%_cic2.*%%')

./limit_plots.py --do-limits -M Asymptotic  $args --input-dir $target -O ~/www/exo/spring15_7415/$www/$target/limits

### ./limit_plots.py --do-pvalues -M ProfileLikelihood $args --input-dir $target -O ~/www/exo/spring15_7415/$www/$target/limits
### 
### for cat in EBEB EBEE; do
###     ./limit_plots.py --do-pvalues -M ProfileLikelihood $args --input-dir $target/$cat -O ~/www/exo/spring15_7415/$www/$target/limits/$cat --label $cat
### done 


## ./limit_plots.py --do-comparison --compare-files $target/graphs_ProfileLikelihood.root,$target/EBEB/graphs_ProfileLikelihood.root,$target/EBEE/graphs_ProfileLikelihood.root --do-pvalues --load lumi_limits.json --lumi 2.4 -O  ~/www/exo/spring15_7415/$www/$target/limits
