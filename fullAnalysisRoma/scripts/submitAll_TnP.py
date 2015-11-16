#
# usage: %prog [opts] --cfg cmssw.py dataset doPUreweighting(0/1) sampleIndex PUweightsFile x-section kFactor
#
# Backgrounds: sampleID>0 && sampleID<100
# Signal (DY): sampleID>100 && sampleID<10000
# Data:        sampleID>10000

# MC DY signal
#./submitBatchTnP.py --cfg tnpAnaBATCH.py DYLL 1 101 pileupWeights___processedAndSilver_2015D_nov2__75d5mb.root 5990  1 

# MC specific background
#./submitBatchTnP.py --cfg tnpAnaBATCH.py WJetsToLNu_HT-600To800   1 1 pileupWeights___processedAndSilver_2015D_nov2__75d5mb.root 12.26  1 
#./submitBatchTnP.py --cfg tnpAnaBATCH.py WJetsToLNu_HT-800To1200  1 2 pileupWeights___processedAndSilver_2015D_nov2__75d5mb.root 5.557  1 
#./submitBatchTnP.py --cfg tnpAnaBATCH.py WJetsToLNu_HT-1200To2500 1 3 pileupWeights___processedAndSilver_2015D_nov2__75d5mb.root 1.302  1 
#./submitBatchTnP.py --cfg tnpAnaBATCH.py WJetsToLNu_HT-2500ToInf  1 4 pileupWeights___processedAndSilver_2015D_nov2__75d5mb.root 0.02865 1 
#./submitBatchTnP.py --cfg tnpAnaBATCH.py WW2L2nu                  1 5 pileupWeights___processedAndSilver_2015D_nov2__75d5mb.root 10.48  1 
#./submitBatchTnP.py --cfg tnpAnaBATCH.py WZjets                   1 6 pileupWeights___processedAndSilver_2015D_nov2__75d5mb.root 5.257  1 
#./submitBatchTnP.py --cfg tnpAnaBATCH.py ZZ4l                     1 7 pileupWeights___processedAndSilver_2015D_nov2__75d5mb.root 1.256  1 
#./submitBatchTnP.py --cfg tnpAnaBATCH.py TTjets                   1 8 pileupWeights___processedAndSilver_2015D_nov2__75d5mb.root 509.1  1 

# other MC backgrounds: gg
#./submitBatchTnP.py --cfg tnpAnaBATCH.py GGJets_M-200To500    1 9  pileupWeights___processedAndSilver_2015D_nov2__75d5mb.root 2.047e+00 1
#./submitBatchTnP.py --cfg tnpAnaBATCH.py GGJets_M-500To1000   1 10 pileupWeights___processedAndSilver_2015D_nov2__75d5mb.root 1.510e-01 1
#./submitBatchTnP.py --cfg tnpAnaBATCH.py GGJets_M-1000To2000  1 11 pileupWeights___processedAndSilver_2015D_nov2__75d5mb.root 1.084e-02 1
#./submitBatchTnP.py --cfg tnpAnaBATCH.py GGJets_M-2000To4000  1 12 pileupWeights___processedAndSilver_2015D_nov2__75d5mb.root 3.690e-04 1
#./submitBatchTnP.py --cfg tnpAnaBATCH.py GGJets_M-4000To6000  1 13 pileupWeights___processedAndSilver_2015D_nov2__75d5mb.root 2.451e-06 1
#./submitBatchTnP.py --cfg tnpAnaBATCH.py GGJets_M-6000To8000  1 14 pileupWeights___processedAndSilver_2015D_nov2__75d5mb.root 1.753e-08 1
#./submitBatchTnP.py --cfg tnpAnaBATCH.py GGJets_M-8000To13000 1 15 pileupWeights___processedAndSilver_2015D_nov2__75d5mb.root 7.053e-11 1

# other MC backgrounds: g+jets
#./submitBatchTnP.py --cfg tnpAnaBATCH.py GJets_HT-100to200  1 16 pileupWeights___processedAndSilver_2015D_nov2__75d5mb.root 9.863e+03 1
#./submitBatchTnP.py --cfg tnpAnaBATCH.py GJets_HT-200to400  1 17 pileupWeights___processedAndSilver_2015D_nov2__75d5mb.root 2.298e+03 1  
#./submitBatchTnP.py --cfg tnpAnaBATCH.py GJets_HT-400to600  1 18 pileupWeights___processedAndSilver_2015D_nov2__75d5mb.root 2.816e+02 1
#./submitBatchTnP.py --cfg tnpAnaBATCH.py GJets_HT-600toInf  1 19 pileupWeights___processedAndSilver_2015D_nov2__75d5mb.root 9.465e+01 1

# other MC backgrounds: QCD 
#./submitBatchTnP.py --cfg tnpAnaBATCH.py QCD_HT-100To200   1 20 pileupWeights___processedAndSilver_2015D_nov2__75d5mb.root 2.803e+07 1
#./submitBatchTnP.py --cfg tnpAnaBATCH.py QCD_HT-200To300   1 21 pileupWeights___processedAndSilver_2015D_nov2__75d5mb.root 1.708e+06 1
#./submitBatchTnP.py --cfg tnpAnaBATCH.py QCD_HT-300To500   1 22 pileupWeights___processedAndSilver_2015D_nov2__75d5mb.root 3.509e+05 1
#./submitBatchTnP.py --cfg tnpAnaBATCH.py QCD_HT-500To700   1 23 pileupWeights___processedAndSilver_2015D_nov2__75d5mb.root 3.196e+04 1
#./submitBatchTnP.py --cfg tnpAnaBATCH.py QCD_HT-700To1000  1 24 pileupWeights___processedAndSilver_2015D_nov2__75d5mb.root 7.029e+03 1
#./submitBatchTnP.py --cfg tnpAnaBATCH.py QCD_HT-1000To1500 1 25 pileupWeights___processedAndSilver_2015D_nov2__75d5mb.root 1.230e+03 1
#./submitBatchTnP.py --cfg tnpAnaBATCH.py QCD_HT-1500To2000 1 26 pileupWeights___processedAndSilver_2015D_nov2__75d5mb.root 1.174e+02 1
#./submitBatchTnP.py --cfg tnpAnaBATCH.py QCD_HT-2000ToInf  1 27 pileupWeights___processedAndSilver_2015D_nov2__75d5mb.root 2.520e+01 1

# Data
#./submitBatchTnP.py --cfg tnpAnaBATCH.py singleEle2015C  0 10001 pippo 1  1 
./submitBatchTnP.py --cfg tnpAnaBATCH.py singleEle2015Dv0 0 10001 pippo 1  1
./submitBatchTnP.py --cfg tnpAnaBATCH.py singleEle2015Dv1 0 10001 pippo 1  1
./submitBatchTnP.py --cfg tnpAnaBATCH.py singleEle2015Dv2v3v4 0 10001 pippo 1  1
