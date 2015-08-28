#!/bin/bash

## target=hlt.py
target=hlt3.py

## hltConfigFromDB --cff --configName /dev/CMSSW_7_4_0/GRun --nopaths --services -PrescaleService,-EvFDaqDirector,-FastMonitoringService > ../python/setup_cff.py
hltGetConfiguration /users/musella/dipho0T/V10 --full --offline --mc --unprescale --process myHLT \
    --globaltag 74X_HLT_mcRun2_asymptotic_fromSpring15DR_v0  \
    --l1-emulator 'stage1,gt' --l1Xml L1Menu_Collisions2015_25nsStage1_v3_L1T_Scales_20141121_Imp0_0x1031.xml \
    --input /store/mc/RunIISpring15Digi74/QCD_Pt-80to120_EMEnriched_TuneCUETP8M1_13TeV_pythia8/GEN-SIM-RAW/AVE_20_BX_25ns_tsg_MCRUN2_74_V7-v1/00000/004960C8-99EF-E411-B39F-00266CF9B3FC.root \
    > $target

set -x

sed 's%process = cms.Process( "myHLT" )%process = cms.Process( "myHLT" )\nprocess.load("diphotons/Trigger/setup_cff")%; s%_customInfo.*maxEvents.*=.*%_customInfo["maxEvents"]=-1%' -i $target

### sed 's%from HLTrigger.Configuration.customizeHLTforALL import customizeHLTforAll%from diphotons.Trigger.JobConfig import customize
### try:
###     customize.setDefault("targetLumi",1e-3) ## to get rates in Hz @ L=1e+33cm-2s-1
### except:
###     pass
### customize.parse()
### 
### if hasattr(customize.options,"processType"):
###     _customInfo["realData"] = customize.options.processType == "data"
### 
### import sys
### sys.argv = sys.argv[:2]
### 
### from HLTrigger.Configuration.customizeHLTforALL import customizeHLTforAll%' -i $target

cat >> $target <<EOF

# print process.source.fileNames

customize(process)
# print process.source.fileNames
 
EOF
