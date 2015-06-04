#! /bin/sh                                                                                                    

lumi=$1      # in pb
echo "Adding weights for " $lumi " pb-1..."

root -l -b <<EOF
.L addWeightsToTree.cc+   
addWeights("data/merged/GGJets_M-200To500.root", $lumi);
addWeights("data/merged/GGJets_M-500To1000.root", $lumi);
addWeights("data/merged/GGJets_M-1000To2000.root", $lumi);
addWeights("data/merged/GGJets_M-2000To4000.root", $lumi);
addWeights("data/merged/GGJets_M-4000To8000.root", $lumi);
addWeights("data/merged/GGJets_M-8000To13000.root", $lumi);
addWeights("data/merged/GJets_HT-100to200.root", $lumi);
addWeights("data/merged/GJets_HT-200to400.root", $lumi);
addWeights("data/merged/GJets_HT-400to600.root", $lumi);
addWeights("data/merged/GJets_HT-600toInf.root", $lumi);
addWeights("data/merged/QCD_HT-100To250.root", $lumi);    
addWeights("data/merged/QCD_HT-250To500.root", $lumi);   
addWeights("data/merged/QCD_HT-500To1000.root", $lumi);   
addWeights("data/merged/QCD_HT-1000ToInf.root", $lumi);   
addWeights("data/merged/RSGravToGG_kMpl-001_M-750.root",  $lumi, 750);
addWeights("data/merged/RSGravToGG_kMpl-001_M-1500.root", $lumi, 1500);
addWeights("data/merged/RSGravToGG_kMpl-001_M-5000.root", $lumi, 5000);
addWeights("data/merged/RSGravToGG_kMpl-01_M-1500.root", $lumi, 1500);
addWeights("data/merged/RSGravToGG_kMpl-01_M-3000.root", $lumi, 3000);
addWeights("data/merged/RSGravToGG_kMpl-02_M-1500.root", $lumi, 1500);
addWeights("data/merged/RSGravToGG_kMpl-02_M-3000.root", $lumi, 3000);
addWeights("data/merged/RSGravToGG_kMpl-02_M-5000.root", $lumi, 5000);
.q

EOF

echo "done weighting."
