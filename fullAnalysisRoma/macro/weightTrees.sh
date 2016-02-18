#! /bin/sh                                                                                                    

lumi=$1      # in pb
echo "Adding weights for " $lumi " pb-1..."

root -l -b <<EOF
.L addWeightsToTree.cc+   
addWeights("data/merged/GGJets_M-200To500.root", $lumi);
addWeights("data/merged/GGJets_M-500To1000.root", $lumi);
addWeights("data/merged/GGJets_M-1000To2000.root", $lumi);
addWeights("data/merged/GGJets_M-2000To4000.root", $lumi);
addWeights("data/merged/GGJets_M-4000To6000.root", $lumi);
addWeights("data/merged/GGJets_M-6000To8000.root", $lumi);
addWeights("data/merged/GGJets_M-8000To13000.root", $lumi);

addWeights("data/merged/GJets_HT-100to200.root", $lumi);
addWeights("data/merged/GJets_HT-400to600.root", $lumi);
addWeights("data/merged/GJets_HT-600toInf.root", $lumi);

addWeights("data/merged/QCD_HT-200To300.root", $lumi);    
addWeights("data/merged/QCD_HT-300To500.root", $lumi);    
addWeights("data/merged/QCD_HT-500To700.root", $lumi);    
addWeights("data/merged/QCD_HT-700To1000.root", $lumi);    
addWeights("data/merged/QCD_HT-1000To1500.root", $lumi);    

addWeights("data/merged/RSGravToGG_kMpl-001_M-500.root",  $lumi, 500);
addWeights("data/merged/RSGravToGG_kMpl-001_M-2000.root",  $lumi, 2000);
addWeights("data/merged/RSGravToGG_kMpl-001_M-3000.root",  $lumi, 3000);

addWeights("data/merged/RSGravToGG_kMpl-01_M-1000.root",  $lumi, 1000);
addWeights("data/merged/RSGravToGG_kMpl-01_M-3500.root",  $lumi, 3500);
addWeights("data/merged/RSGravToGG_kMpl-01_M-4000.root",  $lumi, 4000);
addWeights("data/merged/RSGravToGG_kMpl-01_M-5000.root",  $lumi, 5000);

addWeights("data/merged/RSGravToGG_kMpl-02_M-1000.root",  $lumi, 1000);
addWeights("data/merged/RSGravToGG_kMpl-02_M-4000.root",  $lumi, 4000);
addWeights("data/merged/RSGravToGG_kMpl-02_M-5000.root",  $lumi, 5000);

addWeights("data/merged/DoubleEG.root",  $lumi);  
addWeights("data/merged/singlePhoton.root",  $lumi);  
.q

EOF

echo "done weighting."
