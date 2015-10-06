#
# usage: %prog [opts] --cfg cmssw.py dataset doPUreweighting(0/1) sampleIndex PUweightsFile x-section kFactor
#
# Backgrounds: sampleID>0 && sampleID<100
# Signal (DY): sampleID>100 && sampleID<10000
# Data:        sampleID>10000

# MC DY signal
#./submitBatchTnP.py --cfg tnpAnaBATCH.py DYLL 0 101 pippo 5990  1 

# MC specific background
#./submitBatchTnP.py --cfg tnpAnaBATCH.py WJetsToLNu_HT-600To800   0 1 pippo 12.26  1 
#./submitBatchTnP.py --cfg tnpAnaBATCH.py WJetsToLNu_HT-800To1200  0 2 pippo 5.557  1 
#./submitBatchTnP.py --cfg tnpAnaBATCH.py WJetsToLNu_HT-1200To2500 0 3 pippo 1.302  1 
#./submitBatchTnP.py --cfg tnpAnaBATCH.py WJetsToLNu_HT-2500ToInf  0 4 pippo 0.02865 1 
#./submitBatchTnP.py --cfg tnpAnaBATCH.py WW2L2nu                  0 5 pippo 10.48  1 
#./submitBatchTnP.py --cfg tnpAnaBATCH.py WZjets                   0 6 pippo 5.257  1 
#./submitBatchTnP.py --cfg tnpAnaBATCH.py ZZ4l                     0 7 pippo 1.256  1 
#./submitBatchTnP.py --cfg tnpAnaBATCH.py TTjets                   0 8 pippo 509.1  1 

# other MC backgrounds: gg
#./submitBatchTnP.py --cfg tnpAnaBATCH.py GGJets_M-200To500    0 9  pippo 2.047e+00 1
#./submitBatchTnP.py --cfg tnpAnaBATCH.py GGJets_M-500To1000   0 10 pippo 1.510e-01 1
#./submitBatchTnP.py --cfg tnpAnaBATCH.py GGJets_M-1000To2000  0 11 pippo 1.084e-02 1
#./submitBatchTnP.py --cfg tnpAnaBATCH.py GGJets_M-2000To4000  0 12 pippo 3.690e-04 1
#./submitBatchTnP.py --cfg tnpAnaBATCH.py GGJets_M-4000To6000  0 13 pippo 2.451e-06 1
#./submitBatchTnP.py --cfg tnpAnaBATCH.py GGJets_M-6000To8000  0 14 pippo 1.753e-08 1
#./submitBatchTnP.py --cfg tnpAnaBATCH.py GGJets_M-8000To13000 0 15 pippo 7.053e-11 1

# other MC backgrounds: g+jets
#./submitBatchTnP.py --cfg tnpAnaBATCH.py GJets_HT-100to200  0 16 pippo 9.863e+03 1
#./submitBatchTnP.py --cfg tnpAnaBATCH.py GJets_HT-200to400  0 17 pippo 2.298e+03 1  
#./submitBatchTnP.py --cfg tnpAnaBATCH.py GJets_HT-400to600  0 18 pippo 2.816e+02 1
#./submitBatchTnP.py --cfg tnpAnaBATCH.py GJets_HT-600toInf  0 19 pippo 9.465e+01 1

# other MC backgrounds: QCD 
#./submitBatchTnP.py --cfg tnpAnaBATCH.py QCD_HT-100To200   0 20 pippo 2.803e+07 1
#./submitBatchTnP.py --cfg tnpAnaBATCH.py QCD_HT-200To300   0 21 pippo 1.708e+06 1
#./submitBatchTnP.py --cfg tnpAnaBATCH.py QCD_HT-300To500   0 22 pippo 3.509e+05 1
#./submitBatchTnP.py --cfg tnpAnaBATCH.py QCD_HT-500To700   0 23 pippo 3.196e+04 1
#./submitBatchTnP.py --cfg tnpAnaBATCH.py QCD_HT-700To1000  0 24 pippo 7.029e+03 1
#./submitBatchTnP.py --cfg tnpAnaBATCH.py QCD_HT-1000To1500 0 25 pippo 1.230e+03 1
#./submitBatchTnP.py --cfg tnpAnaBATCH.py QCD_HT-1500To2000 0 26 pippo 1.174e+02 1
#./submitBatchTnP.py --cfg tnpAnaBATCH.py QCD_HT-2000ToInf  0 27 pippo 2.520e+01 1

# Data
./submitBatchTnP.py --cfg tnpAnaBATCH.py singleEle2015C  0 10001 pippo 1  1 
./submitBatchTnP.py --cfg tnpAnaBATCH.py singleEle2015Dv0 0 10001 pippo 1  1
./submitBatchTnP.py --cfg tnpAnaBATCH.py singleEle2015Dv1 0 10001 pippo 1  1
