
folder=$1 && shift 
coup=$1 && shift
mass=$1 && shift

sig=${coup}_${mass}

cd $folder

## text2workspace.py -P HiggsAnalysis.CombinedLimit.PhysicsModel:multiSignalModel --PO map=.*ch1_.*sig:r38T[0,10] --PO map=.*ch2_.*sig:r0T[0,10] -m $mass datacard_${folder}_grav_${sig}.txt -o datacard_${folder}_grav_${sig}_twomus.root  -L libdiphotonsUtils 
## 
## combine -M MultiDimFit --algo=grid --points 200 -P r38T  datacard_${folder}_grav_${sig}_twomus.root -n k_${coup}_r38T -L libdiphotonsUtils -m $mass --floatOtherPOIs=1 --minimizerStrategy 2 &
## combine -M MultiDimFit --algo=grid --points 200 -P r0T  datacard_${folder}_grav_${sig}_twomus.root -n k_${coup}_r0T -L libdiphotonsUtils -m $mass --floatOtherPOIs=1 --minimizerStrategy 2 &


combine -M MultiDimFit --algo=grid --points 200 -P r --setPhysicsModelParameterRanges r=0,10  datacard_${folder}_grav_${sig}.txt -n k_${coup}_rComb -L libdiphotonsUtils -m $mass --minimizerStrategy 2 &


combine -M MultiDimFit -P r --setPhysicsModelParameterRanges r=0,10  datacard_${folder}_grav_${sig}.txt -n k_${coup}_bestFit -L libdiphotonsUtils -m $mass --minimizerStrategy 2 --saveWorkspace &

wait


