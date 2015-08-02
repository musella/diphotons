import FWCore.ParameterSet.Config as cms
import FWCore.Utilities.FileUtils as FileUtils

isMC = True

process = cms.Process("tnpAna")

process.load("FWCore.MessageService.MessageLogger_cfi")

process.load("Configuration.StandardSequences.GeometryDB_cff")
process.load("Configuration.StandardSequences.MagneticField_cff")
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff")

from Configuration.AlCa.GlobalTag import GlobalTag
if (isMC):
    #process.GlobalTag = GlobalTag(process.GlobalTag, 'MCRUN2_74_V9A', '')           # 50ns
    process.GlobalTag = GlobalTag(process.GlobalTag, 'MCRUN2_74_V9', '')            # 25ns
else:
    process.GlobalTag = GlobalTag(process.GlobalTag, 'GR_P_V56', '')

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32( 10 ) )

process.source = cms.Source("PoolSource",
                            fileNames=cms.untracked.vstring(

        # Spring15, DY
        "file:diphotonsMicroAOD_1.root"
        )
                            )

process.load("flashgg/MicroAOD/flashggPhotons_cfi")
process.load("flashgg/MicroAOD/flashggElectrons_cfi")
process.load("flashgg/MicroAOD/flashggDiPhotons_cfi")

process.TFileService = cms.Service("TFileService",fileName = cms.string("TaP_output.root"))

process.tnpAna = cms.EDAnalyzer('TaPAnalyzer',
                                VertexTag = cms.untracked.InputTag('offlineSlimmedPrimaryVertices'),
                                ElectronTag=cms.InputTag('flashggElectrons'),
                                genPhotonExtraTag = cms.InputTag("flashggGenPhotonsExtra"),    
                                DiPhotonTag = cms.untracked.InputTag('flashggDiPhotons'),
                                PileupTag = cms.untracked.InputTag('addPileupInfo'),

                                bits = cms.InputTag("TriggerResults","","HLT"),
                                objects = cms.InputTag("selectedPatTrigger"),

                                generatorInfo = cms.InputTag("generator"),
                                dopureweight = cms.untracked.int32(0),
                                sampleIndex  = cms.untracked.int32(101),   # 5
                                puWFileName  = cms.string('xxx'),   # chiara  
                                xsec         = cms.untracked.double(1.),
                                kfac         = cms.untracked.double(1.),
                                sumDataset   = cms.untracked.double(1.)
                                )

process.p = cms.Path(process.tnpAna)

