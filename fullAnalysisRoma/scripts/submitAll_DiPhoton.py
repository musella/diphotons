#
# usage: %prog [opts] --cfg cmssw.py dataset doPUreweighting(0/1) sampleIndex PUweightsFile x-section kFactor
#
# Backgrounds: sampleID>0 && sampleID<100
# Signals:     sampleID>100 && sampleID<10000
# Data:        sampleID>10000

# GG+jets 
./submitBatchDiPho.py --cfg diPhoAnaBATCH.py GGJets_M-200To500    0 1 pippo 2.302026      1 
./submitBatchDiPho.py --cfg diPhoAnaBATCH.py GGJets_M-500To1000   0 2 pippo 0.1583156     1  
./submitBatchDiPho.py --cfg diPhoAnaBATCH.py GGJets_M-1000To2000  0 3 pippo 0.01233671    1  
./submitBatchDiPho.py --cfg diPhoAnaBATCH.py GGJets_M-2000To4000  0 4 pippo 0.0004121658  1  
./submitBatchDiPho.py --cfg diPhoAnaBATCH.py GGJets_M-4000To6000  0 5 pippo 2.567550e-06  1    
./submitBatchDiPho.py --cfg diPhoAnaBATCH.py GGJets_M-6000To8000  0 6 pippo 2.720000e-08  1     
./submitBatchDiPho.py --cfg diPhoAnaBATCH.py GGJets_M-8000To13000 0 7 pippo 7.851862e-11  1 

# GJets (25ns for the moment)
./submitBatchDiPho.py --cfg diPhoAnaBATCH.py GJets_HT-100to200  0  8 pippo 9110.  1  
#./submitBatchDiPho.py --cfg diPhoAnaBATCH.py GJets_HT-200to400  0  9 pippo 22010. 1         #???  invalid anyway
./submitBatchDiPho.py --cfg diPhoAnaBATCH.py GJets_HT-400to600  0 10 pippo 273.   1  
./submitBatchDiPho.py --cfg diPhoAnaBATCH.py GJets_HT-600toInf  0 11 pippo 94.5   1  

# QCD
./submitBatchDiPho.py --cfg diPhoAnaBATCH.py QCD_HT-200To300   0 12 pippo 1740000. 1       
./submitBatchDiPho.py --cfg diPhoAnaBATCH.py QCD_HT-300To500   0 13 pippo 367000.  1       
./submitBatchDiPho.py --cfg diPhoAnaBATCH.py QCD_HT-500To700   0 14 pippo 29400.   1       
./submitBatchDiPho.py --cfg diPhoAnaBATCH.py QCD_HT-700To1000  0 15 pippo 6524.    1       
./submitBatchDiPho.py --cfg diPhoAnaBATCH.py QCD_HT-1000To1500 0 16 pippo 1064.    1       

# Graviton signal
./submitBatchDiPho.py --cfg diPhoAnaBATCH.py RSGravToGG_kMpl-001_M-500   0 101 pippo 0.33  1   
./submitBatchDiPho.py --cfg diPhoAnaBATCH.py RSGravToGG_kMpl-001_M-2000  0 102 pippo 0.000243  1  
./submitBatchDiPho.py --cfg diPhoAnaBATCH.py RSGravToGG_kMpl-001_M-3000  0 103 pippo 0.000015  1 
#
./submitBatchDiPho.py --cfg diPhoAnaBATCH.py RSGravToGG_kMpl-01_M-1000   0 104 pippo 1.23      1   
./submitBatchDiPho.py --cfg diPhoAnaBATCH.py RSGravToGG_kMpl-01_M-3500   0 105 pippo 0.000435  1 
./submitBatchDiPho.py --cfg diPhoAnaBATCH.py RSGravToGG_kMpl-01_M-4000   0 106 pippo 0.000135  1
./submitBatchDiPho.py --cfg diPhoAnaBATCH.py RSGravToGG_kMpl-01_M-5000   0 107 pippo 0.000014  1
./submitBatchDiPho.py --cfg diPhoAnaBATCH.py RSGravToGG_kMpl-01_M-7000   0 108 pippo 0.000000199 1
#
./submitBatchDiPho.py --cfg diPhoAnaBATCH.py RSGravToGG_kMpl-02_M-1000   0 109 pippo 4.92      1   
./submitBatchDiPho.py --cfg diPhoAnaBATCH.py RSGravToGG_kMpl-02_M-4000   0 110 pippo 0.000545  1 
./submitBatchDiPho.py --cfg diPhoAnaBATCH.py RSGravToGG_kMpl-02_M-5000   0 111 pippo 5.84e-05 1  

# Data
./submitBatchDiPho.py --cfg diPhoAnaBATCH.py DoubleEG   0 10001 pippo 1      1 
./submitBatchDiPho.py --cfg diPhoAnaBATCH.py singlePho  0 10002 pippo 1      1 
