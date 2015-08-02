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
    process.GlobalTag.globaltag = 'MCRUN2_74_V9A'         # 50ns 
    #process.GlobalTag.globaltag = 'MCRUN2_74_V9'         # 25ns 
else:
    process.GlobalTag = GlobalTag(process.GlobalTag, 'GR_P_V56', '')

process.MessageLogger.cerr.FwkReport.reportEvery = cms.untracked.int32( 1000 )

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32( -1 ) )

process.source = cms.Source("PoolSource",
                            fileNames = cms.untracked.vstring('/store/group/phys_higgs/cmshgg/musella/flashgg/ExoPhys14_v2/diphotonsPhys14V2/RSGravToGG_kMpl001_M_5000_Tune4C_13TeV_pythia8/ExoPhys14_v2-diphotonsPhys14V2-v0-Phys14DR-PU20bx25_PHYS14_25_V1-v1/150128_133931/0000/myOutputFile_1.root'
                            )

process.load("flashgg/MicroAOD/flashggPhotons_cfi")
process.load("flashgg/MicroAOD/flashggDiPhotons_cfi")
process.load("flashgg/MicroAOD/flashggElectrons_cfi")

process.TFileService = cms.Service("TFileService",fileName = cms.string("OUTPUT"))

process.tnpAna = cms.EDAnalyzer('TaPAnalyzer',
                                VertexTag = cms.untracked.InputTag('offlineSlimmedPrimaryVertices'),
                                ElectronTag=cms.InputTag('flashggElectrons'),
                                genPhotonExtraTag = cms.InputTag("flashggGenPhotonsExtra"),   
                                DiPhotonTag = cms.untracked.InputTag('flashggDiPhotons'),
                                PileupTag = cms.untracked.InputTag('addPileupInfo'),
                                bits = cms.InputTag("TriggerResults","","HLT"),
                                objects = cms.InputTag("selectedPatTrigger"),
                                generatorInfo = cms.InputTag("generator"),
                                dopureweight = PU,
                                sampleIndex  = SI,
                                puWFileName  = weights,
                                xsec         = XS,
                                kfac         = KF,
                                sumDataset   = SDS
                                )

process.p = cms.Path(process.tnpAna)

