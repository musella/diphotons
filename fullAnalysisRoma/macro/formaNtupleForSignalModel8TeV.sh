#! /bin/sh    

root -l -b <<EOF
.L signalModelFormatting8TeV.cc++

signalModelFormat8TeV("data8TeV/diphoton_tree_RSGravToGG_kMpl-001_M-750_TuneZ2star_8TeV-pythia6_merged.root","001","750")
signalModelFormat8TeV("data8TeV/diphoton_tree_RSGravToGG_kMpl-001_M-1000_TuneZ2star_8TeV-pythia6_merged.root","001","1000")
signalModelFormat8TeV("data8TeV/diphoton_tree_RSGravToGG_kMpl-001_M-1250_TuneZ2star_8TeV-pythia6_merged.root","001","1250")
signalModelFormat8TeV("data8TeV/diphoton_tree_RSGravToGG_kMpl-001_M-1500_TuneZ2star_8TeV-pythia6_merged.root","001","1500")
signalModelFormat8TeV("data8TeV/diphoton_tree_RSGravToGG_kMpl-001_M-1750_TuneZ2star_8TeV-pythia6_merged.root","001","1750")
signalModelFormat8TeV("data8TeV/diphoton_tree_RSGravToGG_kMpl-001_M-2000_TuneZ2star_8TeV-pythia6_merged.root","001","2000")
signalModelFormat8TeV("data8TeV/diphoton_tree_RSGravToGG_kMpl-001_M-2250_TuneZ2star_8TeV-pythia6_merged.root","001","2250")
signalModelFormat8TeV("data8TeV/diphoton_tree_RSGravToGG_kMpl-001_M-2500_TuneZ2star_8TeV-pythia6_merged.root","001","2500")
signalModelFormat8TeV("data8TeV/diphoton_tree_RSGravToGG_kMpl-001_M-3000_TuneZ2star_8TeV-pythia6_merged.root","001","3000")

signalModelFormat8TeV("data8TeV/diphoton_tree_RSGravToGG_kMpl-005_M-2750_TuneZ2star_8TeV-pythia6_merged.root","005","2750")

signalModelFormat8TeV("data8TeV/diphoton_tree_RSGravToGG_kMpl-01_M-3250_TuneZ2star_8TeV-pythia6_merged.root","01","3250")
signalModelFormat8TeV("data8TeV/diphoton_tree_RSGravToGG_kMpl-01_M-3500_TuneZ2star_8TeV-pythia6_merged.root","01","3500")

.q

EOF 

echo "done"
