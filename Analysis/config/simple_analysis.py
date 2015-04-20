#!/usr/bin/env cmsRun

import FWCore.ParameterSet.Config as cms
import FWCore.Utilities.FileUtils as FileUtils

process = cms.Process("Analysis")

process.load("diphotons.Analysis.highMassDiPhotons_cfi")

from flashgg.MicroAOD.flashggPreselectedDiPhotons_cfi import flashggPreselectedDiPhotons

from flashgg.Taggers.diphotonDumper_cfi import diphotonDumper 
import flashgg.Taggers.dumperConfigTools as cfgTools

diphotonDumper.dumpTrees = False
diphotonDumper.dumpWorkspace = False
diphotonDumper.quietRooFit = True
diphotonDumper.src=cms.InputTag("hcic4DiPhotons")
cfgTools.addCategories(diphotonDumper,
                       [## cuts are applied in cascade
                        ("EBHighR9","max(abs(leadingPhoton.superCluster.eta),abs(subLeadingPhoton.superCluster.eta))<1.4442"
                         "&& min(leadingPhoton.r9,subLeadingPhoton.r9)>0.94",0),
                        ("EBLowR9","max(abs(leadingPhoton.superCluster.eta),abs(subLeadingPhoton.superCluster.eta))<1.4442",0),
                        ("EEHighR9","min(leadingPhoton.r9,subLeadingPhoton.r9)>0.94",0),
                        ("EELowR9","1",0),
                        ],
                       variables=["mass", 
                                  "leadPt                   :=leadingPhoton.pt",
                                  "subleadPt                :=subLeadingPhoton.pt",
                                  "leadCShapeMVA            :=leadingPhoton.userFloat('cShapeMVA')",
                                  "subleadCShapeMVA         :=subLeadingPhoton.userFloat('cShapeMVA')",
                                  "minR9                    :=min(leadingPhoton.r9,subLeadingPhoton.r9)",
                                  "maxEta                   :=max(abs(leadingPhoton.superCluster.eta),abs(subLeadingPhoton.superCluster.eta))",
                                  
                                  "leadBlockChIso   := leadingPhotonView.pfChIso03WrtChosenVtx", 
                                  "leadBlockPhoIso  := leadingPhotonView.pfPhoIso03", 
                                  "leadRndConeChIso := leadingPhotonView.extraChgIsoWrtChoosenVtx('rnd03')",
                                  "leadRndConePhoIso:= leadingPhoton.extraPhoIso('rnd03')",
                                  

                                  "leadChIso   := leadingPhoton.egChargedHadronIso", 
                                  "leadChIso   := leadingPhoton.egChargedHadronIso", 
                                  "leadPhoIso  := leadingPhoton.egPhotonIso", 
                                  "leadNeutIso := leadingPhoton.egNeutralHadronIso",
                                  "leadHoE     := leadingPhoton.hadTowOverEm",
                                  "leadSigmaIeIe := (?leadingPhoton.r9>0.8||leadingPhoton.egChargedHadronIso<20||leadingPhoton.egChargedHadronIso/leadingPhoton.pt<0.3?leadingPhoton.full5x5_sigmaIetaIeta:leadingPhoton.sigmaIetaIeta)",
                                  "leadPixSeed  := leadingPhoton.hasPixelSeed",
                                  "leadPassEleVeto := leadingPhoton.passElectronVeto",


                                  "subleadBlockChIso   := subLeadingPhotonView.pfChIso03WrtChosenVtx", 
                                  "subleadBlockPhoIso  := subLeadingPhotonView.pfPhoIso03", 
                                  "subleadRndConeChIso := subleadingPhotonView.extraChgIsoWrtChoosenVtx('rnd03')",
                                  "subleadRndConePhoIso:= subleadingPhoton.extraPhoIso('rnd03')",
                                  
                                  "subleadChIso   := subLeadingPhoton.egChargedHadronIso", 
                                  "subleadPhoIso  := subLeadingPhoton.egPhotonIso", 
                                  "subleadNeutIso := subLeadingPhoton.egNeutralHadronIso",
                                  "subleadHoE     := subLeadingPhoton.hadTowOverEm",
                                  "subleadSigmaIeIe := (?subLeadingPhoton.r9>0.8||subLeadingPhoton.egChargedHadronIso<20||subLeadingPhoton.egChargedHadronIso/subLeadingPhoton.pt<0.3?subLeadingPhoton.full5x5_sigmaIetaIeta:subLeadingPhoton.sigmaIetaIeta)",
                                  "subleadPixSeed := subLeadingPhoton.hasPixelSeed",
                                  "subleadPassEleVeto := subLeadingPhoton.passElectronVeto",
                                  ],
                       histograms=["mass>>mass(1500,0,15000)",
                                   "leadPt>>leadPt(145,100,3000)",
                                   "subleadPt>>subleadPt(145,100,3000)",
                                   
                                   "leadBlockChIso>>leadBlockChIso(60,-10,50)",
                                   "leadBlockPhoIso>>leadBlockPhoIso(60,-10,50)",
                                   "leadChIso>>leadChIso(60,-10,50)",
                                   "leadPhoIso>>leadPhoIso(60,-10,50)",
                                   "leadNeutIso>>leadNeutIso(60,-10,50)",
                                   "leadHoE>>leadHoE(40,0,0.2)",
                                   "leadSigmaIeIe>>leadSigmaIeIe(50,0,5.e-2)",
                                   "leadPixSeed>>leadPixSeed(2,-0.5,1.5)",
                                   "subleadPassEleVeto>>subleadPassEleVeto(2,-0.5,1.5)",

                                   "subleadBlockChIso>>subleadBlockChIso(60,-10,50)",
                                   "subleadBlockPhoIso>>subleadBlockPhoIso(60,-10,50)",
                                   "subleadChIso>>subleadChIso(60,-10,50)",
                                   "subleadPhoIso>>subleadPhoIso(60,-10,50)",
                                   "subleadNeutIso>>subleadNeutIso(60,-10,50)",
                                   "subleadHoE>>subleadHoE(40,0,0.2)",
                                   "subleadSigmaIeIe>>subleadSigmaIeIe(50,0,5.e-2)",
                                   "subleadPixSeed>>subleadPixSeed(2,-0.5,1.5)",
                                   "subleadPassEleVeto>>subleadPassEleVeto(2,-0.5,1.5)",
                                   
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

process.triggerMva=diphotonDumper.clone(src=cms.InputTag("hmvaDiPhotons"))
process.mva=diphotonDumper.clone(src=cms.InputTag("hmvaDiPhotons"))

process.egid=diphotonDumper.clone(src=cms.InputTag("egLooseDiPhotons"))

process.kin=diphotonDumper.clone(src=cms.InputTag("kinDiPhotons"))
process.kin.dumpTrees = True

process.isoKinDiphotons = process.tmpKinDiPhotons.clone(src="kinDiPhotons",
                                                     cut="leadingPhoton.userFloat('genIso') < 10. && subLeadingPhoton.userFloat('genIso') < 10.")
process.isohCic4Diphotons = process.tmpKinDiPhotons.clone(src="hcic4DiPhotons",
                                                       cut="leadingPhoton.userFloat('genIso') < 10. && subLeadingPhoton.userFloat('genIso') < 10.")
process.isohmvaDiphotons = process.tmpKinDiPhotons.clone(src="hmvaDiPhotons",
                                                         cut="leadingPhoton.userFloat('genIso') < 10. && subLeadingPhoton.userFloat('genIso') < 10.")
process.isoKin=diphotonDumper.clone(src=cms.InputTag("isoKinDiphotons"))
process.isoId=diphotonDumper.clone(src=cms.InputTag("isohCic4Diphotons"))
process.isoMva=diphotonDumper.clone(src=cms.InputTag("isohmvaDiphotons"))

process.nonIsoKinDiphotons = process.tmpKinDiPhotons.clone(src="kinDiPhotons",
                                                        cut="leadingPhoton.userFloat('genIso') >= 10. || subLeadingPhoton.userFloat('gensIso') >= 10.")
process.nonIsohCic4Diphotons = process.tmpKinDiPhotons.clone(src="hcic4DiPhotons",
                                                          cut="leadingPhoton.userFloat('genIso') >= 10. || subLeadingPhoton.userFloat('gensIso') >= 10.")
process.nonIsohmvaDiphotons = process.tmpKinDiPhotons.clone(src="hmvaDiPhotons",
                                                          cut="leadingPhoton.userFloat('genIso') >= 10. || subLeadingPhoton.userFloat('gensIso') >= 10.")
process.nonIsoKin=diphotonDumper.clone(src=cms.InputTag("nonIsoKinDiphotons"))
process.nonIsoId=diphotonDumper.clone(src=cms.InputTag("nonIsohCic4Diphotons"))
process.nonIsoMva=diphotonDumper.clone(src=cms.InputTag("nonIsohmvaDiphotons"))


process.load('HLTrigger.HLTfilters.hltHighLevel_cfi')
process.hltHighLevel.HLTPaths = ["HLT_DoublePhoton85*","HLT_Photon250_NoHE*"]


process.watchDog = cms.EDAnalyzer("IdleWatchdog",
                             minIdleFraction=cms.untracked.double(0.5),
                             tolerance=cms.untracked.int32(10),
                             checkEvery=cms.untracked.int32(100),
                             )

process.p1 = cms.Path(
    ((process.tmpKinDiPhotons*process.kinDiPhotons)
    *
    (process.kin
      + (process.isoKinDiphotons+process.nonIsoKinDiphotons)*(process.isoKin+process.nonIsoKin)
      + process.egLooseDiPhotons*process.egid
      + process.hmvaDiPhotons
        * 
        (process.mva 
          + (process.isohmvaDiphotons+process.nonIsohmvaDiphotons)*(process.isoMva +process.nonIsoMva) 
          + (process.hltHighLevel*process.triggerMva) 
         )
      + process.hcic4DiPhotons
        *
        (process.id 
          + (process.isohCic4Diphotons+process.nonIsohCic4Diphotons)*(process.isoId +process.nonIsoId) 
          + (process.hltHighLevel*process.trigger)
        )
    ))
    * process.watchDog
)

from diphotons.MetaData.JobConfig import customize
customize.setDefault("maxEvents",100)
customize.setDefault("targetLumi",1.e+3)
customize(process)
