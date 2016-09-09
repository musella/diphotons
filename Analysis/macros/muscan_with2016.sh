
folder=$1 && shift 
coup=$1 && shift
mass=$1 && shift

sig=${coup}_${mass}

cd $folder



### text2workspace.py -P HiggsAnalysis.CombinedLimit.PhysicsModel:multiSignalModel --PO map=.*sqrts8TeV.*sig:r8TeV[0,10] --PO map=.*run015.*sig:r13TeV015[0,10] --PO map=.*run016.*sig:r13TeV016[-2,10] -m $mass datacard_${folder}_grav_${sig}.txt -o datacard_${folder}_grav_${sig}_threemus.root  -L libdiphotonsUtils 
### 
### combine -M MultiDimFit  datacard_${folder}_grav_${sig}_threemus.root -n k_${coup}_bestFit_threemus -L libdiphotonsUtils -m $mass --minimizerStrategy 2 --saveWorkspace --saveNLL 
### 
### cp -p higgsCombinek_${coup}_bestFit_threemus.MultiDimFit.mH${mass}.root datacard_${folder}_grav_${sig}_threemus_post_fit.root
### 
### 
### 
### combine -M MultiDimFit --algo=grid --points 200 -P r13TeV015  datacard_${folder}_grav_${sig}_threemus_post_fit.root --snapshotName MultiDimFit -n k_${coup}_r13TeV015 -L libdiphotonsUtils -m $mass --floatOtherPOIs=1 --minimizerStrategy 2 --freezeNuisances thetaBias_bkg_EBEB0T,thetaBias_bkg_EBEE0T,thetaBias_bkg_EBEB,thetaBias_bkg_EBEE  --protectUnbinnedChannels &
### combine -M MultiDimFit --algo=grid --points 200 -P r13TeV016  datacard_${folder}_grav_${sig}_threemus_post_fit.root --snapshotName MultiDimFit -n k_${coup}_r13TeV016 -L libdiphotonsUtils -m $mass --floatOtherPOIs=1 --minimizerStrategy 2 --freezeNuisances thetaBias_bkg_EBEB0T,thetaBias_bkg_EBEE0T,thetaBias_bkg_EBEB,thetaBias_bkg_EBEE --protectUnbinnedChannels &
### combine -M MultiDimFit --algo=grid --points 200 -P r8TeV    datacard_${folder}_grav_${sig}_threemus_post_fit.root --snapshotName MultiDimFit -n k_${coup}_r8TeV -L libdiphotonsUtils -m $mass --floatOtherPOIs=1 --minimizerStrategy 2 --freezeNuisances thetaBias_bkg_EBEB0T,thetaBias_bkg_EBEE0T,thetaBias_bkg_EBEB,thetaBias_bkg_EBEE --protectUnbinnedChannels &



combine -M MultiDimFit --algo=grid --points 70 -P r --setPhysicsModelParameterRanges r=-0.5,3  datacard_${folder}_grav_${sig}.txt -n k_${coup}_rComb -L libdiphotonsUtils -m $mass --minimizerStrategy 2 &

### combine -M MultiDimFit -P r --setPhysicsModelParameterRanges r=0,10  datacard_${folder}_grav_${sig}.txt -n k_${coup}_bestFit -L libdiphotonsUtils -m $mass --minimizerStrategy 2 --saveWorkspace --saveNLL &





wait


