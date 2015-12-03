#!/bin/bash

set -x

output=pdf_uncertainties
mkdir $output

### ./auto_plotter.py --selection cicGenIso,cicNonGenIso \
###     --move cicGenIso:cic,cicNonGenIso:cic \
###     --file  /afs/cern.ch/user/m/musella/public/workspace/exo/output_PDFCTEQ6L.root  \
###     --cut "pdfWeight_0 > 0." \
###     --histograms "pdfWeight_0>>pdfWeightCTEQ6L(100,0,4)" \
###     --output $output/output_CTEQ6L_sumIso.root 

### ## ./signal_norm.py --reco-file $output/output_CTEQ6L_sumIso.root --plot-pdfs

./auto_plotter.py --selection cicGenIso,cicNonGenIso \
    --move cicGenIso:cic,cicNonGenIso:cic \
    --file  /afs/cern.ch/user/m/musella/public/workspace/exo/output_MSTW2008lo68cl.root  \
    --cut "pdfWeight_0 > 0." \
    --histograms "pdfWeight_0>>pdfWeightMSTW2008(100,0,4)" \
    --output $output/output_MSTW2008_sumIso.root 

hadd $output/output.root  $output/output_MSTW2008_sumIso.root  $output/output_CTEQ6L_sumIso.root 

./signal_norm.py --reco-file $output/output.root --plot-pdfs
