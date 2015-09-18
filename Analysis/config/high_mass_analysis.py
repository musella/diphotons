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
# load job options
#
from diphotons.MetaData.JobConfig import customize
customize.setDefault("maxEvents",10000)
customize.setDefault("targetLumi",1.e+3)

import FWCore.ParameterSet.VarParsing as VarParsing
customize.options.register ('selection',
                            "diphoton", # default value
                            VarParsing.VarParsing.multiplicity.singleton, # singleton or list
                            VarParsing.VarParsing.varType.string,          # string, int, or float
                            "selection")
customize.options.register ('massCut',
                            200, # default value
                            VarParsing.VarParsing.multiplicity.singleton, # singleton or list
                            VarParsing.VarParsing.varType.float,          # string, int, or float
                            "massCut")
customize.options.register ('ptLead',
                            100, # default value
                            VarParsing.VarParsing.multiplicity.singleton, # singleton or list
                            VarParsing.VarParsing.varType.float,          # string, int, or float
                            "ptLead")
customize.options.register ('ptSublead',
                            100, # default value
                            VarParsing.VarParsing.multiplicity.singleton, # singleton or list
                            VarParsing.VarParsing.varType.float,          # string, int, or float
                            "ptSublead")
customize.options.register ('scaling',
                            False, # default value
                            VarParsing.VarParsing.multiplicity.singleton, # singleton or list
                            VarParsing.VarParsing.varType.bool,          # string, int, or float
                            "scaling")
customize.options.register ('trigger',
                            "", # default value
                            VarParsing.VarParsing.multiplicity.singleton, # singleton or list
                            VarParsing.VarParsing.varType.string,          # string, int, or float
                            "trigger")
customize.options.register ('idversion',
                            "", # default value
                            VarParsing.VarParsing.multiplicity.singleton, # singleton or list
                            VarParsing.VarParsing.varType.string,          # string, int, or float
                            "idversion")
customize.parse()

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

variables=["mass","pt","rapidity",
           "genMass := genP4.mass",
           "deltaEta                 := abs( leadingPhoton.eta - subLeadingPhoton.eta )",
           "cosDeltaPhi              := cos( leadingPhoton.phi - subLeadingPhoton.phi )",
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
           "leadSigmaIeIe := leadingPhoton.full5x5_sigmaIetaIeta",
           "leadPixSeed  := leadingPhoton.hasPixelSeed",
           "leadPassEleVeto := leadingPhoton.passElectronVeto",
           
           
           ## "subleadBlockChIso   := subLeadingView.pfChIso03WrtChosenVtx", 
           "subleadBlockPhoIso  := subLeadingPhoton.pfPhoIso03", 
           ## "subleadRndConePhoIso:= subLeadingPhoton.extraPhoIso('rnd03')",
           
           "subleadChIso   := subLeadingPhoton.egChargedHadronIso", 
           "subleadPhoIso  := subLeadingPhoton.egPhotonIso", 
           "subleadNeutIso := subLeadingPhoton.egNeutralHadronIso",
           "subleadHoE     := subLeadingPhoton.hadTowOverEm",
           "subleadSigmaIeIe := subLeadingPhoton.full5x5_sigmaIetaIeta",
           "subleadPixSeed := subLeadingPhoton.hasPixelSeed",
           "subleadPassEleVeto := subLeadingPhoton.passElectronVeto",
           ]

histograms=["mass>>mass(1500,0,15000)",
            "mass>>lowmass(560,60,200)",
            "pt>>pt(200,0,200)",
            "rapidity>>rapidity(200,-5,5)",
            "deltaEta>>deltaEta(200,0,5)",
            "cosDeltaPhi>>cosDeltaPhi(200,0,1)",
            "global.rho>>rho(20,0,50)",
            "global.nvtx>>nvtx(20,0,50)",
            
            "leadPt>>phoPt(150,0,3000)",
            "subleadPt>>phoPt(150,0,3000)",
            
            "leadPt>>leadPt(150,0,3000)",
            "subleadPt>>subleadPt(150,0,3000)",
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
            "leadPassEleVeto>>leadPassEleVeto(2,-0.5,1.5)",
            
            "subleadBlockChIso>>subleadBlockChIso(120,-10,50)",
            "subleadBlockPhoIso>>subleadBlockPhoIso(120,-10,50)",
            "subleadChIso>>subleadChIso(120,-10,50)",
            "subleadPhoIso>>subleadPhoIso(120,-10,50)",
            "subleadNeutIso>>subleadNeutIso(120,-10,50)",
            "subleadHoE>>subleadHoE(40,0,0.2)",
            "subleadSigmaIeIe>>subleadSigmaIeIe(50,0,5.e-2)",
            "subleadPixSeed>>subleadPixSeed(2,-0.5,1.5)",
            "subleadPassEleVeto>>subleadPassEleVeto(2,-0.5,1.5)",
            
            
            "leadChIso>>phoChIso(120,-10,50)",
            "subleadChIso>>phoChIso(120,-10,50)",
            "leadPhoIso>>phoPhoIso(120,-10,50)",
            "subleadPhoIso>>phoPhoIso(120,-10,50)",
            "leadSigmaIeIe>>phoSigmaIeIe(50,0,5.e-2)",
            "subleadSigmaIeIe>>phoSigmaIeIe(50,0,5.e-2)",
            "leadHoE>>phoHoE(40,0,0.2)",                                   
            "subleadHoE>>phoHoE(40,0,0.2)",                                   
            "leadPassEleVeto>>phoPassEleVeto(2,-0.5,1.5)",
            "subleadPassEleVeto>>phoPassEleVeto(2,-0.5,1.5)",
            
            "subleadPt:leadPt>>ptSubVsLead(145,100,3000:145,100,3000)",
            "minR9>>minR9(110,0,1.1)",
            "maxEta>>maxEta(250,0,2.5)"
            ]


variablesSinglePho=[
    "phoPt                   :=pt",
    "phoEta                  :=eta",
    "phoR9                   :=r9",
    "phoScEta                :=superCluster.eta",
    "phoPhi                  :=phi",
    
    "phoBlockChIso   := pfChgIso03WrtVtx0", 
    
    "phoBlockPhoIso  := pfPhoIso03", 
    ## "phoRndConePhoIso:= extraPhoIso('rnd03')",
    
    "phoPhoIsoEA :=  map( abs(superCluster.eta) :: 0.,0.9,1.5,2.0,2.2,3. :: 0.21,0.2,0.14,0.22,0.31 )",
    
    "phoMatchType            :=genMatchType",
    "phoGenIso               :=userFloat('genIso')",
    
    "phoChIso   := egChargedHadronIso", 
    "phoPhoIso  := egPhotonIso", 
    "phoNeutIso := egNeutralHadronIso",
    "phoHoE     := hadTowOverEm",
    "phoSigmaIeIe := full5x5_sigmaIetaIeta",
    "phoSigmaIpIp := sqrt(sipip)",
    "eMax","e2nd","eTop","eBottom","eLeft","eRight",
    "phoPixSeed  := hasPixelSeed",
    "phoPassEleVeto := passElectronVeto",
    ]


histogramsSinglePho = [
    "phoPt>>phoPt(145,100,3000)",
    "phoEta>>phoEta(55,-2.75,2.75)",
    "phoPhi>>phoPhi(65,-3.25,3.25)",
    
    "phoBlockChIso>>phoBlockChIso(120,-10,50)",
    "phoBlockPhoIso>>phoBlockPhoIso(120,-10,50)",
    "phoChIso>>phoChIso(120,-10,50)",
    "phoPhoIso>>phoPhoIso(120,-10,50)",
    "phoNeutIso>>phoNeutIso(120,-10,50)",
    "phoHoE>>phoHoE(40,0,0.2)",
    "phoSigmaIeIe>>phoSigmaIeIe(50,0,5.e-2)",
    "phoPixSeed>>phoPixSeed(2,-0.5,1.5)",
    "phoScEta:phoPhi>>phoEtaVsPhi(65,-3.25,3.25:55,-2.75,2.75)"
    ]

if not "EXOSpring15_v3" in customize.datasetName() or "EXOSpring15_v3v8" in customize.datasetName():
    variables.extend( [
            "leadRndConeChIso := leadingView.extraChIsoWrtChoosenVtx('rnd03')",
            "leadRndConeChIso0 := leadingView.extraChIsoWrtChoosenVtx('rnd03_0')",
            "leadRndConeChIso1 := leadingView.extraChIsoWrtChoosenVtx('rnd03_1')",
            "leadRndConeChIso2 := leadingView.extraChIsoWrtChoosenVtx('rnd03_2')",
            "leadRndConeChIso3 := leadingView.extraChIsoWrtChoosenVtx('rnd03_3')",
            "leadRndConeChIso4 := leadingView.extraChIsoWrtChoosenVtx('rnd03_4')",
            "leadRndConeChIso5 := leadingView.extraChIsoWrtChoosenVtx('rnd03_5')",
            "leadRndConeChIso6 := leadingView.extraChIsoWrtChoosenVtx('rnd03_6')",
            "leadRndConeChIso7 := leadingView.extraChIsoWrtChoosenVtx('rnd03_7')",
            "leadRndConeChIso8 := leadingView.extraChIsoWrtChoosenVtx('rnd03_8')",
            
            "subleadRndConeChIso := subLeadingView.extraChIsoWrtChoosenVtx('rnd03')",
            "subleadRndConeChIso0 := subLeadingView.extraChIsoWrtChoosenVtx('rnd03_0')",
            "subleadRndConeChIso1 := subLeadingView.extraChIsoWrtChoosenVtx('rnd03_1')",
            "subleadRndConeChIso2 := subLeadingView.extraChIsoWrtChoosenVtx('rnd03_2')",
            "subleadRndConeChIso3 := subLeadingView.extraChIsoWrtChoosenVtx('rnd03_3')",
            "subleadRndConeChIso4 := subLeadingView.extraChIsoWrtChoosenVtx('rnd03_4')",
            "subleadRndConeChIso5 := subLeadingView.extraChIsoWrtChoosenVtx('rnd03_5')",
            "subleadRndConeChIso6 := subLeadingView.extraChIsoWrtChoosenVtx('rnd03_6')",
            "subleadRndConeChIso7 := subLeadingView.extraChIsoWrtChoosenVtx('rnd03_7')",
            "subleadRndConeChIso8 := subLeadingView.extraChIsoWrtChoosenVtx('rnd03_8')",
            ])
    
    histograms.extend([
            "leadRndConeChIso0>>leadRndConeChIso(120,-10,50)",
            "leadRndConeChIso1>>leadRndConeChIso(120,-10,50)",
            "leadRndConeChIso2>>leadRndConeChIso(120,-10,50)",
            "leadRndConeChIso3>>leadRndConeChIso(120,-10,50)",
            "leadRndConeChIso4>>leadRndConeChIso(120,-10,50)",
            "leadRndConeChIso5>>leadRndConeChIso(120,-10,50)",
            "leadRndConeChIso6>>leadRndConeChIso(120,-10,50)",
            "leadRndConeChIso7>>leadRndConeChIso(120,-10,50)",
            "leadRndConeChIso8>>leadRndConeChIso(120,-10,50)",
            
            "subleadRndConeChIso0>>subleadRndConeChIso(120,-10,50)",
            "subleadRndConeChIso1>>subleadRndConeChIso(120,-10,50)",
            "subleadRndConeChIso2>>subleadRndConeChIso(120,-10,50)",
            "subleadRndConeChIso3>>subleadRndConeChIso(120,-10,50)",
            "subleadRndConeChIso4>>subleadRndConeChIso(120,-10,50)",
            "subleadRndConeChIso5>>subleadRndConeChIso(120,-10,50)",
            "subleadRndConeChIso6>>subleadRndConeChIso(120,-10,50)",
            "subleadRndConeChIso7>>subleadRndConeChIso(120,-10,50)",
            "subleadRndConeChIso8>>subleadRndConeChIso(120,-10,50)",
            
            "leadRndConeChIso0>>phoRndConeChIso(120,-10,50)",
            "leadRndConeChIso1>>phoRndConeChIso(120,-10,50)",
            "leadRndConeChIso2>>phoRndConeChIso(120,-10,50)",
            "leadRndConeChIso3>>phoRndConeChIso(120,-10,50)",
            "leadRndConeChIso4>>phoRndConeChIso(120,-10,50)",
            "leadRndConeChIso5>>phoRndConeChIso(120,-10,50)",
            "leadRndConeChIso6>>phoRndConeChIso(120,-10,50)",
            "leadRndConeChIso7>>phoRndConeChIso(120,-10,50)",
            "leadRndConeChIso8>>phoRndConeChIso(120,-10,50)",
            
            "subleadRndConeChIso0>>phoRndConeChIso(120,-10,50)",
            "subleadRndConeChIso1>>phoRndConeChIso(120,-10,50)",
            "subleadRndConeChIso2>>phoRndConeChIso(120,-10,50)",
            "subleadRndConeChIso3>>phoRndConeChIso(120,-10,50)",
            "subleadRndConeChIso4>>phoRndConeChIso(120,-10,50)",
            "subleadRndConeChIso5>>phoRndConeChIso(120,-10,50)",
            "subleadRndConeChIso6>>phoRndConeChIso(120,-10,50)",
            "subleadRndConeChIso7>>phoRndConeChIso(120,-10,50)",
            "subleadRndConeChIso8>>phoRndConeChIso(120,-10,50)",
            ])
    
    variablesSinglePho.extend([    
            "phoRndConeChIso := extraChgIsoWrtVtx0('rnd03')",
            "phoRndConeChIso0 := extraChgIsoWrtVtx0('rnd03_0')",
            "phoRndConeChIso1 := extraChgIsoWrtVtx0('rnd03_1')",
            "phoRndConeChIso2 := extraChgIsoWrtVtx0('rnd03_2')",
            "phoRndConeChIso3 := extraChgIsoWrtVtx0('rnd03_3')",
            "phoRndConeChIso4 := extraChgIsoWrtVtx0('rnd03_4')",
            "phoRndConeChIso5 := extraChgIsoWrtVtx0('rnd03_5')",
            "phoRndConeChIso6 := extraChgIsoWrtVtx0('rnd03_6')",
            "phoRndConeChIso7 := extraChgIsoWrtVtx0('rnd03_7')",
            "phoRndConeChIso8 := extraChgIsoWrtVtx0('rnd03_8')",
            ])
    
    histogramsSinglePho.extend([
            "phoRndConeChIso0>>phoRndConeChIso(120,-10,50)",
            "phoRndConeChIso1>>phoRndConeChIso(120,-10,50)",
            "phoRndConeChIso2>>phoRndConeChIso(120,-10,50)",
            "phoRndConeChIso3>>phoRndConeChIso(120,-10,50)",
            "phoRndConeChIso4>>phoRndConeChIso(120,-10,50)",
            "phoRndConeChIso5>>phoRndConeChIso(120,-10,50)",
            "phoRndConeChIso6>>phoRndConeChIso(120,-10,50)",
            "phoRndConeChIso7>>phoRndConeChIso(120,-10,50)",
            "phoRndConeChIso8>>phoRndConeChIso(120,-10,50)"
            ])
else:
    variables.extend( [
            "leadRndConeChIso  := 999",
            "leadRndConeChIso0 := 999",
            "leadRndConeChIso1 := 999",
            "leadRndConeChIso2 := 999",
            "leadRndConeChIso3 := 999",
            "leadRndConeChIso4 := 999",
            "leadRndConeChIso5 := 999",
            "leadRndConeChIso6 := 999",
            "leadRndConeChIso7 := 999",
            "leadRndConeChIso8 := 999",
            
            "subleadRndConeChIso  := 999",
            "subleadRndConeChIso0 := 999",
            "subleadRndConeChIso1 := 999",
            "subleadRndConeChIso2 := 999",
            "subleadRndConeChIso3 := 999",
            "subleadRndConeChIso4 := 999",
            "subleadRndConeChIso5 := 999",
            "subleadRndConeChIso6 := 999",
            "subleadRndConeChIso7 := 999",
            "subleadRndConeChIso8 := 999",
            ])

    variablesSinglePho.extend([    
            "phoRndConeChIso  := 999",
            "phoRndConeChIso0 := 999",
            "phoRndConeChIso1 := 999",
            "phoRndConeChIso2 := 999",
            "phoRndConeChIso3 := 999",
            "phoRndConeChIso4 := 999",
            "phoRndConeChIso5 := 999",
            "phoRndConeChIso6 := 999",
            "phoRndConeChIso7 := 999",
            "phoRndConeChIso8 := 999",
            ])

cfgTools.addCategories(diphotonDumper,
                       [## cuts are applied in cascade
                        ## ("all","1"),
                        ("EBHighR9","max(abs(leadingPhoton.superCluster.eta),abs(subLeadingPhoton.superCluster.eta))<1.4442"
                         "&& min(leadingPhoton.r9,subLeadingPhoton.r9)>0.94",0),
                        ("EBLowR9","max(abs(leadingPhoton.superCluster.eta),abs(subLeadingPhoton.superCluster.eta))<1.4442",0),
                        ("EEHighR9","min(leadingPhoton.r9,subLeadingPhoton.r9)>0.94",0),
                        ("EELowR9","1",0),
                        ],
                       variables=variables,
                       histograms=histograms
                       )

# single photon dumpoer
photonDumper.processId = "test"
photonDumper.dumpTrees = False
photonDumper.dumpWorkspace = False
photonDumper.quietRooFit = True
photonDumper.maxCandPerEvent=2
photonDumper.nameTemplate = "$PROCESS_$SQRTS_$LABEL_$SUBCAT"
cfgTools.addCategories(photonDumper,
                       [## cuts are applied in cascade
                        ("EBAnomalous","abs(superCluster.eta)<1.4442 && sqrt(sipip)<0.01 && full5x5_sigmaIetaIeta>0.0105",0),
                        ("EBHighR9","abs(superCluster.eta)<1.4442 && r9>0.94",0),
                        ("EBLowR9", "abs(superCluster.eta)<1.4442",0),
                        ("EEHighR9","r9>0.94",0),
                        ("EELowR9","1",0),
                        ],
                       variables=variablesSinglePho,
                       histograms=histogramsSinglePho,
                       )

#
# input and output
#
process.source = cms.Source("PoolSource",
                            fileNames=cms.untracked.vstring(
        "/store/group/phys_higgs/cmshgg/musella/flashgg/EXOSpring15_v5/Spring15BetaV2-2-gfceadad/SinglePhoton/EXOSpring15_v5-Spring15BetaV2-2-gfceadad-v0-Run2015B-PromptReco-v1/150813_095357/0000/diphotonsMicroAOD_99.root"
        # "/store/group/phys_higgs/cmshgg/musella/flashgg/ExoPhys14ANv1/diphotonsPhys14AnV1/GGJets_M-1000To2000_Pt-50_13TeV-sherpa/ExoPhys14ANv1-diphotonsPhys14AnV1-v0-Phys14DR-PU20bx25_PHYS14_25_V1-v1/150330_192709/0000/diphotonsMicroAOD_1.root")
        )
)
process.TFileService = cms.Service("TFileService",
                                   fileName = cms.string("test.root")
)


#
# analysis configuration
#


dataTriggers=[]
mcTriggers=[]
doSinglePho=False
doDoublePho=True
invertEleVeto=False
dumpBits=["HLT_DoublePhoton60","HLT_DoublePhoton85","HLT_Photon250_NoHE","HLT_Photon165_HE"]
askTriggerOnMc=False

if customize.selection == "diphoton":
    mcTriggers=["HLT_DoublePhoton85*","HLT_Photon250_NoHE*","HLT_Photon165_HE*"] ## "HLT_DoublePhoton60*",
    dataTriggers=mcTriggers
elif customize.selection == "photon":
    dataTriggers=["HLT_Photon165*"]
    mcTriggers=dataTriggers
    doSinglePho=True
    doDoublePho=False
    askTriggerOnMc=True
elif customize.selection == "electron":
    ## dataTriggers=["HLT_Ele23_WPLoose*"]
    ## mcTriggers=[]
    dataTriggers=["HLT_Ele27_eta2p1_WPLoose_Gsf_v*"]
    mcTriggers=["HLT_Ele27_eta2p1_WP75_Gsf_v*"]
    invertEleVeto=True
elif customize.selection == "dielectron":
    dataTriggers=["*"]
    mcTriggers=dataTriggers
    if customize.options.trigger == "":
        sys.exit("please complete dielectron selection",-1)
        invertEleVeto=True
        
if customize.options.trigger != "":
    dataTriggers = customize.options.trigger.split(",")
    mcTriggers = dataTriggers
    
from HLTrigger.HLTfilters.hltHighLevel_cfi import hltHighLevel
dumpBits=set(dumpBits)
if len(dumpBits) > 0:
    if doDoublePho:
        diphotonDumper.globalVariables.addTriggerBits = cms.PSet(
            tag=cms.InputTag("TriggerResults","","HLT"),bits=cms.vstring(dumpBits)
            )
    if doSinglePho:
        photonDumper.globalVariables.addTriggerBits = cms.PSet(
            tag=cms.InputTag("TriggerResults","","HLT"),bits=cms.vstring(dumpBits)
            )
            

minimalDumper = diphotonDumper.clone()
cfgTools.dumpOnly(minimalDumper,
                  ["mass","pt","genMass",
                   "leadPt","leadEta","leadScEta","leadPhi",
                   "subleadPt","subleadEta","subleadScEta","subleadPhi",
                   "leadBlockPhoIso","subleadBlockPhoIso",
                   "leadBlockChIso","subleadBlockChIso",
                   "leadRndConePhoIso","leadRndConeChIso",
                   "subleadRndConePhoIso","subleadRndConeChIso",
                   "leadRndConeChIso0","leadRndConeChIso1","leadRndConeChIso2","leadRndConeChIso3",
                   "leadRndConeChIso4","leadRndConeChIso5","leadRndConeChIso6","leadRndConeChIso7","leadRndConeChIso8",
                   "subleadRndConeChIso0","subleadRndConeChIso1","subleadRndConeChIso2","subleadRndConeChIso3",
                   "subleadRndConeChIso4","subleadRndConeChIso5","subleadRndConeChIso6","subleadRndConeChIso7","subleadRndConeChIso8",
                   "leadMatchType","leadGenIso",
                   "subleadMatchType","subleadGenIso",
                   "leadPhoIsoEA","subleadPhoIsoEA",
                   "leadPhoIso","subleadPhoIso",
                   "leadChIso","subleadChIso",
                   "leadSigmaIeIe","subleadSigmaIeIe",
                   "leadHoE","subleadHoE",
                   ])


from diphotons.Analysis.DiPhotonAnalysis import DiPhotonAnalysis
analysis = DiPhotonAnalysis(diphotonDumper,
                            massCut=customize.massCut,ptLead=customize.ptLead,ptSublead=customize.ptSublead,scaling=customize.scaling, ## kinematic cuts
                            computeMVA=False,
                            genIsoDefinition=("userFloat('genIso')",10.),
                            dataTriggers=dataTriggers,
                            mcTriggers=mcTriggers,
                            askTriggerOnMc=askTriggerOnMc, ## if mcTriggers is not empty will still compute efficiencies
                            singlePhoDumperTemplate=photonDumper
                            )

# drop samples overlap
if "GJet-HT" in customize.datasetName():
    analysis.keepPFOnly = True
elif "QCD" in customize.datasetName():
    analysis.keepFFOnly = True

## kinematic selection
analysis.addKinematicSelection(process,dumpTrees=True,splitByIso=True
                               )


## analysis selections
# CiC
if customize.idversion != "":
    if customize.idversion == "V2":
        from diphotons.Analysis.highMassCiCDiPhotons_cfi import highMassCiCDiPhotonsV2   as highMassCiCDiPhotons
        from diphotons.Analysis.highMassCiCDiPhotons_cfi import highMassCiCDiPhotonsSBV2 as highMassCiCDiPhotonsSB
    else:
        print "Unknown ID version %s " % customize.idversion
        sys.exit(-1)
else:
    from diphotons.Analysis.highMassCiCDiPhotons_cfi import highMassCiCDiPhotons, highMassCiCDiPhotonsSB

if invertEleVeto:
    ## highMassCiCDiPhotons.variables[-1] = "-(passElectronVeto-1)"
    ## highMassCiCDiPhotonsSB.variables[-1] = "-(passElectronVeto-1)"
    highMassCiCDiPhotons.variables[-1] = "hasPixelSeed"
    highMassCiCDiPhotonsSB.variables[-1] = "hasPixelSeed"
if doDoublePho:
    analysis.addAnalysisSelection(process,"cic",highMassCiCDiPhotons,dumpTrees=True,dumpWorkspace=False,dumpHistos=True,splitByIso=True,
                                  dumperTemplate=minimalDumper,
                                  nMinusOne=[(0,"NoChIso",        True, False,True), ## removeIndex(es), label, dumpTree, dumpWorkspace, dumpHistos
                                             (1,"NoPhoIso",       False, False,True),
                                             (2,"NoNeuIso",       False,False,True),
                                             (3,"NoHoverE",       False,False,True),
                                             (4,"NoSigmaIetaIeta",False,False,True),
                                             (5,"NoEleVeto",      False,False,True),
                                             ]
                                  )
    
    analysis.addAnalysisSelection(process,"cicSB",highMassCiCDiPhotonsSB,dumpTrees=True,dumpWorkspace=False,dumpHistos=True,splitByIso=True,
                                  dumperTemplate=minimalDumper,
                                  nMinusOne=[## Sidebands
                                             ## removeIndex, (ignoreIndex(es),ingnoreNtimes), dumpTree, dumpWorkspace, dumpHistos, splitByIso
                                             ((0),(4,1),"NoChIsoSingleSB",  True, False,True,False),
                                             ((0),(4,2),"NoChIsoDoubleSB",  True, False,True,False)
                                             ]
                                  )

# single photon selection
if customize.idversion != "":
    if customize.idversion == "V2":
        from diphotons.Analysis.highMassCiCPhotons_cfi import highMassCiCPhotonsV2 as highMassCiCPhotons
        from diphotons.Analysis.highMassCiCPhotons_cfi import highMassCiCPhotonsSBV2 as highMassCiCPhotonsSB
    else:
        print "Unknown ID version %s " % customize.idversion
        sys.exit(-1)
else:
    from diphotons.Analysis.highMassCiCPhotons_cfi import highMassCiCPhotons, highMassCiCPhotonsSB
if invertEleVeto:
    ## highMassCiCPhotons.variables[-1] = "-(passElectronVeto-1)"
    ## highMassCiCPhotonsSB.variables[-1] = "-(passElectronVeto-1)"
    highMassCiCPhotons.variables[-1] = "hasPixelSeed"
    highMassCiCPhotonsSB.variables[-1] = "hasPixelSeed"
if doSinglePho:
    analysis.addPhotonAnalysisSelection(process,"cic",highMassCiCPhotons,dumpTrees=False,dumpWorkspace=False,dumpHistos=True,splitByIso=True,
                                        dumperTemplate=photonDumper,
                                        nMinusOne=[(0,"NoChIso",        True, False,True), ## removeIndex(es), label, dumpTree, dumpWorkspace, dumpHistos
                                                   (1,"NoPhoIso",       False, False,True),
                                                   (2,"NoNeuIso",       False,False,True),
                                                   (3,"NoHoverE",       False,False,True),
                                                   (4,"NoSigmaIetaIeta",False,False,True),
                                                   (5,"NoEleVeto",      False,False,True),
                                                   ]
                              )
    
    analysis.addPhotonAnalysisSelection(process,"cicSB",highMassCiCPhotonsSB,dumpTrees=False,dumpWorkspace=False,dumpHistos=True,splitByIso=True,
                                        dumperTemplate=photonDumper,
                                        nMinusOne=[## Sidebands
                                                   ## removeIndex, (ignoreIndex(es),ingnoreNtimes), dumpTree, dumpWorkspace, dumpHistos, splitByIso
                                                   ((0),(4,1),"NoChIsoSB",  True, False,True,False),
                                                   ]
                              )


if not customize.lastAttempt:
    # make sure process doesn't get stuck due to slow I/O
    process.watchDog = cms.EDAnalyzer("IdleWatchdog",
                                      minIdleFraction=cms.untracked.double(0.1),
                                      tolerance=cms.untracked.int32(10),
                                      checkEvery=cms.untracked.int32(100),
                                      )
    process.watch = cms.Path(
        process.watchDog
        )


# this will call customize(process), configure the analysis paths and make the process unscheduled
analysis.customize(process,customize)

## print process.dumpPython()
