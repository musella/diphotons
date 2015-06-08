import FWCore.ParameterSet.Config as cms
import FWCore.Utilities.FileUtils as FileUtils

process = cms.Process("diPhoAna")

process.load("FWCore.MessageService.MessageLogger_cfi")

process.load("Configuration.StandardSequences.GeometryDB_cff")
process.load("Configuration.StandardSequences.MagneticField_cff")
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
process.GlobalTag.globaltag = 'POSTLS170_V5::All'
#process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff")
#process.GlobalTag.globaltag = 'POSTLS170_V5'

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32( 1000 ) )

process.source = cms.Source("PoolSource",
                            fileNames=cms.untracked.vstring(

        # Phys14, GG+jets, AnV1 
        #"/store/group/phys_higgs/cmshgg/musella/flashgg/ExoPhys14ANv1/diphotonsPhys14AnV1/GGJets_M-500To1000_Pt-50_13TeV-sherpa/ExoPhys14ANv1-diphotonsPhys14AnV1-v0-Phys14DR-PU20bx25_PHYS14_25_V1-v1/150330_141301/0000/diphotonsMicroAOD_1.root"

        # Phys14, RS 3TeV, AnV1 
        "/store/group/phys_higgs/cmshgg/musella/flashgg/ExoPhys14ANv1/diphotonsPhys14AnV1/RSGravToGG_kMpl-01_M-3000_Tune4C_13TeV-pythia8/ExoPhys14ANv1-diphotonsPhys14AnV1-v0-Phys14DR-PU30bx50_PHYS14_25_V1-v1/150330_141554/0000/diphotonsMicroAOD_2.root"
        )
                            )

process.load("flashgg/MicroAOD/flashggPhotons_cfi")
process.load("flashgg/MicroAOD/flashggDiPhotons_cfi")

process.TFileService = cms.Service("TFileService",fileName = cms.string("diPhotons.root"))

process.diPhoAna = cms.EDAnalyzer('DiPhoAnalyzer',
                                  VertexTag = cms.untracked.InputTag('offlineSlimmedPrimaryVertices'),
                                  genPhotonExtraTag = cms.InputTag("flashggGenPhotonsExtra"),    
                                  DiPhotonTag = cms.untracked.InputTag('flashggDiPhotons'),
                                  reducedBarrelRecHitCollection = cms.InputTag('reducedEgamma','reducedEBRecHits'),
                                  reducedEndcapRecHitCollection = cms.InputTag('reducedEgamma','reducedEERecHits'),
                                  PileupTag = cms.untracked.InputTag('addPileupInfo'),
                                  generatorInfo = cms.InputTag("generator"),
                                  dopureweight = cms.untracked.int32(0),
                                  sampleIndex  = cms.untracked.int32(101),   # 5
                                  puWFileName  = cms.string('xxx'),   # chiara  
                                  xsec         = cms.untracked.double(0.172872),
                                  kfac         = cms.untracked.double(1.),
                                  #sumDataset   = cms.untracked.double(2236.0)
                                  sumDataset   = cms.untracked.double(49972.0)
                                  )

process.p = cms.Path(process.diPhoAna)

