#!/usr/bin/env cmsRun

import FWCore.ParameterSet.Config as cms
import FWCore.Utilities.FileUtils as FileUtils

process = cms.Process("Analysis")

#
# general stuff
#
process.load("FWCore.MessageService.MessageLogger_cfi")

process.load("Configuration.StandardSequences.GeometryDB_cff")
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff")
from Configuration.AlCa.GlobalTag import GlobalTag

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )
process.MessageLogger.cerr.FwkReport.reportEvery = cms.untracked.int32( 1000 )

process.RandomNumberGeneratorService = cms.Service("RandomNumberGeneratorService",
                                                   highMassCorrectedDiphotonsMC = cms.PSet(initialSeed = cms.untracked.uint32(664)),
                                                  )


#
# load job options
#
from diphotons.MetaData.JobConfig import customize
customize.setDefault("maxEvents",10000)
customize.setDefault("targetLumi",1.e+3)

## 80mb 1.7/fb
## customize.setDefault("puTarget",
##                       '4.98e+04,3.4e+05,3.94e+05,4.15e+05,6.23e+05,7.74e+05,1.27e+06,4e+06,2.08e+07,7.66e+07,1.64e+08,2.42e+08,2.8e+08,2.74e+08,2.29e+08,1.62e+08,9.64e+07,4.8e+07,2.02e+07,7.33e+06,2.36e+06,7.07e+05,2.09e+05,6.58e+04,2.34e+04,9.84e+03,4.85e+03,2.63e+03,1.46e+03,803,428,220,108,51.2,23.3,10.1,4.24,1.7,0.655,0.242,0.0859,0.0292,0.00954,0.00299,0.0009,0.00026,7.19e-05,1.91e-05,4.88e-06,1.19e-06,2.81e-07,6.33e-08'
## )

### ## 69mb 2.4/fb
### customize.setDefault("puTarget",
###                       '1.435e+05,6.576e+05,8.781e+05,1.304e+06,2.219e+06,5.052e+06,1.643e+07,6.709e+07,1.975e+08,3.527e+08,4.44e+08,4.491e+08,3.792e+08,2.623e+08,1.471e+08,6.79e+07,2.748e+07,1.141e+07,5.675e+06,3.027e+06,1.402e+06,5.119e+05,1.467e+05,3.53e+04,8270,2235,721.3,258.8,97.27,36.87,13.73,4.932,1.692,0.5519,0.1706,0.04994,0.01383,0.003627,0.0008996,0.0002111,4.689e-05,9.854e-06,1.959e-06,3.686e-07,6.562e-08,1.105e-08,1.762e-09,2.615e-10,4.768e-11,0,0,0'
### )


customize.setDefault("puTarget",
                     "1.34e+05,6.34e+05,8.42e+05,1.23e+06,2.01e+06,4.24e+06,1.26e+07,4.88e+07,1.56e+08,3.07e+08,4.17e+08,4.48e+08,4.04e+08,3.05e+08,1.89e+08,9.64e+07,4.19e+07,1.71e+07,7.85e+06,4.2e+06,2.18e+06,9.43e+05,3.22e+05,8.9e+04,2.16e+04,5.43e+03,1.6e+03,551,206,80.1,31.2,11.9,4.38,1.54,0.518,0.165,0.0501,0.0144,0.00394,0.00102,0.000251,5.87e-05,1.3e-05,2.74e-06,5.47e-07,1.04e-07,1.86e-08,3.18e-09,5.16e-10,9.35e-11")

## customize.setDefault("puTarget",
##                      '3.82e+03,1.77e+05,4.26e+05,3.55e+05,5.22e+05,7.1e+05,9.13e+05,2.06e+06,9.03e+06,4.3e+07,1.19e+08,2.07e+08,2.67e+08,2.82e+08,2.55e+08,1.97e+08,1.28e+08,6.95e+07,3.18e+07,1.24e+07,4.21e+06,1.3e+06,3.84e+05,1.16e+05,3.85e+04,1.48e+04,6.8e+03,3.54e+03,1.96e+03,1.09e+03,589,308,155,74.8,34.7,15.4,6.59,2.7,1.06,0.4,0.145,0.0503,0.0168,0.00537,0.00165,0.000486,0.000137,3.73e-05,9.71e-06,2.43e-06,5.82e-07,1.34e-07')

import FWCore.ParameterSet.VarParsing as VarParsing
customize.options.register ('selection',
                            "diphoton", # default value
                            VarParsing.VarParsing.multiplicity.singleton, # singleton or list
                            VarParsing.VarParsing.varType.string,          # string, int, or float
                            "selection")
customize.options.register ('massCut',
                            "200", # default value
                            VarParsing.VarParsing.multiplicity.singleton, # singleton or list
                            VarParsing.VarParsing.varType.string,          # string, int, or float
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
customize.options.register ('mctrigger',
                            "", # default value
                            VarParsing.VarParsing.multiplicity.singleton, # singleton or list
                            VarParsing.VarParsing.varType.string,          # string, int, or float
                            "mctrigger")
customize.options.register ('idversion',
                            "V2", # default value
                            VarParsing.VarParsing.multiplicity.singleton, # singleton or list
                            VarParsing.VarParsing.varType.string,          # string, int, or float
                            "idversion")
customize.options.register ('applyDiphotonCorrections',
                            False, # default value
                            VarParsing.VarParsing.multiplicity.singleton, # singleton or list
                            VarParsing.VarParsing.varType.bool,          # string, int, or float
                            "applyDiphotonCorrections")
customize.options.register ('diphotonCorrectionsVersion',
                            "", # default value
                            VarParsing.VarParsing.multiplicity.singleton, # singleton or list
                            VarParsing.VarParsing.varType.string,          # string, int, or float
                            "diphotonCorrectionsVersion")
customize.options.register ('useVtx0',
                            False, # default value
                            VarParsing.VarParsing.multiplicity.singleton, # singleton or list
                            VarParsing.VarParsing.varType.bool,          # string, int, or float
                            "useVtx0")
customize.options.register ('addRegressionInput',
                            False, # default value
                            VarParsing.VarParsing.multiplicity.singleton, # singleton or list
                            VarParsing.VarParsing.varType.bool,          # string, int, or float
                            "addRegressionInput")
customize.parse()

from Configuration.AlCa.autoCond import autoCond
if customize.options.processType == "data":
    process.GlobalTag = GlobalTag(process.GlobalTag, autoCond['run2_data'].replace("::All","") )
else:
    process.GlobalTag = GlobalTag(process.GlobalTag, autoCond['run2_mc'].replace("::All",""))

#
# define minitrees and histograms
#
from flashgg.Taggers.diphotonDumper_cfi import diphotonDumper 
from flashgg.Taggers.photonDumper_cfi import photonDumper 
import flashgg.Taggers.dumperConfigTools as cfgTools

sourceDiphotons = "flashggDiPhotons"

# Track count vertex
if "0T" in customize.idversion:
    # diphotonDumper.src = "flashggDiPhotonsTrkCount"
    sourceDiphotons = "flashggDiPhotonsTrkCount"

if customize.useVtx0:
    from flashgg.MicroAOD.flashggDiPhotons_cfi import flashggDiPhotonsLite
    process.flashggDiPhotonsVtx0 = flashggDiPhotonsLite.clone(VertexSelectorName="FlashggZerothVertexSelector",whichVertex=cms.uint32(0))
    sourceDiphotons = "flashggDiPhotonsVtx0"

diphotonDumper.processId = "test"
diphotonDumper.dumpTrees = False
diphotonDumper.dumpWorkspace = False
diphotonDumper.quietRooFit = True
diphotonDumper.maxCandPerEvent=1
diphotonDumper.nameTemplate = "$PROCESS_$SQRTS_$LABEL_$SUBCAT"
diphotonDumper.throwOnUnclassified = cms.bool(False)

def addGloabalFloat(globalVariables,process,producer,name,expr):    
    getattr(process,producer).variables.append( cms.PSet(tag=cms.untracked.string(name),quantity=cms.untracked.string(expr)) )
    setattr(globalVariables.extraFloats,name,cms.InputTag(producer,name))

def bookCandViewNtProducer(process,name,collection):    
    setattr(process,name,cms.EDProducer(
            "CandViewNtpProducer", 
            src = cms.InputTag(collection), lazyParser = cms.untracked.bool(True),
            variables = cms.VPSet()
            )
            )
process.flashggUnpackedJets = cms.EDProducer("FlashggVectorVectorJetUnpacker",
                                             JetsTag = cms.InputTag("flashggFinalJets"),
                                             NCollections = cms.uint32(8)
                                             )
process.selectedJsets60 = cms.EDFilter("FlashggJetSelector",
                                     src=cms.InputTag("flashggUnpackedJets","0"),
                                     cut=cms.string("pt>60 && abs(eta)<2.5"),
                                )

process.selectedJsets30 = cms.EDFilter("FlashggJetSelector",
                                     src=cms.InputTag("flashggUnpackedJets","0"),
                                     cut=cms.string("pt>30 && abs(eta)<2.5"),
                                )

process.selectedJsets60Fwd = cms.EDFilter("FlashggJetSelector",
                                     src=cms.InputTag("flashggUnpackedJets","0"),
                                     cut=cms.string("pt>30 && abs(eta)<4.7"),
                                )

process.genGravitons = cms.EDProducer("GenParticlePruner",
                                    src = cms.InputTag("flashggPrunedGenParticles"),
                                    select = cms.vstring("drop  *  ", # this is the default
                                                         "keep pdgId = 5100039",
                                                         )
                                    )
### if "Grav" in customize.datasetName():
###     bookCandViewNtProducer(process,"genGr","genGravitons")
###     addGloabalFloat(diphotonDumper.globalVariables,process,"genGr","genVtxZ","vertex.z")
### 
### process.MHT60 = cms.EDProducer("MyMHTProducer",src=cms.InputTag("selectedJsets60"))
### process.MHT30 = cms.EDProducer("MyMHTProducer",src=cms.InputTag("selectedJsets30"))
### process.MHT60Clean = cms.EDProducer("MyMHTProducer",src=cms.InputTag("selectedJsets60"),veto=cms.InputTag("cicDiPhotons"))
### process.MHT30Clean = cms.EDProducer("MyMHTProducer",src=cms.InputTag("selectedJsets30"),veto=cms.InputTag("cicDiPhotons"))
### process.dijet60Clean = cms.EDProducer("MyMHTProducer",src=cms.InputTag("selectedJsets60"),maxCand=cms.int32(2),veto=cms.InputTag("cicDiPhotons"))
### process.dijet30Clean = cms.EDProducer("MyMHTProducer",src=cms.InputTag("selectedJsets30"),maxCand=cms.int32(2),veto=cms.InputTag("cicDiPhotons"))
### 
### bookCandViewNtProducer(process,"mht60","MHT60")
### bookCandViewNtProducer(process,"mht30","MHT30")
### bookCandViewNtProducer(process,"mht60clean","MHT60Clean")
### bookCandViewNtProducer(process,"mht30clean","MHT30Clean")
### bookCandViewNtProducer(process,"dijet60clean","dijet60Clean")
### bookCandViewNtProducer(process,"dijet30clean","dijet30Clean")
### 
### addGloabalFloat(diphotonDumper.globalVariables,process,"mht60","mht60Mass","mass")
### addGloabalFloat(diphotonDumper.globalVariables,process,"mht60","mht60Pt","pt")
### addGloabalFloat(diphotonDumper.globalVariables,process,"mht60","mht60Rapidity","rapidity")
### addGloabalFloat(diphotonDumper.globalVariables,process,"mht60","mht60Phi","phi")
### 
### addGloabalFloat(diphotonDumper.globalVariables,process,"mht60clean","mht60CleanMass","mass")
### addGloabalFloat(diphotonDumper.globalVariables,process,"mht60clean","mht60CleanPt","pt")
### addGloabalFloat(diphotonDumper.globalVariables,process,"mht60clean","mht60CleanRapidity","rapidity")
### addGloabalFloat(diphotonDumper.globalVariables,process,"mht60clean","mht60CleanPhi","phi")
### addGloabalFloat(diphotonDumper.globalVariables,process,"mht60clean","nJets60","numberOfDaughters")
### 
### addGloabalFloat(diphotonDumper.globalVariables,process,"dijet60clean","dijet60CleanMass","mass")
### addGloabalFloat(diphotonDumper.globalVariables,process,"dijet60clean","dijet60CleanPt","pt")
### addGloabalFloat(diphotonDumper.globalVariables,process,"dijet60clean","dijet60CleanRapidity","rapidity")
### addGloabalFloat(diphotonDumper.globalVariables,process,"dijet60clean","dijet60CleanPhi","phi")
### 
### addGloabalFloat(diphotonDumper.globalVariables,process,"dijet30clean","dijet30CleanMass","mass")
### addGloabalFloat(diphotonDumper.globalVariables,process,"dijet30clean","dijet30CleanPt","pt")
### addGloabalFloat(diphotonDumper.globalVariables,process,"dijet30clean","dijet30CleanRapidity","rapidity")
### addGloabalFloat(diphotonDumper.globalVariables,process,"dijet30clean","dijet30CleanPhi","phi")
### 
### addGloabalFloat(diphotonDumper.globalVariables,process,"mht30","mht30Mass","mass")
### addGloabalFloat(diphotonDumper.globalVariables,process,"mht30","mht30Pt","pt")
### addGloabalFloat(diphotonDumper.globalVariables,process,"mht30","mht30Rapidity","rapidity")
### addGloabalFloat(diphotonDumper.globalVariables,process,"mht30","mht30Phi","phi")
### addGloabalFloat(diphotonDumper.globalVariables,process,"mht30","nJets30","numberOfDaughters")
### 
### addGloabalFloat(diphotonDumper.globalVariables,process,"mht30clean","mht30CleanMass","mass")
### addGloabalFloat(diphotonDumper.globalVariables,process,"mht30clean","mht30CleanPt","pt")
### addGloabalFloat(diphotonDumper.globalVariables,process,"mht30clean","mht30CleanRapidity","rapidity")
### addGloabalFloat(diphotonDumper.globalVariables,process,"mht30clean","mht30CleanPhi","phi")
### addGloabalFloat(diphotonDumper.globalVariables,process,"mht30clean","nJets30","numberOfDaughters")
### 
### addGloabalFloat(diphotonDumper.globalVariables,process,"mht30clean","jet1Btag","?numberOfDaughters>0?daughter(0).bDiscriminator('combinedInclusiveSecondaryVertexV2BJetTags'):0")
### addGloabalFloat(diphotonDumper.globalVariables,process,"mht30clean","jet1Pt","?numberOfDaughters>0?daughter(0).pt:0")
### addGloabalFloat(diphotonDumper.globalVariables,process,"mht30clean","jet1Eta","?numberOfDaughters>0?daughter(0).eta:0")
### addGloabalFloat(diphotonDumper.globalVariables,process,"mht30clean","jet1Phi","?numberOfDaughters>0?daughter(0).phi:0")
### 
### addGloabalFloat(diphotonDumper.globalVariables,process,"mht30clean","jet2Btag","?numberOfDaughters>1?daughter(1).bDiscriminator('combinedInclusiveSecondaryVertexV2BJetTags'):0")
### addGloabalFloat(diphotonDumper.globalVariables,process,"mht30clean","jet2Pt","?numberOfDaughters>1?daughter(1).pt:0")
### addGloabalFloat(diphotonDumper.globalVariables,process,"mht30clean","jet2Eta","?numberOfDaughters>1?daughter(1).eta:0")
### addGloabalFloat(diphotonDumper.globalVariables,process,"mht30clean","jet2Phi","?numberOfDaughters>1?daughter(1).phi:0")
### 
### addGloabalFloat(diphotonDumper.globalVariables,process,"mht30clean","jet3Btag","?numberOfDaughters>2?daughter(2).bDiscriminator('combinedInclusiveSecondaryVertexV2BJetTags'):0")
### addGloabalFloat(diphotonDumper.globalVariables,process,"mht30clean","jet3Pt","?numberOfDaughters>2?daughter(2).pt:0")
### addGloabalFloat(diphotonDumper.globalVariables,process,"mht30clean","jet3Eta","?numberOfDaughters>2?daughter(2).eta:0")
### addGloabalFloat(diphotonDumper.globalVariables,process,"mht30clean","jet3Phi","?numberOfDaughters>2?daughter(2).phi:0")
### 
### addGloabalFloat(diphotonDumper.globalVariables,process,"mht30clean","jet4Btag","?numberOfDaughters>3?daughter(3).bDiscriminator('combinedInclusiveSecondaryVertexV2BJetTags'):0")
### addGloabalFloat(diphotonDumper.globalVariables,process,"mht30clean","jet4Pt","?numberOfDaughters>3?daughter(3).pt:0")
### addGloabalFloat(diphotonDumper.globalVariables,process,"mht30clean","jet4Eta","?numberOfDaughters>3?daughter(3).eta:0")
### addGloabalFloat(diphotonDumper.globalVariables,process,"mht30clean","jet4Phi","?numberOfDaughters>3?daughter(3).phi:0")
### 
### bookCandViewNtProducer(process,"met","slimmedMETs")
### addGloabalFloat(diphotonDumper.globalVariables,process,"met","metPt","pt")
### addGloabalFloat(diphotonDumper.globalVariables,process,"met","metPhi","phi")
### addGloabalFloat(diphotonDumper.globalVariables,process,"met","sumEt","sumEt")



variables=["mass","pt","rapidity","eta",
           "vertexZ  := vtx.z", 
           "vertexId := vtx.key",
           ## "satRegressedMass := sqrt( (leadingPhoton.energyAtStep('satRegressedEnergy','initial')*subLeadingPhoton.energyAtStep('satRegressedEnergy','initial')) / (leadingPhoton.energy*subLeadingPhoton.energy) ) * genP4.mass",
           ## "regressedMass := sqrt( (leadingPhoton.energyAtStep('regressedEnergy')*subLeadingPhoton.energyAtStep('regressedEnergy')) / (leadingPhoton.energy*subLeadingPhoton.energy) ) * genP4.mass",
           "genMass := genP4.mass",
           ### "leadSatRegressedEnergy := leadingPhoton.userFloat('satRegressedEnergy')",
           ### "subLeadSatRegressedEnergy := subLeadingPhoton.userFloat('satRegressedEnergy')",
           ### "leadRegressedEnergy := leadingPhoton.userFloat('regressedEnergy')",
           ### "subLeadRegressedEnergy := subLeadingPhoton.userFloat('regressedEnergy')",
           "leadInitialEnergy := leadingPhoton.energyAtStep('initial')",
           "subLeadInitialEnergy := subLeadingPhoton.energyAtStep('initial')",
           "leadEnergy := leadingPhoton.p4.energy",
           "subLeadEnergy := subLeadingPhoton.p4.energy",
           "leadIsSat := leadingPhoton.checkStatusFlag('kSaturated')",
           "subLeadIsSat := subLeadingPhoton.checkStatusFlag('kSaturated')",
           "leadIsWeird := leadingPhoton.checkStatusFlag('kWeird')",
           "subLeadIsWeird := subLeadingPhoton.checkStatusFlag('kWeird')",
           "genLeadPt := ?leadingPhoton.hasMatchedGenPhoton?leadingPhoton.matchedGenPhoton.pt:0",
           "genSubLeadPt := ?subLeadingPhoton.hasMatchedGenPhoton?subLeadingPhoton.matchedGenPhoton.pt:0",
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
           ## "leadCShapeMVA            :=leadingPhoton.userFloat('cShapeMVA')",
           ## "subleadCShapeMVA         :=subLeadingPhoton.userFloat('cShapeMVA')",
           "minR9                    :=min(leadingPhoton.r9,subLeadingPhoton.r9)",
           "maxEta                   :=max(abs(leadingPhoton.superCluster.eta),abs(subLeadingPhoton.superCluster.eta))",
           
           "leadBlockChIso   := leadingView.pfChIso03WrtChosenVtx", 
           "leadBlockPhoIso  := leadingPhoton.pfPhoIso03", 
           
           "leadPhoIsoEA :=  map( abs(leadingPhoton.superCluster.eta) :: 0.,0.9,1.5,2.0,2.2,3. :: 0.21,0.2,0.14,0.22,0.31 )",
           "subleadPhoIsoEA :=  map( abs(subLeadingPhoton.superCluster.eta) :: 0.,0.9,1.5,2.0,2.2,3. :: 0.21,0.2,0.14,0.22,0.31 )",
           
           "leadMatchType            :=leadingPhoton.genMatchType",
           "leadGenIso               :=?leadingPhoton.hasUserFloat('genIso')?leadingPhoton.userFloat('genIso'):0",
           "subleadMatchType         :=subLeadingPhoton.genMatchType",
           "subleadGenIso            :=?subLeadingPhoton.hasUserFloat('genIso')?subLeadingPhoton.userFloat('genIso'):0",
           
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

def makeOneLegInputs(label,obj,inputs):
    out = []
    for inp in inputs:
        if type(inp) == str: 
            name, expr = inp, inp
        else:
            expr, name = inp
        if not name.startswith("_"): name = name[0].capitalize()+name[1:]
        name = "%s%s"  % ( label, name )        
        expr = "%s.%s" % ( obj,   expr )
        ## print expr[14]
        out.append( "%s := %s" % (name,expr) )
        print out[-1]
    return out


variables.extend( makeOneLegInputs("lead","leadingPhoton",[("superCluster.preshowerEnergy","scPreshowerEnergy"),
                                                           ("superCluster.rawEnergy","scRawEnergy"),
                                                           ("superCluster.preshowerEnergyPlane1","scPreshowerEnergyPlane1"),
                                                           ("superCluster.preshowerEnergyPlane2","scPreshowerEnergyPlane2")]) ),
variables.extend( makeOneLegInputs("sublead","subLeadingPhoton",[("superCluster.preshowerEnergy","scPreshowerEnergy"),
                                                                 ("superCluster.rawEnergy","scRawEnergy"),
                                                                 ("superCluster.preshowerEnergyPlane1","preshowerEnergyPlane1"),
                                                                 ("superCluster.preshowerEnergyPlane2","preshowerEnergyPlane2")]) ),


if customize.addRegressionInput:
    regInputs = [
        ("superCluster.clustersSize","scClustersSize"),
        ("superCluster.seed.energy","scSeedEnergy"),
        ("superCluster.energy","scEnergy"),
        
        # ("? hasMatchedGenPhoton ? matchedGenPhoton.energy : 0","etrue"),
        
        ("userInt('seedRecoFlag')","seedRecoFlag"),
        
        ## cluster shapes
        "e1x5",           "full5x5_e1x5",           
        "e2x5",           "full5x5_e2x5",           
        "e3x3",           "full5x5_e3x3",           
        "e5x5",           "full5x5_e5x5",           
        "maxEnergyXtal",  "full5x5_maxEnergyXtal",  
        "sigmaIetaIeta",  "full5x5_sigmaIetaIeta",  
        "r1x5",           "full5x5_r1x5",           
        "r2x5",           "full5x5_r2x5",           
        "r9",             "full5x5_r9",             
        "eMax","e2nd","eTop","eBottom","eLeft","eRight",
        "iEta","iPhi","cryEta","cryPhi",
        
        ## more cluster shapes
        ("e2x5right" ,"e2x5Right"  ),
        ("e2x5left"  ,"e2x5Left"   ),
        ("e2x5top"   ,"e2x5Top"    ),
        ("e2x5bottom","e2x5Bottom" ),
        ("e2x5max"   ,"e2x5Max"    ),
        ("e1x3"      ,"e1x3"       ),
        ("s4"        ,"s4"         ),
        
        ("esEffSigmaRR","sigmaRR"),
        ("spp","covarianceIphiIphi"),
        ("sep","covarianceIetaIphi"),
        ("superCluster.etaWidth","etaWidth"),("superCluster.phiWidth","phiWidth"),
        
        ("checkStatusFlag('kSaturated')","kSaturated"),("checkStatusFlag('kWeird')","kWeird"),
        ]
    
    variables.extend( makeOneLegInputs("lead","leadingPhoton",regInputs))
    variables.extend( makeOneLegInputs("sublead","subLeadingPhoton",regInputs))
    
histograms=["mass>>mass(1500,0,15000)",
            "mass>>lowmass(560,60,200)",
            "genMass>>genmass(1500,0,15000)",            
            "pt>>pt(200,0,200)",
            "rapidity>>rapidity(200,-5,5)",
            "deltaEta>>deltaEta(200,0,5)",
            "cosDeltaPhi>>cosDeltaPhi(200,0,1)",
            "global.rho>>rho(20,0,50)",
            "global.nvtx>>nvtx(51,0.5,50.5)",
            
            ### "global.mht60>>mht60(1500,0,15000)",
            ### "global.mht30>>mht30(1500,0,15000)",
            ### "global.metPt>>met(200,0,200)",
            ### "global.sumEt>>sumEt(1500,0,15000)",
              
            "leadPt>>phoPt(150,0,3000)",
            "subleadPt>>phoPt(150,0,3000)",
            
            "leadPt>>leadPt(200,0,800)",
            "subleadPt>>subleadPt(200,0,800)",
            "leadEta>>leadEta(55,-2.75,2.75)",
            "subleadEta>>subleadEta(55,-2.75,2.75)",
            
            "leadBlockChIso>>leadBlockChIso(120,-10,50)",
            "leadBlockPhoIso>>leadBlockPhoIso(120,-10,50)",
            "leadChIso>>leadChIso(120,-10,50)",
            "leadPhoIso>>leadPhoIso(120,-10,50)",
            "leadNeutIso>>leadNeutIso(120,-10,50)",
            "leadHoE>>leadHoE(40,0,0.2)",
            "leadSigmaIeIe>>leadSigmaIeIe(320,0,3.2e-2)",
            "leadPixSeed>>leadPixSeed(2,-0.5,1.5)",
            "leadPassEleVeto>>leadPassEleVeto(2,-0.5,1.5)",
            
            "subleadBlockChIso>>subleadBlockChIso(120,-10,50)",
            "subleadBlockPhoIso>>subleadBlockPhoIso(120,-10,50)",
            "subleadChIso>>subleadChIso(120,-10,50)",
            "subleadPhoIso>>subleadPhoIso(120,-10,50)",
            "subleadNeutIso>>subleadNeutIso(120,-10,50)",
            "subleadHoE>>subleadHoE(40,0,0.2)",
            "subleadSigmaIeIe>>subleadSigmaIeIe(320,0,3.2e-2)",
            "subleadPixSeed>>subleadPixSeed(2,-0.5,1.5)",
            "subleadPassEleVeto>>subleadPassEleVeto(2,-0.5,1.5)",
            
            
            "leadChIso>>phoChIso(120,-10,50)",
            "subleadChIso>>phoChIso(120,-10,50)",
            "leadPhoIso>>phoPhoIso(120,-10,50)",
            "subleadPhoIso>>phoPhoIso(120,-10,50)",
            "leadSigmaIeIe>>phoSigmaIeIe(320,0,3.2e-2)",
            "subleadSigmaIeIe>>phoSigmaIeIe(320,0,3.2e-2)",
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
    "genPt                   :=?hasMatchedGenPhoton?matchedGenPhoton.pt:0",
    "phoEta                  :=eta",
    "phoR9                   :=r9",
    "phoScEta                :=superCluster.eta",
    "phoPhi                  :=phi",
    
    "phoBlockChIso   := pfChgIso03WrtVtx0", 
    
    "phoBlockPhoIso  := pfPhoIso03", 
    ## "phoRndConePhoIso:= extraPhoIso('rnd03')",
    
    "phoPhoIsoEA :=  map( abs(superCluster.eta) :: 0.,0.9,1.5,2.0,2.2,3. :: 0.21,0.2,0.14,0.22,0.31 )",
    
    "phoMatchType            :=genMatchType",
    "phoGenIso               :=?hasUserFloat('genIso')?userFloat('genIso'):0",
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
    "genPt>>phoPt(145,100,3000)",
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

if (customize.selection=="diphoton" or customize.selection=="photon"):
    if customize.datasetName() and (not "EXOSpring15_v3" in customize.datasetName() or "EXOSpring15_v3v8" in customize.datasetName()):
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

if ":" in customize.massCut:
    massCutEB,massCutEE = map(float,customize.massCut.split(":"))
    massCut = min(massCutEB,massCutEE)
else:
    massCutEB,massCutEE = None,None
    massCut = float(customize.massCut)


if massCutEB or massCutEE:
    cfgTools.addCategory(diphotonDumper,"RejectLowMass",
                         "   (max(abs(leadingPhoton.superCluster.eta),abs(subLeadingPhoton.superCluster.eta))<1.4442 && mass <= %f)"
                         "|| (max(abs(leadingPhoton.superCluster.eta),abs(subLeadingPhoton.superCluster.eta))>1.566  && mass <= %f)" %
                                            (massCutEB,massCutEE),-1)
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
        "file:diphotonsMicroAOD.root"
        #"/store/group/phys_higgs/cmshgg/musella/flashgg/EXOSpring15_v5/Spring15BetaV2-2-gfceadad/SinglePhoton/EXOSpring15_v5-Spring15BetaV2-2-gfceadad-v0-Run2015B-PromptReco-v1/150813_095357/0000/diphotonsMicroAOD_99.root"
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
doDoublePho0T=False
invertEleVeto=False
dumpBits=["HLT_DoublePhoton60","HLT_DoublePhoton85","HLT_Photon250_NoHE","HLT_Photon165_HE"]
askTriggerOnMc=False

if customize.selection == "diphoton":
    mcTriggers=["HLT_DoublePhoton85*","HLT_Photon250_NoHE*","HLT_DoublePhoton60*"] ## ,
    dataTriggers=mcTriggers
elif customize.selection == "photon":
    dataTriggers=["HLT_Photon165*"]
    mcTriggers=dataTriggers
    doSinglePho=True
    doDoublePho=False
    ## askTriggerOnMc=True
elif customize.selection == "electron":
    ## dataTriggers=["HLT_Ele23_WPLoose*"]
    ## mcTriggers=[]
    dataTriggers=["HLT_Ele27_eta2p1_WPLoose_Gsf_v*"]
    ## mcTriggers=["HLT_Ele27_eta2p1_WP75_Gsf_v*"]
    mcTriggers=dataTriggers
    askTriggerOnMc=True
    invertEleVeto=True
elif customize.selection == "dielectron":
    dataTriggers=["*"]
    mcTriggers=dataTriggers
    if customize.options.trigger == "":
        sys.exit("please complete dielectron selection",-1)
        invertEleVeto=True
elif customize.selection == "dielectron0T":
    invertEleVeto=True
    doDoublePho0T=True
    mcTriggers=[]
    askTriggerOnMc=False

if customize.options.trigger != "":
    dataTriggers = customize.options.trigger.split(",")
    mcTriggers = [] ## dataTriggers
    dumpBits.extend( map(lambda x: x.rstrip("*"), dataTriggers)  )
    askTriggerOnMc=False

if customize.options.mctrigger != "":
    mcTriggers = customize.options.mctrigger.split(",")
    
from HLTrigger.HLTfilters.hltHighLevel_cfi import hltHighLevel
dumpBits=set(dumpBits)
if customize.processType == "data" and not "electron" in customize.selection:
    if "Prompt" in customize.datasetName() or "04Dec" in customize.datasetName() or "16Dec2015" in customize.datasetName(): 
        filterProc = "RECO"
    else: 
        filterProc = "PAT"
    diphotonDumper.globalVariables.addTriggerBits = cms.PSet(
        tag=cms.InputTag("TriggerResults","",filterProc),bits=cms.vstring("eeBadScFilter","goodVertices")
        )
elif len(dumpBits) > 0:
    if doDoublePho:
        diphotonDumper.globalVariables.addTriggerBits = cms.PSet(
            tag=cms.InputTag("TriggerResults","","HLT"),bits=cms.vstring(dumpBits)
            )
    if doSinglePho:
        photonDumper.globalVariables.addTriggerBits = cms.PSet(
            tag=cms.InputTag("TriggerResults","","HLT"),bits=cms.vstring(dumpBits)
            )
            

from flashgg.Taggers.genDiphotonDumper_cfi import genDiphotonDumper
from flashgg.Taggers.globalVariables_cff import globalVariables
genDiphotonDumper.dumpGlobalVariables = True
genDiphotonDumper.globalVariables = globalVariables
cfgTools.addCategories(genDiphotonDumper,
                       [("EB","max(abs(leadingPhoton.eta),abs(subLeadingPhoton.eta))<1.4442",0),
                        ("EE","1",0)
                        ],
                       variables=["genMass := mass","pt",
                                  "genLeadPt := leadingPhoton.pt",
                                  "genSubeadPt := subLeadingPhoton.pt",
                                  ],
                       histograms=["genMass>>genmass(1500,0,15000)",
                                   ]
                       )

minimalDumper = diphotonDumper.clone()
cfgTools.dumpOnly(minimalDumper,
                  ["mass","pt","genMass","satRegressedMass","regressedMass",
                   "leadEnergy","subLeadEnergy",
                   "leadSatRegressedEnergy","subLeadSatRegressedEnergy",
                   "leadRegressedEnergy","subLeadRegressedEnergy",
                   "leadIsSat","subLeadIsSat","leadIsWeird","subLeadIsWeird",
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
                            massCut=massCut,ptLead=customize.ptLead,ptSublead=customize.ptSublead,scaling=customize.scaling, ## kinematic cuts
                            computeMVA=False,
                            genIsoDefinition=("genIso",10.),
                            dataTriggers=dataTriggers,
                            mcTriggers=mcTriggers,
                            askTriggerOnMc=askTriggerOnMc, ## if mcTriggers is not empty will still compute efficiencies
                            singlePhoDumperTemplate=photonDumper,
                            applyDiphotonCorrections=customize.applyDiphotonCorrections,
                            diphotonCorrectionsVersion=customize.diphotonCorrectionsVersion,
                            sourceDiphotons=sourceDiphotons
                            )

dumpKinTree=False
if customize.datasetName():
    # drop samples overlap
    if "GJet-HT" in customize.datasetName():
        analysis.keepPFOnly = True
    elif "QCD" in customize.datasetName():
        analysis.keepFFOnly = True
    elif "DiPhotonJetsBox_MGG-80toInf_13TeV-Sherpa" in customize.datasetName():
        analysis.vetoGenDiphotons = 200.
    
    # alyaws get full info for signal
    if "Grav" in customize.datasetName():
        dumpKinTree=True
        minimalDumper=diphotonDumper


## kinematic selection
analysis.addKinematicSelection(process,dumpTrees=dumpKinTree,splitByIso=True
                               )

if not dumpKinTree: minimalDumper=diphotonDumper

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
    if doDoublePho0T:
        highMassCiCDiPhotons0T.variables[-1] = "? matchedGsfTrackInnerMissingHits==0 ? 2 : 0"
    else:
        highMassCiCDiPhotons.variables[-1] = "hasPixelSeed"
        highMassCiCDiPhotonsSB.variables[-1] = "hasPixelSeed"
        ## highMassCiCDiPhotons.variables[-1] = "-(passElectronVeto-1)"
        ## highMassCiCDiPhotonsSB.variables[-1] = "-(passElectronVeto-1)"

# gen-only analysis
if( customize.processType!="data" ):
    analysis.addGenOnlySelection(process,genDiphotonDumper)


## Diphotons 0T
if doDoublePho0T:
    analysis.addAnalysisSelection(process, "cic",
                                  highMassCiCDiPhotons0T,
                                  dumpTrees=True, dumpWorkspace=False, dumpHistos=True, splitByIso=True,
                                  dumperTemplate=diphotonDumper,
                                  nMinusOne=[(0,"NoSieie",        True, False, True), ## removeIndex(es), label, dumpTree, dumpWorkspace, dumpHistos
                                             (1,"NoSipip",        True, False, False),
                                             (2,"NoTrkIso",       True, False, False),
                                             (3,"NoPhoIso",       True, False, False),
                                             (4,"NoEleVeto",      True, False, False)
                                         ]
                              )    
    
elif doDoublePho:
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

if doSinglePho:

    if invertEleVeto:
        highMassCiCPhotons.variables[-1] = "hasPixelSeed"
        highMassCiCPhotonsSB.variables[-1] = "hasPixelSeed"
        ## highMassCiCPhotons.variables[-1] = "-(passElectronVeto-1)"
        ## highMassCiCPhotonsSB.variables[-1] = "-(passElectronVeto-1)"

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


### process.p = cms.Path(process.genGr)
### 
### ### process.out = cms.OutputModule("PoolOutputModule", 
###                                fileName = cms.untracked.string('diphotonsMicroAOD.root'),
###                                outputCommands = cms.untracked.vstring("drop *","keep *_genGr_*_*", "keep *_genGravitons_*_*"),
###                                SelectEvents = cms.untracked.PSet(SelectEvents = cms.vstring('p')),
###                                )
### process.e = cms.EndPath(process.out)


# load appropriate scale and smearing bins here
# systematics customization scripts will take care of adjusting flashggDiPhotonSystematics
process.load('flashgg.Systematics.escales.escale76X_16DecRereco_2015')

# this will call customize(process), configure the analysis paths and make the process unscheduled
analysis.customize(process,customize)

## print process.dumpPython()
