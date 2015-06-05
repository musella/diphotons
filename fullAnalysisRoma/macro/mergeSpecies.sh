#! /bin/sh 
# this scripts creates a merged root file in the self-created mergedFinal

mkdir -p data/mergedFinal

hadd data/mergedFinal/GGJets.root data/merged/GGJets_M-200To500.root data/merged/GGJets_M-500To1000.root data/merged/GGJets_M-1000To2000.root data/merged/GGJets_M-2000To4000.root data/merged/GGJets_M-4000To8000.root data/merged/GGJets_M-8000To13000.root
#
hadd data/mergedFinal/GJets.root data/merged/GJets_HT-100to200.root data/merged/GJets_HT-200to400.root data/merged/GJets_HT-400to600.root data/merged/GJets_HT-600toInf.root 
#
hadd data/mergedFinal/QCD.root data/merged/QCD_HT-100To250.root data/merged/QCD_HT-250To500.root data/merged/QCD_HT-500To1000.root data/merged/QCD_HT-1000ToInf.root
#
cp data/merged/RSGravToGG_kMpl-01_M-1500.root data/mergedFinal/RSGravToGG_kMpl-01_M-1500.root
cp data/merged/RSGravToGG_kMpl-01_M-3000.root data/mergedFinal/RSGravToGG_kMpl-01_M-3000.root
#
cp data/merged/RSGravToGG_kMpl-001_M-750.root data/mergedFinal/RSGravToGG_kMpl-001_M-750.root
cp data/merged/RSGravToGG_kMpl-001_M-1500.root data/mergedFinal/RSGravToGG_kMpl-001_M-1500.root
cp data/merged/RSGravToGG_kMpl-001_M-5000.root data/mergedFinal/RSGravToGG_kMpl-001_M-5000.root
#
cp data/merged/RSGravToGG_kMpl-02_M-1500.root data/mergedFinal/RSGravToGG_kMpl-02_M-1500.root
cp data/merged/RSGravToGG_kMpl-02_M-3000.root data/mergedFinal/RSGravToGG_kMpl-02_M-3000.root
cp data/merged/RSGravToGG_kMpl-02_M-5000.root data/mergedFinal/RSGravToGG_kMpl-02_M-5000.root

