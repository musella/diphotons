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
process.load("Configuration.StandardSequences.GeometryDB_cff")
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff")
from Configuration.AlCa.GlobalTag import GlobalTag
from Configuration.AlCa.autoCond import autoCond
process.GlobalTag = GlobalTag(process.GlobalTag, autoCond['run2_mc'].replace("::All",""))

# process.source = cms.Source ("PoolSource",
#                              fileNames = cms.untracked.vstring(options.inputFiles))

readFiles = cms.untracked.vstring()
process.source = cms.Source ("PoolSource",fileNames = readFiles)

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

# run all upstream corrections from flashgg
process.load("flashgg.Taggers.flashggUpdatedIdMVADiPhotons_cfi")
process.flashggSinglePhotonViews = cms.EDProducer("FlashggSinglePhotonViewProducer",
                                                  DiPhotonTag   = cms.InputTag('flashggUpdatedIdMVADiPhotons'),
                                                  maxCandidates = cms.int32(5)
                                              )

from flashgg.Taggers.photonViewDumper_cfi import photonViewDumper as photonDumper
process.photonDumper = photonDumper
## process.load("flashgg.Taggers.photonDumper_cfi") 
import flashgg.Taggers.dumperConfigTools as cfgTools

## process.photonDumper.src = "flashggRandomizedPhotons"
process.photonDumper.src = "flashggSinglePhotonViews"
process.photonDumper.dumpTrees = True
process.photonDumper.dumpWorkspace = False
process.photonDumper.quietRooFit = True

## list of variables to be dumped in trees/datasets. Same variables for all categories
variables=["pt := photon.pt",
           "energy := photon.energy",
           "eta := photon.eta",
           "phi := photon.phi",
           
           "scEta := photon.superCluster.eta",
           "scRawE := photon.superCluster.rawEnergy",
           
           "etaWidth := photon.superCluster.etaWidth",
           "phiWidth := photon.superCluster.phiWidth",
           "sipip := sqrt(photon.sipip)",
           "chgIsoWrtWorstVtx := photon.pfChgIsoWrtWorstVtx03",
           "phoIso03 := photon.pfPhoIso03",
           "chgIsoWrtVtx0 := photon.pfChgIso03WrtVtx0",
           "hcalTowerSumEtConeDR03 := photon.hcalTowerSumEtConeDR03",
           "trkSumPtHollowConeDR03 := photon.trkSumPtHollowConeDR03",
           "hadTowOverEm := photon.hadTowOverEm",
           
           ## "idMVA := phoIdMvaWrtChosenVtx",
           "genIso := ? photon.hasMatchedGenPhoton ? photon.userFloat('genIso') : -1", 
           ## "etrue := ? hasMatchedGenPhoton ? matchedGenPhoton.energy : 0",
           "sieie := photon.full5x5_sigmaIetaIeta",
           "r9 := photon.full5x5_r9",
           "esEffSigmaRR := photon.esEffSigmaRR",
           "s4 := photon.s4",
           "sieip := sqrt(photon.sieip)",
           
           "egChargedHadronIso := photon.egChargedHadronIso" ,
           "egNeutralHadronIso := photon.egNeutralHadronIso",
           "egPhotonIso := photon.egPhotonIso" ,
           
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
                     "   abs(photon.superCluster.eta)>=1.4442&&abs(photon.superCluster.eta)<=1.566 "
                     "|| abs(photon.superCluster.eta)>=2.5 "
                     "|| photon.pt<75",
                     -1 ## if nSubcat is -1 do not store anythings
                     )

# interestng categories 
cfgTools.addCategories(process.photonDumper,
                       ## categories definition
                       ## cuts are applied in cascade. Events getting to these categories have already failed the "Reject" selection
                       [("promptTree","photon.genMatchType == 1",0),
                        ("fakesTree",  "photon.genMatchType != 1",0),
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
    process.flashggUpdatedIdMVADiPhotons*
    process.flashggSinglePhotonViews*
    process.photonDumper
    )

## process.e = cms.EndPath(process.out)

from diphotons.MetaData.JobConfig import customize
customize(process)

