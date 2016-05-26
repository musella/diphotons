#!/usr/bin/env cmsRun

import FWCore.ParameterSet.Config as cms
import FWCore.Utilities.FileUtils as FileUtils
from FWCore.ParameterSet.VarParsing import VarParsing

### ## CMD LINE OPTIONS ##
### options = VarParsing('analysis')
### 
### # maxEvents is the max number of events processed of each file, not globally
### options.maxEvents = -1
### options.inputFiles = "file:diphotonsMicroAOD.root"
### options.outputFile = "quickDump.root"
### options.parseArguments()

process = cms.Process("Analysis")

process.load("FWCore.MessageService.MessageLogger_cfi")

# process.source = cms.Source ("PoolSource",
#                              fileNames = cms.untracked.vstring(options.inputFiles))

readFiles = cms.untracked.vstring()
process.source = cms.Source ("PoolSource",fileNames = readFiles)
readFiles.extend( [
           '/store/user/spigazzi/flashgg/diphotonsIsoStudies0T/1_2_0-31-g16f7065/GJet_Pt-15to6000_TuneCUETP8M1_Flat_13TeV_pythia8/diphotonsIsoStudies0T-1_2_0-31-g16f7065-v0-RunIIFall15MiniAODv2-PU25nsData2015v1_magnetOff_76X_mcRun2_asymptotic_v12-v1/160119_091710/0000/myMicroAODOutputFile_1.root',
           '/store/user/spigazzi/flashgg/diphotonsIsoStudies0T/1_2_0-31-g16f7065/GJet_Pt-15to6000_TuneCUETP8M1_Flat_13TeV_pythia8/diphotonsIsoStudies0T-1_2_0-31-g16f7065-v0-RunIIFall15MiniAODv2-PU25nsData2015v1_magnetOff_76X_mcRun2_asymptotic_v12-v1/160119_091710/0000/myMicroAODOutputFile_105.root',
           '/store/user/spigazzi/flashgg/diphotonsIsoStudies0T/1_2_0-31-g16f7065/GJet_Pt-15to6000_TuneCUETP8M1_Flat_13TeV_pythia8/diphotonsIsoStudies0T-1_2_0-31-g16f7065-v0-RunIIFall15MiniAODv2-PU25nsData2015v1_magnetOff_76X_mcRun2_asymptotic_v12-v1/160119_091710/0000/myMicroAODOutputFile_106.root',
           '/store/user/spigazzi/flashgg/diphotonsIsoStudies0T/1_2_0-31-g16f7065/GJet_Pt-15to6000_TuneCUETP8M1_Flat_13TeV_pythia8/diphotonsIsoStudies0T-1_2_0-31-g16f7065-v0-RunIIFall15MiniAODv2-PU25nsData2015v1_magnetOff_76X_mcRun2_asymptotic_v12-v1/160119_091710/0000/myMicroAODOutputFile_107.root',
           '/store/user/spigazzi/flashgg/diphotonsIsoStudies0T/1_2_0-31-g16f7065/GJet_Pt-15to6000_TuneCUETP8M1_Flat_13TeV_pythia8/diphotonsIsoStudies0T-1_2_0-31-g16f7065-v0-RunIIFall15MiniAODv2-PU25nsData2015v1_magnetOff_76X_mcRun2_asymptotic_v12-v1/160119_091710/0000/myMicroAODOutputFile_109.root',
           '/store/user/spigazzi/flashgg/diphotonsIsoStudies0T/1_2_0-31-g16f7065/GJet_Pt-15to6000_TuneCUETP8M1_Flat_13TeV_pythia8/diphotonsIsoStudies0T-1_2_0-31-g16f7065-v0-RunIIFall15MiniAODv2-PU25nsData2015v1_magnetOff_76X_mcRun2_asymptotic_v12-v1/160119_091710/0000/myMicroAODOutputFile_11.root',
           '/store/user/spigazzi/flashgg/diphotonsIsoStudies0T/1_2_0-31-g16f7065/GJet_Pt-15to6000_TuneCUETP8M1_Flat_13TeV_pythia8/diphotonsIsoStudies0T-1_2_0-31-g16f7065-v0-RunIIFall15MiniAODv2-PU25nsData2015v1_magnetOff_76X_mcRun2_asymptotic_v12-v1/160119_091710/0000/myMicroAODOutputFile_14.root',
           '/store/user/spigazzi/flashgg/diphotonsIsoStudies0T/1_2_0-31-g16f7065/GJet_Pt-15to6000_TuneCUETP8M1_Flat_13TeV_pythia8/diphotonsIsoStudies0T-1_2_0-31-g16f7065-v0-RunIIFall15MiniAODv2-PU25nsData2015v1_magnetOff_76X_mcRun2_asymptotic_v12-v1/160119_091710/0000/myMicroAODOutputFile_26.root',
           '/store/user/spigazzi/flashgg/diphotonsIsoStudies0T/1_2_0-31-g16f7065/GJet_Pt-15to6000_TuneCUETP8M1_Flat_13TeV_pythia8/diphotonsIsoStudies0T-1_2_0-31-g16f7065-v0-RunIIFall15MiniAODv2-PU25nsData2015v1_magnetOff_76X_mcRun2_asymptotic_v12-v1/160119_091710/0000/myMicroAODOutputFile_30.root',
           '/store/user/spigazzi/flashgg/diphotonsIsoStudies0T/1_2_0-31-g16f7065/GJet_Pt-15to6000_TuneCUETP8M1_Flat_13TeV_pythia8/diphotonsIsoStudies0T-1_2_0-31-g16f7065-v0-RunIIFall15MiniAODv2-PU25nsData2015v1_magnetOff_76X_mcRun2_asymptotic_v12-v1/160119_091710/0000/myMicroAODOutputFile_31.root',
           '/store/user/spigazzi/flashgg/diphotonsIsoStudies0T/1_2_0-31-g16f7065/GJet_Pt-15to6000_TuneCUETP8M1_Flat_13TeV_pythia8/diphotonsIsoStudies0T-1_2_0-31-g16f7065-v0-RunIIFall15MiniAODv2-PU25nsData2015v1_magnetOff_76X_mcRun2_asymptotic_v12-v1/160119_091710/0000/myMicroAODOutputFile_44.root',
           '/store/user/spigazzi/flashgg/diphotonsIsoStudies0T/1_2_0-31-g16f7065/GJet_Pt-15to6000_TuneCUETP8M1_Flat_13TeV_pythia8/diphotonsIsoStudies0T-1_2_0-31-g16f7065-v0-RunIIFall15MiniAODv2-PU25nsData2015v1_magnetOff_76X_mcRun2_asymptotic_v12-v1/160119_091710/0000/myMicroAODOutputFile_45.root',
           '/store/user/spigazzi/flashgg/diphotonsIsoStudies0T/1_2_0-31-g16f7065/GJet_Pt-15to6000_TuneCUETP8M1_Flat_13TeV_pythia8/diphotonsIsoStudies0T-1_2_0-31-g16f7065-v0-RunIIFall15MiniAODv2-PU25nsData2015v1_magnetOff_76X_mcRun2_asymptotic_v12-v1/160119_091710/0000/myMicroAODOutputFile_47.root',
           '/store/user/spigazzi/flashgg/diphotonsIsoStudies0T/1_2_0-31-g16f7065/GJet_Pt-15to6000_TuneCUETP8M1_Flat_13TeV_pythia8/diphotonsIsoStudies0T-1_2_0-31-g16f7065-v0-RunIIFall15MiniAODv2-PU25nsData2015v1_magnetOff_76X_mcRun2_asymptotic_v12-v1/160119_091710/0000/myMicroAODOutputFile_53.root',
           '/store/user/spigazzi/flashgg/diphotonsIsoStudies0T/1_2_0-31-g16f7065/GJet_Pt-15to6000_TuneCUETP8M1_Flat_13TeV_pythia8/diphotonsIsoStudies0T-1_2_0-31-g16f7065-v0-RunIIFall15MiniAODv2-PU25nsData2015v1_magnetOff_76X_mcRun2_asymptotic_v12-v1/160119_091710/0000/myMicroAODOutputFile_54.root',
           '/store/user/spigazzi/flashgg/diphotonsIsoStudies0T/1_2_0-31-g16f7065/GJet_Pt-15to6000_TuneCUETP8M1_Flat_13TeV_pythia8/diphotonsIsoStudies0T-1_2_0-31-g16f7065-v0-RunIIFall15MiniAODv2-PU25nsData2015v1_magnetOff_76X_mcRun2_asymptotic_v12-v1/160119_091710/0000/myMicroAODOutputFile_58.root',
           '/store/user/spigazzi/flashgg/diphotonsIsoStudies0T/1_2_0-31-g16f7065/GJet_Pt-15to6000_TuneCUETP8M1_Flat_13TeV_pythia8/diphotonsIsoStudies0T-1_2_0-31-g16f7065-v0-RunIIFall15MiniAODv2-PU25nsData2015v1_magnetOff_76X_mcRun2_asymptotic_v12-v1/160119_091710/0000/myMicroAODOutputFile_63.root',
           '/store/user/spigazzi/flashgg/diphotonsIsoStudies0T/1_2_0-31-g16f7065/GJet_Pt-15to6000_TuneCUETP8M1_Flat_13TeV_pythia8/diphotonsIsoStudies0T-1_2_0-31-g16f7065-v0-RunIIFall15MiniAODv2-PU25nsData2015v1_magnetOff_76X_mcRun2_asymptotic_v12-v1/160119_091710/0000/myMicroAODOutputFile_64.root',
           '/store/user/spigazzi/flashgg/diphotonsIsoStudies0T/1_2_0-31-g16f7065/GJet_Pt-15to6000_TuneCUETP8M1_Flat_13TeV_pythia8/diphotonsIsoStudies0T-1_2_0-31-g16f7065-v0-RunIIFall15MiniAODv2-PU25nsData2015v1_magnetOff_76X_mcRun2_asymptotic_v12-v1/160119_091710/0000/myMicroAODOutputFile_66.root',
           '/store/user/spigazzi/flashgg/diphotonsIsoStudies0T/1_2_0-31-g16f7065/GJet_Pt-15to6000_TuneCUETP8M1_Flat_13TeV_pythia8/diphotonsIsoStudies0T-1_2_0-31-g16f7065-v0-RunIIFall15MiniAODv2-PU25nsData2015v1_magnetOff_76X_mcRun2_asymptotic_v12-v1/160119_091710/0000/myMicroAODOutputFile_7.root',
           '/store/user/spigazzi/flashgg/diphotonsIsoStudies0T/1_2_0-31-g16f7065/GJet_Pt-15to6000_TuneCUETP8M1_Flat_13TeV_pythia8/diphotonsIsoStudies0T-1_2_0-31-g16f7065-v0-RunIIFall15MiniAODv2-PU25nsData2015v1_magnetOff_76X_mcRun2_asymptotic_v12-v1/160119_091710/0000/myMicroAODOutputFile_71.root',
           '/store/user/spigazzi/flashgg/diphotonsIsoStudies0T/1_2_0-31-g16f7065/GJet_Pt-15to6000_TuneCUETP8M1_Flat_13TeV_pythia8/diphotonsIsoStudies0T-1_2_0-31-g16f7065-v0-RunIIFall15MiniAODv2-PU25nsData2015v1_magnetOff_76X_mcRun2_asymptotic_v12-v1/160119_091710/0000/myMicroAODOutputFile_8.root' ] )



process.TFileService = cms.Service("TFileService",
                                   fileName = cms.string("output.root"))

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )
process.MessageLogger.cerr.FwkReport.reportEvery = cms.untracked.int32( 10000 )

## from flashgg.MicroAOD.flashggPreselectedDiPhotons_cfi import flashggPreselectedDiPhotons
## process.kinPreselDiPhotons = flashggPreselectedDiPhotons.clone(
## cut=cms.string(
##         "1"
##         ### "mass > 100"
##         ### " && leadingPhoton.pt > 30 && subLeadingPhoton.pt > 30"
##         ### " && abs(leadingPhoton.superCluster.eta)<2.5 && abs(subLeadingPhoton.superCluster.eta)<2.5 "
##         ### " && ( abs(leadingPhoton.superCluster.eta)<1.4442 || abs(leadingPhoton.superCluster.eta)>1.566)"
##         ### " && ( abs(subLeadingPhoton.superCluster.eta)<1.4442 || abs(subLeadingPhoton.superCluster.eta)>1.566)"
##         ### ## " && leadingPhoton.genMatchType != subLeadingPhoton.genMatchType " ## selects only prompt-fake pairs
##         )
##                                                              )


## process.flashggSinglePhotonViews = cms.EDProducer("FlashggSinglePhotonViewProducer",
##                                           DiPhotonTag=cms.untracked.InputTag('kinPreselDiPhotons'),
##                                                   
##                                           )

## process.load("flashgg.Taggers.photonViewDumper_cfi") ##  import diphotonDumper 
process.load("flashgg.Taggers.photonDumper_cfi") 
import flashgg.Taggers.dumperConfigTools as cfgTools

process.photonDumper.src = "flashggRandomizedPhotons"
## process.photonDumper.src = "flashggSinglePhotonViews"
process.photonDumper.dumpTrees = True
process.photonDumper.dumpWorkspace = False
process.photonDumper.quietRooFit = True

## list of variables to be dumped in trees/datasets. Same variables for all categories
variables=["pt := pt",
           "energy := energy",
           "eta := eta",
           "phi := phi",
           
           "scEta:=superCluster.eta",
           "scRawE := superCluster.rawEnergy",
           
           "etaWidth := superCluster.etaWidth",
           "phiWidth := superCluster.phiWidth",
           "sipip := sqrt(sipip)",
           "chgIsoWrtWorstVtx := pfChgIsoWrtWorstVtx03",
           "phoIso03 := pfPhoIso03",
           "chgIsoWrtVtx0 := pfChgIso03WrtVtx0",
           "hcalTowerSumEtConeDR03 := hcalTowerSumEtConeDR03",
           "trkSumPtHollowConeDR03 := trkSumPtHollowConeDR03",
           "hadTowOverEm := hadTowOverEm",
           
           ## "idMVA := phoIdMvaWrtChosenVtx",
           "genIso := ? hasMatchedGenPhoton ? userFloat('genIso') : -1", 
           ## "etrue := ? hasMatchedGenPhoton ? matchedGenPhoton.energy : 0",
           "sieie := sigmaIetaIeta",
           "r9 := r9",
           "esEffSigmaRR := esEffSigmaRR",
           "s4 := s4",
           "sieip := sqrt(sieip)",
           
           "egChargedHadronIso := egChargedHadronIso" ,
           "egNeutralHadronIso := egNeutralHadronIso",
           "egPhotonIso := egPhotonIso" ,
           
           ## "rndConeDeltaPhi := userFloat('rnd03_rndcone_deltaphi')",
           ## "fprRndConeDeltaPhi := userFloat('fprRnd03_rndcone_deltaphi')",
           
           ## "rndConeChIso := extraChgIsoWrtVtx0('rnd03')",
           ## "stdChIso := extraChgIsoWrtVtx0('std03')",
           
           ## "fprRndConeChIso := extraChgIsoWrtVtx0('fprRnd03')",
           ## "fprChIso := extraChgIsoWrtVtx0('fpr03')",
                      
           ## "rndConePhoIso := extraPhoIso('rnd03')",
           ## "stdPhoIso := extraPhoIso('std03')",
           
           ## "fprRndConePhoIso := extraPhoIso('fprRnd03')",
           ## "fprPhoIso := extraPhoIso('fpr03')",
           
           ## "fprRndNoMapConePhoIso := extraPhoIso('fprRndNoMap03')",
           ## "fprNoMapPhoIso := extraPhoIso('fprNoMap03')",
           ]

## list of histograms to be plotted
histograms=["r9>>r9(110,0,1.1)",
            "scEta>>scEta(100,-2.5,2.5)",
            "rndConePhoIso>>rndConePhoIso(60,-10,50)",
            "rndConeChIso>>rndConeChIso(60,-10,50)",
            "stdPhoIso>>stdPhoIso(60,-10,50)",
            "stdChIso>>stdChIso(60,-10,50)",
            ]

## define categories and associated objects to dump
cfgTools.addCategory(process.photonDumper,
                     "Reject",
                     "   abs(superCluster.eta)>=1.4442&&abs(superCluster.eta)<=1.566 "
                     "|| abs(superCluster.eta)>=2.5 "
                     "|| pt<75",
                     -1 ## if nSubcat is -1 do not store anythings
                     )

# interestng categories 
cfgTools.addCategories(process.photonDumper,
                       ## categories definition
                       ## cuts are applied in cascade. Events getting to these categories have already failed the "Reject" selection
                       [("promptTree","genMatchType == 1",0),
                        ("fakesTree",  "genMatchType != 1",0),
                        ],
                       ## variables to be dumped in trees/datasets. Same variables for all categories
                       ## if different variables wanted for different categories, can add categorie one by one with cfgTools.addCategory
                       variables=variables,
                       ## histograms to be plotted. 
                       ## the variables need to be defined first
                       histograms=histograms,
                       ## compute MVA on the fly. More then one MVA can be tested at once
                       mvas = None
                       )

process.idleWatchdog=cms.EDAnalyzer("IdleWatchdog",
                                    checkEvery = cms.untracked.int32(100),
                                    minIdleFraction = cms.untracked.double(0.5),
                                    tolerance = cms.untracked.int32(5)
                                    )

process.p1 = cms.Path(
## process.idleWatchdog*process.kinPreselDiPhotons*process.flashggSinglePhotonViews*process.photonViewDumper
    #process.idleWatchdog*
    process.photonDumper
    )

## process.e = cms.EndPath(process.out)

from diphotons.MetaData.JobConfig import customize
customize(process)

