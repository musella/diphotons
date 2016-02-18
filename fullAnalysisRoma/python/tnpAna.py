import FWCore.ParameterSet.Config as cms
import FWCore.Utilities.FileUtils as FileUtils

isMC = True
is25ns = True
is2012B = False;
is2012C = False;
is2012D = False;

process = cms.Process("tnpAna")

process.load("FWCore.MessageService.MessageLogger_cfi")

process.load("Configuration.StandardSequences.GeometryDB_cff")
process.load("Configuration.StandardSequences.MagneticField_cff")
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff")

from Configuration.AlCa.GlobalTag import GlobalTag

if (isMC and is25ns==False):
    process.GlobalTag = GlobalTag(process.GlobalTag, 'MCRUN2_74_V9A', '')     
elif (isMC and is25ns):  
    process.GlobalTag = GlobalTag(process.GlobalTag, 'MCRUN2_74_V9', '')      
elif (isMC==False and is2012B):
    process.GlobalTag = GlobalTag(process.GlobalTag, '74X_dataRun2_Prompt_v1', '')
elif (isMC==False and is2012C): 
    process.GlobalTag = GlobalTag(process.GlobalTag, '74X_dataRun2_Prompt_v1', '')
elif ((isMC==False and is2012D)):
    process.GlobalTag = GlobalTag(process.GlobalTag, '74X_dataRun2_Prompt_v2', '')
print process.GlobalTag

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32( 5 ) )

process.source = cms.Source("PoolSource",
                            fileNames=cms.untracked.vstring(
        # DY
        #"/store/group/phys_higgs/cmshgg/musella/flashgg/EXOSpring15_7415_v2/diphotons_7415_v2/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/EXOSpring15_7415_v2-diphotons_7415_v2-v0-RunIISpring15MiniAODv2-74X_mcRun2_asymptotic_v2-v1/151019_005506/0000/diphotonsMicroAOD_10.root"
        # singleEle
        #"/store/group/phys_higgs/cmshgg/musella/flashgg/EXOSpring15_7412_v2/diphotons_7412_v1/SingleElectron/EXOSpring15_7412_v2-diphotons_7412_v1-v0-Run2015C-PromptReco-v1/150927_235231/0000/diphotonsMicroAOD_11.root"
        "/store/group/phys_higgs/cmshgg/sethzenz/flashgg/RunIISpring15-ReMiniAOD-BetaV7-25ns/Spring15BetaV7/GJet_Pt-20to40_DoubleEMEnriched_MGG-80toInf_TuneCUETP8M1_13TeV_Pythia8/RunIISpring15-ReMiniAOD-BetaV7-25ns-Spring15BetaV7-v0-RunIISpring15MiniAODv2-74X_mcRun2_asymptotic_v2-v1/151021_151907/0000/myMicroAODOutputFile_13.root"
        )
                            )

process.load("flashgg/MicroAOD/flashggPhotons_cfi")
process.load("flashgg/MicroAOD/flashggElectrons_cfi")
process.load("flashgg/MicroAOD/flashggDiPhotons_cfi")

process.TFileService = cms.Service("TFileService",
                                   fileName = cms.string("TaP_output.root"))

process.tnpAna = cms.EDAnalyzer('TaPAnalyzer',
                                VertexTag = cms.untracked.InputTag('offlineSlimmedPrimaryVertices'),
                                ElectronTag=cms.InputTag('flashggElectrons'),
                                genPhotonExtraTag = cms.InputTag("flashggGenPhotonsExtra"),    
                                DiPhotonTag = cms.untracked.InputTag('flashggDiPhotons'),
                                PileUpTag = cms.untracked.InputTag('slimmedAddPileupInfo'),

                                bits = cms.InputTag("TriggerResults","","HLT"),
                                objects = cms.InputTag("selectedPatTrigger"),

                                MetTag=cms.InputTag('slimmedMETs'),

                                reducedBarrelRecHitCollection = cms.InputTag('reducedEgamma','reducedEBRecHits'), 
                                reducedEndcapRecHitCollection = cms.InputTag('reducedEgamma','reducedEERecHits'),

                                generatorInfo = cms.InputTag("generator"),
                                dopureweight = cms.untracked.int32(1),
                                sampleIndex  = cms.untracked.int32(1), 
                                #puWFileName  = cms.string('pileupWeights_69mb.root'),   # chiara  
                                puWFileName  = cms.string('/afs/cern.ch/user/c/crovelli/public/json2015/doubleEG/pileupWeights___processedAndGolden_finalAfewMissing__69mb.root'),
                                xsec         = cms.untracked.double(6025),
                                kfac         = cms.untracked.double(1.),
                                sumDataset   = cms.untracked.double(1.)
                                )

process.p = cms.Path(process.tnpAna)

