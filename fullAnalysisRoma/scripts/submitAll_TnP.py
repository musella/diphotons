#
# usage: %prog [opts] --cfg cmssw.py dataset doPUreweighting(0/1) sampleIndex PUweightsFile x-section kFactor
#
# Backgrounds: sampleID>0 && sampleID<100
# Signal (DY): sampleID>100 && sampleID<10000
# Data:        sampleID>10000

# DY
./submitBatchTnP.py --cfg tnpAnaBATCH.py DYLL 0 101 pippo 2008.4  1 

# Data
./submitBatchTnP.py --cfg tnpAnaBATCH.py DoubleEG  0 10001 pippo 1  1 

