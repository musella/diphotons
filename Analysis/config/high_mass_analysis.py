#!/usr/bin/env cmsRun

import FWCore.ParameterSet.Config as cms
import FWCore.Utilities.FileUtils as FileUtils

process = cms.Process("Analysis")

#
# general stuff
#
process.load("FWCore.MessageService.MessageLogger_cfi")

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )
process.MessageLogger.cerr.FwkReport.reportEvery = cms.untracked.int32( 1000 )


#
# define minitrees and histograms
#
from flashgg.Taggers.diphotonDumper_cfi import diphotonDumper 
from flashgg.Taggers.photonDumper_cfi import photonDumper 
import flashgg.Taggers.dumperConfigTools as cfgTools


diphotonDumper.processId = "test"
diphotonDumper.dumpTrees = False
diphotonDumper.dumpWorkspace = False
diphotonDumper.quietRooFit = True
diphotonDumper.maxCandPerEvent=1
diphotonDumper.nameTemplate = "$PROCESS_$SQRTS_$LABEL_$SUBCAT"
cfgTools.addCategories(diphotonDumper,
                       [## cuts are applied in cascade
                        ## ("all","1"),
                        ("EBHighR9","max(abs(leadingPhoton.superCluster.eta),abs(subLeadingPhoton.superCluster.eta))<1.4442"
                         "&& min(leadingPhoton.r9,subLeadingPhoton.r9)>0.94",0),
                        ("EBLowR9","max(abs(leadingPhoton.superCluster.eta),abs(subLeadingPhoton.superCluster.eta))<1.4442",0),
                        ("EEHighR9","min(leadingPhoton.r9,subLeadingPhoton.r9)>0.94",0),
                        ("EELowR9","1",0),
                        ],
                       variables=["mass","pt", 
                                  "leadPt                   :=leadingPhoton.pt",
                                  "subleadPt                :=subLeadingPhoton.pt",
                                  "leadEta                  :=leadingPhoton.eta",
                                  "subleadEta               :=subLeadingPhoton.eta",
                                  "leadR9                   :=leadingPhoton.r9",
                                  "subleadR9                :=subLeadingPhoton.r9",
                                  "leadScEta                :=leadingPhoton.superCluster.eta",
                                  "subleadScEta             :=subLeadingPhoton.superCluster.eta",
                                  "leadPhi                  :=leadingPhoton.phi",
                                  "subleadPhi               :=subLeadingPhoton.phi",
                                  "leadCShapeMVA            :=leadingPhoton.userFloat('cShapeMVA')",
                                  "subleadCShapeMVA         :=subLeadingPhoton.userFloat('cShapeMVA')",
                                  "minR9                    :=min(leadingPhoton.r9,subLeadingPhoton.r9)",
                                  "maxEta                   :=max(abs(leadingPhoton.superCluster.eta),abs(subLeadingPhoton.superCluster.eta))",
                                  
                                  "leadBlockChIso   := leadingView.pfChIso03WrtChosenVtx", 
                                  "leadBlockPhoIso  := leadingPhoton.pfPhoIso03", 
                                  "leadRndConeChIso := leadingView.extraChIsoWrtChoosenVtx('rnd03')",
                                  "leadRndConePhoIso:= leadingPhoton.extraPhoIso('rnd03')",
                                  
                                  "leadPhoIsoEA :=  map( abs(leadingPhoton.superCluster.eta) :: 0.,0.9,1.5,2.0,2.2,3. :: 0.21,0.2,0.14,0.22,0.31 )",
                                  "subleadPhoIsoEA :=  map( abs(subLeadingPhoton.superCluster.eta) :: 0.,0.9,1.5,2.0,2.2,3. :: 0.21,0.2,0.14,0.22,0.31 )",
                                  
                                  "leadMatchType            :=leadingPhoton.genMatchType",
                                  "leadGenIso               :=leadingPhoton.userFloat('genIso')",
                                  "subleadMatchType         :=subLeadingPhoton.genMatchType",
                                  "subleadGenIso            :=subLeadingPhoton.userFloat('genIso')",
                                  
                                  "leadChIso   := leadingPhoton.egChargedHadronIso", 
                                  "leadPhoIso  := leadingPhoton.egPhotonIso", 
                                  "leadNeutIso := leadingPhoton.egNeutralHadronIso",
                                  "leadHoE     := leadingPhoton.hadTowOverEm",
                                  "leadSigmaIeIe := (?leadingPhoton.r9>0.8||leadingPhoton.egChargedHadronIso<20||leadingPhoton.egChargedHadronIso/leadingPhoton.pt<0.3?leadingPhoton.full5x5_sigmaIetaIeta:leadingPhoton.sigmaIetaIeta)",
                                  "leadPixSeed  := leadingPhoton.hasPixelSeed",
                                  "leadPassEleVeto := leadingPhoton.passElectronVeto",
                                  
                                  
                                  "subleadBlockChIso   := subLeadingView.pfChIso03WrtChosenVtx", 
                                  "subleadBlockPhoIso  := subLeadingPhoton.pfPhoIso03", 
                                  "subleadRndConeChIso := subLeadingView.extraChIsoWrtChoosenVtx('rnd03')",
                                  "subleadRndConePhoIso:= subLeadingPhoton.extraPhoIso('rnd03')",
                                  
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
                                   "leadEta>>leadEta(55,-2.75,2.75)",
                                   "subleadEta>>subleadEta(55,-2.75,2.75)",

                                   "leadBlockChIso>>leadBlockChIso(120,-10,50)",
                                   "leadBlockPhoIso>>leadBlockPhoIso(120,-10,50)",
                                   "leadChIso>>leadChIso(120,-10,50)",
                                   "leadPhoIso>>leadPhoIso(120,-10,50)",
                                   "leadNeutIso>>leadNeutIso(120,-10,50)",
                                   "leadHoE>>leadHoE(40,0,0.2)",
                                   "leadSigmaIeIe>>leadSigmaIeIe(50,0,5.e-2)",
                                   "leadPixSeed>>leadPixSeed(2,-0.5,1.5)",
                                   "subleadPassEleVeto>>subleadPassEleVeto(2,-0.5,1.5)",
                                   
                                   "subleadBlockChIso>>subleadBlockChIso(120,-10,50)",
                                   "subleadBlockPhoIso>>subleadBlockPhoIso(120,-10,50)",
                                   "subleadChIso>>subleadChIso(120,-10,50)",
                                   "subleadPhoIso>>subleadPhoIso(120,-10,50)",
                                   "subleadNeutIso>>subleadNeutIso(120,-10,50)",
                                   "subleadHoE>>subleadHoE(40,0,0.2)",
                                   "subleadSigmaIeIe>>subleadSigmaIeIe(50,0,5.e-2)",
                                   "subleadPixSeed>>subleadPixSeed(2,-0.5,1.5)",
                                   "subleadPassEleVeto>>subleadPassEleVeto(2,-0.5,1.5)",
                                   
                                   "subleadPt:leadPt>>ptSubVsLead(145,100,3000:145,100,3000)",
                                   "minR9>>minR9(110,0,1.1)",
                                   "maxEta>>maxEta(250,0,2.5)"
                                   ]
                       )

minimalDumper = diphotonDumper.clone()
cfgTools.dumpOnly(minimalDumper,
                  ["mass","pt",
                   "leadPt","leadEta","leadScEta","leadPhi",
                   "subleadPt","subleadEta","subleadScEta","subleadPhi",
                   "leadBlockPhoIso","subleadBlockPhoIso",
                   "leadBlockChIso","subleadBlockChIso",
                   "leadRndConePhoIso","leadRndConeChIso",
                   "subleadRndConePhoIso","subleadRndConeChIso",
                   "leadMatchType","leadGenIso",
                   "subleadMatchType","subleadGenIso",
                   "leadPhoIsoEA","subleadPhoIsoEA",
                   "leadPhoIso","subleadPhoIso",
                   "leadChIso","subleadChIso",
                   "leadSigmaIeIe","subleadSigmaIeIe",
                   "leadHoE","subleadHoE",
                   ])


# single photon dumpoer
photonDumper.processId = "test"
photonDumper.dumpTrees = False
photonDumper.dumpWorkspace = False
photonDumper.quietRooFit = True
photonDumper.maxCandPerEvent=2
photonDumper.nameTemplate = "$PROCESS_$SQRTS_$LABEL_$SUBCAT"
cfgTools.addCategories(photonDumper,
                       [## cuts are applied in cascade
                        ("EBHighR9","abs(superCluster.eta)<1.4442 && r9>0.94",0),
                        ("EBLowR9", "abs(superCluster.eta)<1.4442",0),
                        ("EEHighR9","r9>0.94",0),
                        ("EELowR9","1",0),
                        ],
                       variables=[
                                  "phoPt                   :=pt",
                                  "phoEta                  :=eta",
                                  "phoR9                   :=r9",
                                  "phoScEta                :=superCluster.eta",
                                  "phoPhi                  :=phi",
                                  
                                  "phoBlockChIso   := pfChgIso03WrtVtx0", 
                                  "phoBlockPhoIso  := pfPhoIso03", 
                                  "phoRndConeChIso := extraChgIsoWrtVtx0('rnd03')",
                                  "phoRndConePhoIso:= extraPhoIso('rnd03')",
                                  
                                  "phoPhoIsoEA :=  map( abs(superCluster.eta) :: 0.,0.9,1.5,2.0,2.2,3. :: 0.21,0.2,0.14,0.22,0.31 )",
                                  
                                  "phoMatchType            :=genMatchType",
                                  "phoGenIso               :=userFloat('genIso')",
                                  
                                  "phoChIso   := egChargedHadronIso", 
                                  "phoPhoIso  := egPhotonIso", 
                                  "phoNeutIso := egNeutralHadronIso",
                                  "phoHoE     := hadTowOverEm",
                                  "phoSigmaIeIe := (?r9>0.8||egChargedHadronIso<20||egChargedHadronIso/pt<0.3?full5x5_sigmaIetaIeta:sigmaIetaIeta)",
                                  "phoPixSeed  := hasPixelSeed",
                                  "phoPassEleVeto := passElectronVeto",
                                  ],
                       histograms=[
                                   "phoPt>>phoPt(145,100,3000)",
                                   "phoEta>>phoEta(55,-2.75,2.75)",

                                   "phoBlockChIso>>phoBlockChIso(120,-10,50)",
                                   "phoBlockPhoIso>>phoBlockPhoIso(120,-10,50)",
                                   "phoChIso>>phoChIso(120,-10,50)",
                                   "phoPhoIso>>phoPhoIso(120,-10,50)",
                                   "phoNeutIso>>phoNeutIso(120,-10,50)",
                                   "phoHoE>>phoHoE(40,0,0.2)",
                                   "phoSigmaIeIe>>phoSigmaIeIe(50,0,5.e-2)",
                                   "phoPixSeed>>phoPixSeed(2,-0.5,1.5)",
                                                                      ]
                       )

minimalPhotonDumper = photonDumper.clone()
cfgTools.dumpOnly(minimalPhotonDumper,
                  ["leadPt","leadEta","leadPhi",
                   "leadBlockPhoIso","leadBlockChIso",
                   "leadRndConePhoIso","leadRndConeChIso","leadRndConeChIso",
                   "leadMatchType","leadGenIso",
                   "leadPhoIsoEA","leadPhoIso",
                   "leadChIso","leadScEta","leadSigmaIeIe"
                   ])

#
# input and output
#
process.source = cms.Source("PoolSource",
                            fileNames=cms.untracked.vstring("/store/group/phys_higgs/cmshgg/musella/flashgg/ExoPhys14ANv1/diphotonsPhys14AnV1/GGJets_M-1000To2000_Pt-50_13TeV-sherpa/ExoPhys14ANv1-diphotonsPhys14AnV1-v0-Phys14DR-PU20bx25_PHYS14_25_V1-v1/150330_192709/0000/diphotonsMicroAOD_1.root")
)
process.TFileService = cms.Service("TFileService",
                                   fileName = cms.string("test.root")
)


#
# analysis configuration
#
from diphotons.Analysis.DiPhotonAnalysis import DiPhotonAnalysis
analysis = DiPhotonAnalysis(diphotonDumper,
                            massCut=200.,ptLead=100.,ptSublead=100.,scaling=False, ## kinematic cuts
                            computeMVA=False,
                            genIsoDefinition=("userFloat('genIso')",10.),
                            dataTriggers=["HLT_DoublePhoton85*","HLT_Photon250_NoHE*"],
                            mcTriggers=[],## ["HLT_DoublePhoton85*","HLT_Photon250_NoHE*"],
                            askTriggerOnMc=False, ## if mcTriggers is not empty will still compute efficiencies
                            singlePhoDumperTemplate=photonDumper
                            )

## kinematic selection
analysis.addKinematicSelection(process,dumpTrees=True,splitByIso=True
                               )


## analysis selections
# CiC
from diphotons.Analysis.highMassCiCDiPhotons_cfi import highMassCiCDiPhotons
analysis.addAnalysisSelection(process,"cic",highMassCiCDiPhotons,dumpTrees=True,dumpWorkspace=False,dumpHistos=True,splitByIso=True,
                              dumperTemplate=minimalDumper,
                              nMinusOne=[(0,"NoChIso",        True, False,True), ## removeIndex(es), label, dumpTree, dumpWorkspace, dumpHistos
                                         (1,"NoPhoIso",       False, False,True),
                                         (2,"NoNeuIso",       False,False,True),
                                         (3,"NoHoverE",       False,False,True),
                                         (4,"NoSigmaIetaIeta",False,False,True),
                                         (5,"NoEleVeto",      False,False,True),
                                         ## Sidebands
                                         ## removeIndex, (ignoreIndex(es),ingnoreNtimes), dumpTree, dumpWorkspace, dumpHistos, splitByIso
                                         ((0,1),(4,1),"NoChIsoSingleSB",  True, False,True,False),
                                         ((0,1),(4,2),"NoChIsoDoubleSB",  True, False,True,False),
                                         (1,(4,1),"NoPhoIsoSingleSB",  False, False,True,False),
                                         (1,(4,2),"NoPhoIsoDoubleSB",  False, False,True,False),
                                         ]
                              )

# single photon selection
from diphotons.Analysis.highMassCiCPhotons_cfi import highMassCiCPhotons
analysis.addPhotonAnalysisSelection(process,"cic",highMassCiCPhotons,dumpTrees=False,dumpWorkspace=False,dumpHistos=True,splitByIso=True,
                                    dumperTemplate=photonDumper,
                                    nMinusOne=[(0,"NoChIso",        True, False,True), ## removeIndex(es), label, dumpTree, dumpWorkspace, dumpHistos
                                               (1,"NoPhoIso",       False, False,True),
                                               (2,"NoNeuIso",       False,False,True),
                                               (3,"NoHoverE",       False,False,True),
                                               (4,"NoSigmaIetaIeta",False,False,True),
                                               (5,"NoEleVeto",      False,False,True),
                                               ## Sidebands
                                               ## removeIndex, (ignoreIndex(es),ingnoreNtimes), dumpTree, dumpWorkspace, dumpHistos, splitByIso
                                               ((0,1),(4,1),"NoChIsoSB",  True, False,True,False),
                                               (1,(4,1),"NoPhoIsoSB",  False, False,True,False)
                                               ]
                              )


# make sure process doesn't get stuck due to slow I/O
process.watchDog = cms.EDAnalyzer("IdleWatchdog",
                             minIdleFraction=cms.untracked.double(0.5),
                             tolerance=cms.untracked.int32(10),
                             checkEvery=cms.untracked.int32(100),
                             )
process.watch = cms.Path(
    process.watchDog
)

# final customization
from diphotons.MetaData.JobConfig import customize
customize.setDefault("maxEvents",10000)
customize.setDefault("targetLumi",1.e+3)

# this will call customize(process), configure the analysis paths and make the process unscheduled
analysis.customize(process,customize)

## print process.dumpPython()
