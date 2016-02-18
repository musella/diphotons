import FWCore.ParameterSet.Config as cms
import FWCore.Utilities.FileUtils as FileUtils

process = cms.Process("singlePhoAna")

process.load("FWCore.MessageService.MessageLogger_cfi")

process.load("Configuration.StandardSequences.GeometryDB_cff")
process.load("Configuration.StandardSequences.MagneticField_cff")
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
#process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff")
process.GlobalTag.globaltag = 'POSTLS170_V5::All'

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32( -1 ) )

process.MessageLogger.cerr.FwkReport.reportEvery = cms.untracked.int32( 100 )
process.MessageLogger.cerr.threshold = 'ERROR' 
process.eventCount = cms.EDProducer("EventCountProducer")

process.source = cms.Source("PoolSource",
                            fileNames = cms.untracked.vstring('pippo.root'
                                                              )
                            
process.load("flashgg/MicroAOD/flashggPhotons_cfi")

process.TFileService = cms.Service("TFileService",fileName = cms.string("OUTPUT"))

process.singlePhoAna = cms.EDAnalyzer('SinglePhoAnalyzer',
                                      packedGenParticles = cms.untracked.InputTag('flashggGenPhotons'),
                                      reducedBarrelRecHitCollection = cms.InputTag('reducedEgamma','reducedEBRecHits'),
                                      reducedEndcapRecHitCollection = cms.InputTag('reducedEgamma','reducedEERecHits')
                                      )

process.p = cms.Path(process.singlePhoAna)
