#!/bin/bash

set -x 

src=$1 && shift
www=~/www/exo/spring15_v850ns
label=""

[ -n "$1" ] && www=$1 && shift
[ -n "$1" ] && label=$1 && shift


target=$(basename $src)
[[ -n "$label" ]] && target=${target}_${label}

[[ ! -d $www ]] && mkdir  $www/
mkdir  $www/$target/

mkdir $www/$target/tune

wwbase=$www/$target/tune

### ./idEvolution.py --load idEvolution_mkWeights.json -O ${wwbase}/mkWei  -i ${src}/output.root/photonDumper/trees --weight 'weight*(pt>75 && pt<1000)'
### mv ${wwbase}/mkWei/store.root ${src}/${label}rewei.root
### 
### ./idEvolution.py --load idEvolution_applyWeights.json -O ${wwbase}/appWei  -i ${src}/output.root/photonDumper/trees  --weight 'weight*(pt>75 && pt<1000)' --reweight ${src}/${label}rewei.root::absScEta
### mv  ${wwbase}/appWei/wei.root ${src}/${label}wei.root
### 
### ./idEvolution.py --load idEvolution_rho_corr.json -O ${wwbase}/rhoCorr  -i ${src}/output.root/photonDumper/trees  --weight 'weight*(pt>75 && pt<1000)'
### 
### cat >> ${src}/${label}eff_areas.json <<EOF
### {
###    "aliases" : [
### 
###    ]
### }
### EOF
### emacs -nw ${wwbase}/rhoCorr/README.txt ${src}/${label}eff_areas.json
### 
### ./idEvolution.py --load idEvolution_pt_dep.json --load ${src}/${label}eff_areas.json  -O ${wwbase}/ptDep  -i ${src}/output.root/photonDumper/trees  --weight 'weight*(pt>75 && pt<1000)' --wread ${src}/${label}wei.root --binningMaxErr 0.1 --binningTarget 100 --fit-expression "[0]+[1]*log(x)"

### ./idEvolution.py --load idEvolution_pt_dep.json --load ${src}/${label}eff_areas.json  -O ${wwbase}/ptLinDep  -i ${src}/output.root/photonDumper/trees  --weight 'weight*(pt>75 && pt<1000)' --binningMaxErr 0.1 --binningTarget 100 --fit-expression "pol1"
### 
### ./idEvolution.py --load idEvolution_pt_dep.json --load ${src}/${label}eff_areas.json  -O ${wwbase}/ptLinSplitDep  -i ${src}/output.root/photonDumper/trees  --weight 'weight*(pt>75 && pt<1000)' --binningMaxErr 0.1 --binningTarget 100 --fit-expression "pol1" \
###     --categories 'isEBHighR9 := abs(scEta)<1.5 && r9>=0.94' \
###     --categories 'isEEHighR9 := abs(scEta)>1.5 && r9>=0.94' \
###     --categories 'isEBLowR9  := abs(scEta)<1.5 && r9<0.94' \
###     --categories 'isEELowR9  := abs(scEta)>1.5 && r9<0.94'

## emacs -nw ${wwbase}/ptDep/README.txt ${src}/${label}eff_areas.json


### ./idEvolution.py --load idEvolution_isolations.json --load ${src}/${label}eff_areas.json  -O ${wwbase}/unCorrectedVars  -i ${src}/output.root/photonDumper/trees  --weight 'weight*(pt>75 && pt<1000)' --wread ${src}/${label}wei.root --pt-corr-var 'log(pt)' --no-pt-corr --no-rho-corr --no-offset-corr
### 
### ./idEvolution.py --load idEvolution_isolations.json --load ${src}/${label}eff_areas.json  -O ${wwbase}/rhoCorrectedVars  -i ${src}/output.root/photonDumper/trees  --weight 'weight*(pt>75 && pt<1000)' --wread ${src}/${label}wei.root --pt-corr-var 'log(pt)' --no-pt-corr --no-offset-corr
### 
### ./idEvolution.py --load idEvolution_isolations.json --load ${src}/${label}eff_areas.json  -O ${wwbase}/correctedVars  -i ${src}/output.root/photonDumper/trees  --weight 'weight*(pt>75 && pt<1000)' --wread ${src}/${label}wei.root --pt-corr-var 'log(pt)'

### ./idEvolution.py --load idEvolution_isolations.json --load ${src}/${label}eff_areas.json  -O ${wwbase}/ptCorrectedVars  -i ${src}/output.root/photonDumper/trees  --weight 'weight*(pt>75 && pt<1000)' --wread ${src}/${label}wei.root --pt-corr-var 'log(pt)' --no-offset-corr

set -x

./idEvolution.py --load idEvolution_isolations.json --load ${src}/${label}eff_areas.json  -O ${wwbase}/ptCorrectedVars  -i ${src}/output.root/photonDumper/trees  --weight 'weight*(pt>75 && pt<1000)' --wread ${src}/${label}wei.root --pt-corr-var pt --no-offset-corr



### ./idEvolution.py --load idEvolution_isolations.json --load ${src}/${label}eff_areas.json  -O ${wwbase}/offset  -i ${src}/output.root/photonDumper/trees  --weight 'weight*(pt>75 && pt<1000)' --wread ${src}/${label}wei.root --pt-corr-var 'log(pt)' --fit-median
###
### emacs -nw ${wwbase}/offset/README.txt ${src}/${label}eff_areas.json

