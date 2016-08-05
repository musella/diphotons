
folder=$1 && shift 
coup=$1 && shift
mass=$1 && shift

## sig=${coup}_${mass}
sig=${coup}

cd $folder

text2workspace.py --PO verbose=10 -P HiggsAnalysis.CombinedLimit.PhysicsModel:multiSignalModel  --PO map=.*run015.*sig:r13TeV015[0,10] --PO map=".*run016.*sig:r13TeV016=expr::r13TeV016(\"@0+@1\",r13TeV015,deltaR[-6,6])" -m $mass datacard_${folder}_grav_${sig}.txt -o datacard_${folder}_grav_${sig}_deltaMu.root  -L libdiphotonsUtils 



combine -M MultiDimFit --algo=grid --points 200 --redefineSignalPOIs r13TeV015,deltaR -P deltaR  datacard_${folder}_grav_${sig}_deltaMu.root -n k_${coup}_deltaMu -L libdiphotonsUtils -m $mass --floatOtherPOIs=1 --minimizerStrategy 2 &



wait


