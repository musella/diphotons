
folder=$1 && shift 
coup=$1 && shift
mass=$1 && shift

sig=${coup}_${mass}

cd $folder


text2workspace.py -P HiggsAnalysis.CombinedLimit.PhysicsModel:multiSignalModel --PO map=".*sqrts8TeV.*sig:r8TeV=expr::r8TeV(\"@0+@1\",r13TeV015,deltaR8TeV[-6,6])" --PO map=.*run015.*sig:r13TeV015[0,10] --PO map=".*run016.*sig:r13TeV016=expr::r13TeV016(\"@0+@1\",r13TeV015,deltaR016[-6,6])" -m $mass datacard_${folder}_grav_${sig}.txt -o datacard_${folder}_grav_${sig}_deltaMu2.root  -L libdiphotonsUtils 


combine -M MultiDimFit  --redefineSignalPOIs r13TeV015,deltaR8TeV,deltaR016 datacard_${folder}_grav_${sig}_deltaMu2.root -n k_${coup}_bestFit_deltaMu2 -L libdiphotonsUtils -m $mass --minimizerStrategy 2 --saveWorkspace --saveNLL

combine -M MultiDimFit  --redefineSignalPOIs r13TeV015,deltaR8TeV,deltaR016  --setPhysicsModelParameters deltaR8TeV=0,deltaR016=0 --freezeNuisances deltaR8TeV,deltaR016  datacard_${folder}_grav_${sig}_deltaMu2.root -n k_${coup}_bestFit_deltaMu2_0 -L libdiphotonsUtils -m $mass --minimizerStrategy 2 --saveWorkspace --saveNLL



wait


