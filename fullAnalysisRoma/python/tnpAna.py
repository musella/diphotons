import FWCore.ParameterSet.Config as cms
import FWCore.Utilities.FileUtils as FileUtils

isMC = True
is25ns = True
is2012B = False;
is2012C = True;
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

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32( -1 ) )

process.source = cms.Source("PoolSource",
                            fileNames=cms.untracked.vstring(
        "/store/group/phys_higgs/cmshgg/musella/flashgg/EXOSpring15_v3/Spring15BetaV2-2-gfceadad/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/EXOSpring15_v3-Spring15BetaV2-2-gfceadad-v0-RunIISpring15DR74-Asympt25ns_MCRUN2_74_V9-v3/150807_132311/0000/diphotonsMicroAOD_10.root"
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
                                PileupTag = cms.untracked.InputTag('addPileupInfo'),

                                bits = cms.InputTag("TriggerResults","","HLT"),
                                objects = cms.InputTag("selectedPatTrigger"),

                                MetTag=cms.InputTag('slimmedMETs'),

                                generatorInfo = cms.InputTag("generator"),
                                dopureweight = cms.untracked.int32(0),
                                sampleIndex  = cms.untracked.int32(1), 
                                puWFileName  = cms.string('xxx'),   # chiara  
                                xsec         = cms.untracked.double(6025),
                                kfac         = cms.untracked.double(1.),
                                sumDataset   = cms.untracked.double(1.)
                                )

process.p = cms.Path(process.tnpAna)

