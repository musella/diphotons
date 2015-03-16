#!/usr/bin/env cmsRun

import FWCore.ParameterSet.Config as cms
import FWCore.Utilities.FileUtils as FileUtils

process = cms.Process("Analysis")

process.load("diphotons.Analysis.highMassDiPhotons_cfi")

from flashgg.MicroAODProducers.flashggPreselectedDiPhotons_cfi import flashggPreselectedDiPhotons
process.kinDiPhotons = flashggPreselectedDiPhotons.clone(cut=cms.string(process.egLooseDiPhotons.cut._value))

from flashgg.TagProducers.diphotonDumper_cfi import diphotonDumper 
import flashgg.TagAlgos.dumperConfigTools as cfgTools

## process.options = cms.untracked.PSet( wantSummary = cms.untracked.bool(True) )

process.noGenIsokinDiPhotons = process.kinDiPhotons.clone()
process.kinDiPhotons.cut     = "(%s)&& leadingPhoton.userFloat('genIso') < 10. && subLeadingPhoton.userFloat('genIso') < 10."% process.kinDiPhotons.cut._value    
process.egLooseDiPhotons.cut = "(%s)&& leadingPhoton.userFloat('genIso') < 10. && subLeadingPhoton.userFloat('genIso') < 10."% process.egLooseDiPhotons.cut._value
process.hcic4DiPhotons.cut   = "(%s)&& leadingPhoton.userFloat('genIso') < 10. && subLeadingPhoton.userFloat('genIso') < 10."% process.hcic4DiPhotons.cut._value   

diphotonDumper.dumpTrees = False
diphotonDumper.dumpWorkspace = False
diphotonDumper.quietRooFit = True
diphotonDumper.src=cms.InputTag("hcic4DiPhotons")
cfgTools.addCategories(diphotonDumper,
                       [## cuts are applied in cascade
                        ("EBHighR9","max(abs(leadingPhoton.superCluster.eta),abs(leadingPhoton.superCluster.eta))<1.4442"
                         "&& min(leadingPhoton.r9,subLeadingPhoton.r9)>0.94",0),
                        ("EBLowR9","max(abs(leadingPhoton.superCluster.eta),abs(leadingPhoton.superCluster.eta))<1.4442",0),
                        ("EEHighR9","min(leadingPhoton.r9,subLeadingPhoton.r9)>0.94",0),
                        ("EELowR9","1",0),
                        ],
                       variables=["mass", 
                                  "leadPt                   :=leadingPhoton.pt",
                                  "subleadPt                :=subLeadingPhoton.pt",
                                  "minR9                    :=min(leadingPhoton.r9,subLeadingPhoton.r9)",
                                  "maxEta                   :=max(abs(leadingPhoton.superCluster.eta),abs(leadingPhoton.superCluster.eta))",

                                  "leadChIso   := leadingPhoton.egChargedHadronIso", 
                                  "leadPhoIso  := leadingPhoton.egPhotonIso", 
                                  "leadNeutIso := leadingPhoton.egNeutralHadronIso",
                                  "leadHoE     := leadingPhoton.hadTowOverEm",
                                  "leadSigmaIeIe := (?leadingPhoton.r9>0.8||leadingPhoton.egChargedHadronIso<20||leadingPhoton.egChargedHadronIso/leadingPhoton.pt<0.3?leadingPhoton.full5x5_sigmaIetaIeta:leadingPhoton.sigmaIetaIeta)",
                                  "leadPixSeed := leadingPhoton.hasPixelSeed",


                                  "subleadChIso   := subLeadingPhoton.egChargedHadronIso", 
                                  "subleadPhoIso  := subLeadingPhoton.egPhotonIso", 
                                  "subleadNeutIso := subLeadingPhoton.egNeutralHadronIso",
                                  "subleadHoE     := subLeadingPhoton.hadTowOverEm",
                                  "subleadSigmaIeIe := (?subLeadingPhoton.r9>0.8||subLeadingPhoton.egChargedHadronIso<20||subLeadingPhoton.egChargedHadronIso/subLeadingPhoton.pt<0.3?subLeadingPhoton.full5x5_sigmaIetaIeta:subLeadingPhoton.sigmaIetaIeta)",
                                  "subleadPixSeed := subLeadingPhoton.hasPixelSeed",
                                  ],
                       histograms=["mass>>mass(1500,0,15000)",
                                   "leadPt>>leadPt(145,100,3000)",
                                   "subleadPt>>subleadPt(145,100,3000)",
                                   
                                   "leadChIso>>leadChIso(60,-10,50)",
                                   "leadPhoIso>>leadPhoIso(60,-10,50)",
                                   "leadNeutIso>>leadNeutIso(60,-10,50)",
                                   "leadHoE>>leadHoE(40,0,0.2)",
                                   "leadSigmaIeIe>>leadSigmaIeIe(50,0,5.e-2)",
                                   "leadPixSeed>>leadPixSeed(2,-0.5,1.5)",

                                   "subleadChIso>>subleadChIso(60,-10,50)",
                                   "subleadPhoIso>>subleadPhoIso(60,-10,50)",
                                   "subleadNeutIso>>subleadNeutIso(60,-10,50)",
                                   "subleadHoE>>subleadHoE(40,0,0.2)",
                                   "subleadSigmaIeIe>>subleadSigmaIeIe(50,0,5.e-2)",
                                   "subleadPixSeed>>subleadPixSeed(2,-0.5,1.5)",
                                   
                                   "subleadPt:leadPt>>ptSubVsLead(145,100,3000:145,100,3000)",
                                   "minR9>>minR9(110,0,1.1)",
                                   "maxEta>>maxEta(250,0,2.5)"
                                   ]
                       )





process.load("FWCore.MessageService.MessageLogger_cfi")

process.load("Configuration.StandardSequences.GeometryDB_cff")
process.load("Configuration.StandardSequences.MagneticField_cff")
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
process.GlobalTag.globaltag = 'POSTLS170_V5::All'
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )
process.MessageLogger.cerr.FwkReport.reportEvery = cms.untracked.int32( 1000 )



process.source = cms.Source("PoolSource",
                            fileNames=cms.untracked.vstring(
        )
)


process.TFileService = cms.Service("TFileService",
                                   fileName = cms.string("test.root")
)

process.trigger=diphotonDumper.clone()
process.id=diphotonDumper.clone()
process.egid=diphotonDumper.clone(src=cms.InputTag("egLooseDiPhotons"))
process.kin=diphotonDumper.clone(src=cms.InputTag("kinDiPhotons"))
process.noGenIso=diphotonDumper.clone(src=cms.InputTag("noGenIsokinDiPhotons"))
process.kin.dumpTrees = True

process.load('HLTrigger.HLTfilters.hltHighLevel_cfi')
process.hltHighLevel.HLTPaths = ["HLT_DoublePhoton85*","HLT_Photon250_NoHE*"]


process.p1 = cms.Path(
    (process.noGenIsokinDiPhotons+process.kinDiPhotons+process.hcic4DiPhotons)*(process.kin+process.noGenIso+process.id)
    )

process.p2 = cms.Path(
    (process.kinDiPhotons+process.egLooseDiPhotons)*(process.egid)
    )

process.p3 = cms.Path(process.hltHighLevel*
                      process.trigger
                      )




from diphotons.MetaData.JobConfig import customize
## customize.campaign = "isolation_studies"
customize.setDefault("maxEvents",100)
customize(process)

### process.out = cms.OutputModule("PoolOutputModule", fileName = cms.untracked.string('myOutputFile.root'),
###                                outputCommands = cms.untracked.vstring("drop *",
###                                                             "keep *_kinDiPhotons_*_*",
###                                                             "keep *_egLooseDiPhotons_*_*"
###                                                             )
###                                )
### process.e = cms.EndPath(process.out)
