import FWCore.ParameterSet.Config as cms
import FWCore.Utilities.FileUtils as FileUtils

process = cms.Process("FLASHggMicroAOD")

process.load("FWCore.MessageService.MessageLogger_cfi")

process.load("Configuration.StandardSequences.GeometryDB_cff")
process.load("Configuration.StandardSequences.MagneticField_cff")
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
process.GlobalTag.globaltag = 'POSTLS170_V5::All'

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32( 100000000 ) )
process.MessageLogger.cerr.FwkReport.reportEvery = cms.untracked.int32( 100 )
process.MessageLogger.cerr.threshold = 'ERROR' # can't get suppressWarning to work: disable all warnings for now
# process.MessageLogger.suppressWarning.extend(['SimpleMemoryCheck','MemoryCheck']) # this would have been better...

# Uncomment the following if you notice you have a memory leak
# This is a lightweight tool to digg further
process.SimpleMemoryCheck = cms.Service("SimpleMemoryCheck",
                                        ignoreTotal = cms.untracked.int32(1),
                                        monitorPssAndPrivate = cms.untracked.bool(True)
                                       )

process.source = cms.Source("PoolSource",
                            fileNames=cms.untracked.vstring(
        "/store/relval/CMSSW_7_2_0/RelValTTbar_13/MINIAODSIM/PU25ns_PHYS14_25_V1_Phys14-v2/00000/603403B7-9E59-E411-B5E0-0026189438B3.root"
        ## "/store/relval/CMSSW_7_2_0/RelValH130GGgluonfusion_13/MINIAODSIM/PU25ns_PHYS14_25_V1_Phys14-v2/00000/1830E403-9F59-E411-9BAC-0025905A48BC.root"
        ## "/store/mc/Spring14miniaod/GJets_HT-600toInf_Tune4C_13TeV-madgraph-tauola/MINIAODSIM/PU20bx25_POSTLS170_V5-v1/00000/02ACF78E-9621-E411-BD22-0025904B2ABC.root",
        ## "/store/mc/Spring14miniaod/GJets_HT-600toInf_Tune4C_13TeV-madgraph-tauola/MINIAODSIM/PU20bx25_POSTLS170_V5-v1/00000/0A9AA777-8521-E411-B528-0025904CC686.root",
        ## "/store/mc/Spring14miniaod/GJets_HT-600toInf_Tune4C_13TeV-madgraph-tauola/MINIAODSIM/PU20bx25_POSTLS170_V5-v1/00000/14AA7B7A-8521-E411-B204-0025904A862C.root",
        ## "/store/mc/Spring14miniaod/GJets_HT-600toInf_Tune4C_13TeV-madgraph-tauola/MINIAODSIM/PU20bx25_POSTLS170_V5-v1/00000/18DB3235-9321-E411-AB87-00259082116C.root",
        ## "/store/mc/Spring14miniaod/GJets_HT-600toInf_Tune4C_13TeV-madgraph-tauola/MINIAODSIM/PU20bx25_POSTLS170_V5-v1/00000/20257463-8621-E411-9C49-0025904D9D48.root",
        ## "/store/mc/Spring14miniaod/GJets_HT-600toInf_Tune4C_13TeV-madgraph-tauola/MINIAODSIM/PU20bx25_POSTLS170_V5-v1/00000/2A034660-9321-E411-A59B-002590494D4C.root",
        ## "/store/mc/Spring14miniaod/GJets_HT-600toInf_Tune4C_13TeV-madgraph-tauola/MINIAODSIM/PU20bx25_POSTLS170_V5-v1/00000/2A855B55-9321-E411-9A83-002590496AD4.root",
        ## "/store/mc/Spring14miniaod/GJets_HT-600toInf_Tune4C_13TeV-madgraph-tauola/MINIAODSIM/PU20bx25_POSTLS170_V5-v1/00000/2A9321EA-8421-E411-8492-0025904E41E6.root",
        ## "/store/mc/Spring14miniaod/GJets_HT-600toInf_Tune4C_13TeV-madgraph-tauola/MINIAODSIM/PU20bx25_POSTLS170_V5-v1/00000/2C51E682-8521-E411-9B1D-0025904B1284.root",
        ## "/store/mc/Spring14miniaod/GJets_HT-600toInf_Tune4C_13TeV-madgraph-tauola/MINIAODSIM/PU20bx25_POSTLS170_V5-v1/00000/305BE282-8521-E411-95B4-0025904B1284.root",
        )
                            )
process.load("flashgg/MicroAOD/flashggMicroAODSequence_cff")

process.flashggPhotons.copyExtraGenInfo = True

process.flashggPhotons.extraCaloIsolations.extend([
        # photon and neutral iso w/ overlap removal
        cms.PSet(overlapRemoval=cms.bool(True),
                 type=cms.int32(4), # photon, use str to enum 
                 vetoRegions=cms.vdouble(0.3,0.,0.,0.,0.,0.,0.)
                 ),
        cms.PSet(overlapRemoval=cms.bool(True),
                 type=cms.int32(5), # neutral hadron, use str to enum 
                 vetoRegions=cms.vdouble(0.3,0.1,0.1,0.,0.,0.,0.)
                 ),
        
        # photon iso w/ veto cones
        cms.PSet(overlapRemoval=cms.bool(False),
                 type=cms.int32(4), # photon, use str to enum 
                 vetoRegions=cms.vdouble(0.3,0.0,0.050,0.01,0.0,0.0,0.0)
                 ),
        cms.PSet(overlapRemoval=cms.bool(False),
                 type=cms.int32(4), # photon, use str to enum 
                 vetoRegions=cms.vdouble(0.3,0.0,0.060,0.012,0.0,0.0,0.0)
                 ),
        cms.PSet(overlapRemoval=cms.bool(False),
                 type=cms.int32(4), # photon, use str to enum 
                 vetoRegions=cms.vdouble(0.3,0.0,0.070,0.015,0.0,0.0,0.0)
                 ),
        cms.PSet(overlapRemoval=cms.bool(False),
                 type=cms.int32(4), # photon, use str to enum 
                 vetoRegions=cms.vdouble(0.3,0.0,0.080,0.02,0.0,0.0,0.0)
                 ),
        cms.PSet(overlapRemoval=cms.bool(True),
                 type=cms.int32(4), # photon, use str to enum 
                 vetoRegions=cms.vdouble(0.3,0.0,0.050,0.01,0.0,0.0,0.0)
                 ),

                
        # netural iso rings
         cms.PSet(overlapRemoval=cms.bool(True),
                 type=cms.int32(5), # neutral hadron, use str to enum 
                 vetoRegions=cms.vdouble(0.1,0.,0.,0.,0.,0.,0.)
                 ),
        cms.PSet(overlapRemoval=cms.bool(True),
                 type=cms.int32(5), # neutral hadron, use str to enum 
                 vetoRegions=cms.vdouble(0.12,0.1,0.1,0.,0.,0.,0.)
                 ),
        cms.PSet(overlapRemoval=cms.bool(True),
                 type=cms.int32(5), # neutral hadron, use str to enum 
                 vetoRegions=cms.vdouble(0.15,0.12,0.12,0.,0.,0.,0.)
                 ),
        cms.PSet(overlapRemoval=cms.bool(True),
                 type=cms.int32(5), # neutral hadron, use str to enum 
                 vetoRegions=cms.vdouble(0.2,0.15,0.15,0.,0.,0.,0.)
                 ),
        cms.PSet(overlapRemoval=cms.bool(True),
                 type=cms.int32(5), # neutral hadron, use str to enum 
                 vetoRegions=cms.vdouble(0.3,0.2,0.2,0.,0.,0.,0.)
                 ),
        
        cms.PSet(overlapRemoval=cms.bool(False),
                 type=cms.int32(5), # neutral hadron, use str to enum 
                 vetoRegions=cms.vdouble(0.1,0.,0.,0.,0.,0.,0.)
                 ),
        cms.PSet(overlapRemoval=cms.bool(False),
                 type=cms.int32(5), # neutral hadron, use str to enum 
                 vetoRegions=cms.vdouble(0.12,0.1,0.1,0.,0.,0.,0.)
                 ),
        cms.PSet(overlapRemoval=cms.bool(False),
                 type=cms.int32(5), # neutral hadron, use str to enum 
                 vetoRegions=cms.vdouble(0.15,0.12,0.12,0.,0.,0.,0.)
                 ),
        cms.PSet(overlapRemoval=cms.bool(False),
                 type=cms.int32(5), # neutral hadron, use str to enum 
                 vetoRegions=cms.vdouble(0.2,0.15,0.15,0.,0.,0.,0.)
                 ),
        cms.PSet(overlapRemoval=cms.bool(False),
                 type=cms.int32(5), # neutral hadron, use str to enum 
                 vetoRegions=cms.vdouble(0.3,0.2,0.2,0.,0.,0.,0.)
                 ),

        cms.PSet(overlapRemoval=cms.bool(True),
                 type=cms.int32(4), # photon, use str to enum 
                 vetoRegions=cms.vdouble(0.070,0.050,0.050,0.0,0.0,0.0,0.0)
                 ),
        cms.PSet(overlapRemoval=cms.bool(True),
                 type=cms.int32(4), # photon, use str to enum 
                 vetoRegions=cms.vdouble(0.080,0.070,0.070,0.0,0.0,0.0,0.0)
                 ),
        cms.PSet(overlapRemoval=cms.bool(True),
                 type=cms.int32(4), # photon, use str to enum 
                 vetoRegions=cms.vdouble(0.1,0.080,0.080,0.0,0.0,0.0,0.0)
                 ),
        cms.PSet(overlapRemoval=cms.bool(True),
                 type=cms.int32(4), # photon, use str to enum 
                 vetoRegions=cms.vdouble(0.15,0.1,0.1,0.0,0.0,0.0,0.0)
                 ),
        cms.PSet(overlapRemoval=cms.bool(True),
                 type=cms.int32(4), # photon, use str to enum 
                 vetoRegions=cms.vdouble(0.2,0.15,0.15,0.0,0.0,0.0,0.0)
                 ),
        cms.PSet(overlapRemoval=cms.bool(True),
                 type=cms.int32(4), # photon, use str to enum 
                 vetoRegions=cms.vdouble(0.3,0.2,0.2,0.0,0.0,0.0,0.0)
                 ),
        cms.PSet(overlapRemoval=cms.bool(False),
                 type=cms.int32(4), # photon, use str to enum 
                 vetoRegions=cms.vdouble(0.070,0.050,0.050,0.0,0.0,0.0,0.0)
                 ),
        cms.PSet(overlapRemoval=cms.bool(False),
                 type=cms.int32(4), # photon, use str to enum 
                 vetoRegions=cms.vdouble(0.080,0.070,0.070,0.0,0.0,0.0,0.0)
                 ),
        cms.PSet(overlapRemoval=cms.bool(False),
                 type=cms.int32(4), # photon, use str to enum 
                 vetoRegions=cms.vdouble(0.1,0.080,0.080,0.0,0.0,0.0,0.0)
                 ),
        cms.PSet(overlapRemoval=cms.bool(False),
                 type=cms.int32(4), # photon, use str to enum 
                 vetoRegions=cms.vdouble(0.15,0.1,0.1,0.0,0.0,0.0,0.0)
                 ),
        cms.PSet(overlapRemoval=cms.bool(False),
                 type=cms.int32(4), # photon, use str to enum 
                 vetoRegions=cms.vdouble(0.2,0.15,0.15,0.0,0.0,0.0,0.0)
                 ),
        cms.PSet(overlapRemoval=cms.bool(False),
                 type=cms.int32(4), # photon, use str to enum 
                 vetoRegions=cms.vdouble(0.3,0.2,0.2,0.0,0.0,0.0,0.0)
                 ),

        ]
                                                  )

from flashgg.MicroAODProducers.flashggMicroAODOutputCommands_cff import microAODDefaultOutputCommand, microAODDebugOutputCommand

process.out = cms.OutputModule("PoolOutputModule", fileName = cms.untracked.string('myOutputFile.root'),
                               outputCommands = microAODDefaultOutputCommand
                               )

### process.out.outputCommands.extend( microAODDebugOutputCommand )
process.out.outputCommands.extend([### "keep *_eventCount_*_*",
                                   ### "keep *_weightsCount_*_*",
                                   ### "keep *_prunedGenParticles_*_*",
                                   ### "keep *_packedGenParticles_*_*",
                                   "keep *_reducedEgamma_reduced*RecHits_*",
                                   "keep *_flashggDiPhotons_*_*"
                                   ]
                                  )

process.p = cms.Path((process.eventCount+process.weightsCount
                      +process.flashggVertexMapUnique+process.flashggVertexMapNonUnique+process.flashggElectrons
                      +process.flashggMicroAODGenSequence
                      )
                     *process.flashggPhotons*process.flashggDiPhotons
                     )

process.e = cms.EndPath(process.out)
