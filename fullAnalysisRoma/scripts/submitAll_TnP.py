#
# usage: %prog [opts] --cfg cmssw.py dataset doPUreweighting(0/1) sampleIndex PUweightsFile x-section kFactor
#
# Backgrounds: sampleID>0 && sampleID<100
# Signal (DY): sampleID>100 && sampleID<10000
# Data:        sampleID>10000

# MC DY signal
./submitBatchTnP.py --cfg tnpAnaBATCH.py DYLL 0 101 pippo 5990  1 

# MC background
./submitBatchTnP.py --cfg tnpAnaBATCH.py WJetsToLNu_HT-600To800   0 1 pippo 12.26  1 
./submitBatchTnP.py --cfg tnpAnaBATCH.py WJetsToLNu_HT-800To1200  0 2 pippo 5.557  1 
./submitBatchTnP.py --cfg tnpAnaBATCH.py WJetsToLNu_HT-1200To2500 0 3 pippo 1.302  1 
./submitBatchTnP.py --cfg tnpAnaBATCH.py WJetsToLNu_HT-2500ToInf  0 4 pippo 0.02865 1 
./submitBatchTnP.py --cfg tnpAnaBATCH.py WW2L2nu                  0 5 pippo 10.48  1 
./submitBatchTnP.py --cfg tnpAnaBATCH.py WZjets                   0 6 pippo 5.257  1 
#./submitBatchTnP.py --cfg tnpAnaBATCH.py ZZ4l                     0 7 pippo 1.256  1 
#./submitBatchTnP.py --cfg tnpAnaBATCH.py TTjets                   0 8 pippo 509.1  1 

# Data
##./submitBatchTnP.py --cfg tnpAnaBATCH.py singleEle  0 10001 pippo 1  1 
##./submitBatchTnP.py --cfg tnpAnaBATCH.py singleEle2015D 0 10001 pippo 1  1
