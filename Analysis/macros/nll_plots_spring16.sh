
www=limits_12p9invfb
target=comparison

spin0dir=combined_13TeV_8TeV_cert_ichep_parametic_v2_spin0
## spin2dir=combined_13TeV_8TeV_cert_275125_spin2

coup=001
mass=750

### ./muscan.sh $spin0dir $coup $mass & 
### ./muscan.sh $spin2dir $coup $mass &
### 
### wait


./limit_plots.py --do-nll \
    --compare-file ${spin0dir}/higgsCombinek_${coup}_rComb.MultiDimFit.mH${mass}.root  \
    --compare-file combined_13TeV_8TeV_cert_275125_spin0/higgsCombinek_${coup}_r13TeV015.MultiDimFit.mH${mass}.root \
    --compare-file ${spin0dir}/higgsCombinek_${coup}_r13TeV016.MultiDimFit.mH${mass}.root \
    --compare-file ${spin0dir}/higgsCombinek_${coup}_r8TeV.MultiDimFit.mH${mass}.root  \
    --compare-labels 'Combined,13TeV 2015(3.3fb^{-1}),13TeV 2016(12.9fb^{-1}),8TeV(19.7fb^{-1})' --compare-obs 'r,r13TeV015,r13TeV016,r8TeV' \
    --x-range -1.7,10 --label spin0_${mass}_${coup} --y-range 0,4 \
    --load lumi_combined.json --legend 0.5,0.55,0.8,0.85 -k ${coup} --spin0 \
    --lumistr 16.2 \
    -O ~/www/exo/spring16/$www/$target --saveas png,pdf,root

### ./limit_plots.py --do-nll \
###     --compare-file ${spin2dir}/higgsCombinek_${coup}_rComb.MultiDimFit.mH${mass}.root  \
###     --compare-file ${spin2dir}/higgsCombinek_${coup}_r13TeV015.MultiDimFit.mH${mass}.root \
###     --compare-file ${spin2dir}/higgsCombinek_${coup}_r13TeV016.MultiDimFit.mH${mass}.root \
###     --compare-file ${spin2dir}/higgsCombinek_${coup}_r8TeV.MultiDimFit.mH${mass}.root  \
###     --compare-labels Combined,2015,2016,8TeV --compare-obs r,r13TeV015,r13TeV016,r8TeV \
###     --x-range -2.5,10 --label spin2_${mass}_${coup} --y-range 0,6 \
###     --load lumi_combined_internal.json --legend 0.2,0.55,0.49,0.85 -k ${coup} --spin2 \
###     --lumi 7.3 \
###     -O ~/www/exo/spring16/$www/$target --saveas png,pdf,root


exit

spin0dir=combined_13TeV_cert_ichep_parametic_v2_spin0
## spin2dir=combined_13TeV_cert_275125_spin2

coup=001
mass=750

### ./muscan.sh $spin0dir $coup $mass & 
### ./muscan.sh $spin2dir $coup $mass &
### 
### wait


./limit_plots.py --do-nll \
    --compare-file ${spin0dir}/higgsCombinek_${coup}_deltaMu.MultiDimFit.mH${mass}.root \
    --compare-labels '#sigma_{2015}-#sigma_{2016}' --compare-obs '-deltaR' \
    --x-range -6,6 --label spin0_${mass}_${coup}_deltaR13 --y-range 0,6 \
    --load lumi.json --legend 0.2,0.35,0.49,0.65 -k ${coup} --spin0 \
    --lumistr 16.2 \
    -O ~/www/exo/spring16/$www/$target --saveas png,pdf,root

## ./limit_plots.py --do-nll \
##     --compare-file ${spin2dir}/higgsCombinek_${coup}_deltaMu.MultiDimFit.mH${mass}.root  \
##     --compare-labels '#sigma_{2015}-#sigma_{2016}' --compare-obs '-deltaR' \
##     --x-range -6,6 --label spin2_${mass}_${coup}_deltaR13 --y-range 0,6 \
##     --load lumi_internal.json --legend 0.2,0.35,0.49,0.65 -k ${coup} --spin2 \
##     --lumi 7.3 \
##     -O ~/www/exo/spring16/$www/$target --saveas png,pdf,root


