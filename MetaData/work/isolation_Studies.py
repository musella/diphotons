import FWCore.ParameterSet.Config as cms
import FWCore.Utilities.FileUtils as FileUtils

process = cms.Process("FLASHggMicroAOD")

process.load("FWCore.MessageService.MessageLogger_cfi")

## process.load("Configuration.StandardSequences.MagneticField_38T_cff")
## process.load("Geometry.CMSCommonData.cmsIdealGeometryXML_cfi")
## process.load("Geometry.CaloEventSetup.CaloGeometry_cff")
## process.load("Geometry.CaloEventSetup.CaloTopology_cfi")
## process.load("Geometry.CaloEventSetup.EcalTrigTowerConstituents_cfi")
## process.load("CalibCalorimetry.EcalTrivialCondModules.EcalTrivialCondRetriever_cfi")
## process.load("CalibCalorimetry.EcalLaserCorrection.ecalLaserCorrectionService_cfi")

process.load("Configuration.StandardSequences.GeometryDB_cff")
process.load("Configuration.StandardSequences.MagneticField_cff")
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
process.GlobalTag.globaltag = 'POSTLS170_V5::All'

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32( 10000 ) )
### process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32( 10 ) )
process.MessageLogger.cerr.FwkReport.reportEvery = 100
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
        ## "/store/mc/Phys14DR/RSGravToGG_kMpl-001_M-1500_Tune4C_13TeV-pythia8/MINIAODSIM/PU20bx25_PHYS14_25_V1-v1/10000/065B2E1D-1468-E411-946C-002590200A7C.root"
        ## "/store/relval/CMSSW_7_2_0/RelValTTbar_13/MINIAODSIM/PU25ns_PHYS14_25_V1_Phys14-v2/00000/603403B7-9E59-E411-B5E0-0026189438B3.root"
        ## "file:/tmp/musella/1830E403-9F59-E411-9BAC-0025905A48BC.root"
        ## "/store/relval/CMSSW_7_2_0/RelValH130GGgluonfusion_13/MINIAODSIM/PU25ns_PHYS14_25_V1_Phys14-v2/00000/1830E403-9F59-E411-9BAC-0025905A48BC.root"
        "/store/mc/Phys14DR/GJets_HT-100to200_Tune4C_13TeV-madgraph-tauola/MINIAODSIM/PU20bx25_PHYS14_25_V1-v1/00000/00D67F78-2873-E411-B3BB-0025907DC9C0.root",
        ## "/store/mc/Phys14DR/GGJets_M-1000To2000_Pt-50_13TeV-sherpa/MINIAODSIM/PU20bx25_PHYS14_25_V1-v1/10000/6844B8AF-D06B-E411-9CE0-002590D0B040.root"
        )
                            )
process.load("flashgg/MicroAOD/flashggMicroAODSequence_cff")
process.flashggPhotons.copyExtraGenInfo = True
process.flashggPrunedGenParticles.select.extend(["keep status = 3",
                                                 "keep status = 22",
                                                 "keep status = 23",
                                                 "++keep pdgId = 22",
                                                 "keep++ pdgId = 39",
                                                 "keep++ pdgId = 5000039",
                                                 ])
process.flashggGenPhotonsExtra.promptMothers=cms.vint32(39,5000039,5100039)

process.vetoPhotons = cms.EDFilter("CandPtrSelector",
                                   src=cms.InputTag("flashggPhotons"),
                                   cut=cms.string("pt>10"),
                                   )
process.vetoJets = cms.EDFilter("CandPtrSelector",
                                src=cms.InputTag("slimmedJets"),
                                cut=cms.string("pt>30"),
                                )

process.flashggPhotons.extraIsolations.extend([
        cms.PSet(
            algo=cms.string("FlashggStdIsolationAlgo"),
            name=cms.string("std03"),
            coneSize=cms.double(0.3), doOverlapRemoval=cms.bool(False),
            charged=cms.vdouble(0.02,0.02,0.1),
            photon=cms.vdouble(0.0, 0.070, 0.015, 0.0, 0.0, 0.0),
            neutral=cms.vdouble(0.0, 0.000, 0.000, 0.0, 0.0, 0.0)
            ),
        cms.PSet(
            algo=cms.string("DiphotonsFootPrintRemovedIsolationAlgo"),
            name=cms.string("fpr03"),
            coneSize=cms.double(0.3), doRandomCone=cms.bool(False), removePhotonsInMap=cms.int32(1),
            rechitLinkEnlargement=cms.double(0.25),
            charged=cms.vdouble(0.02,0.02,0.1),
            photon=cms.vdouble(0.0, 0.070, 0.015, 0.0, 0.0, 0.0),
            neutral=cms.vdouble(0.0, 0.000, 0.000, 0.0, 0.0, 0.0)
            ),
        cms.PSet(
            algo=cms.string("DiphotonsFootPrintRemovedIsolationAlgo"),
            name=cms.string("fprNoMap03"),
            coneSize=cms.double(0.3), doRandomCone=cms.bool(False), removePhotonsInMap=cms.int32(0),
            rechitLinkEnlargement=cms.double(0.25),
            photon=cms.vdouble(0.0, 0.070, 0.015, 0.0, 0.0, 0.0),
            ),
        cms.PSet(
            algo=cms.string("FlashggRandomConeIsolationAlgo"),
            name=cms.string("rnd03"),
            coneSize=cms.double(0.3), doOverlapRemoval=cms.bool(False),
            charged=cms.vdouble(0.02,0.02,0.1),
            photon=cms.vdouble(0.0, 0.070, 0.015, 0.0, 0.0, 0.0),
            neutral=cms.vdouble(0.0, 0.000, 0.000, 0.0, 0.0, 0.0),
            vetoCollections_=cms.VInputTag(cms.InputTag("vetoPhotons"),cms.InputTag("vetoJets")),
            veto=cms.double(0.8),
            ),
        cms.PSet(
            algo=cms.string("DiphotonsFootPrintRemovedIsolationAlgo"),
            name=cms.string("fprRnd03"),
            coneSize=cms.double(0.3), doRandomCone=cms.bool(True), removePhotonsInMap=cms.int32(1),
            rechitLinkEnlargement=cms.double(0.25),
            charged=cms.vdouble(0.02,0.02,0.1),
            photon=cms.vdouble(0.0, 0.070, 0.015, 0.0, 0.0, 0.0),
            vetoCollections_=cms.VInputTag(cms.InputTag("vetoPhotons"),cms.InputTag("vetoJets")),
            veto=cms.double(0.8),
            ),
        cms.PSet(
            algo=cms.string("DiphotonsFootPrintRemovedIsolationAlgo"),
            name=cms.string("fprRndNoMap03"),
            coneSize=cms.double(0.3), doRandomCone=cms.bool(True), removePhotonsInMap=cms.int32(0),
            rechitLinkEnlargement=cms.double(0.25),
            photon=cms.vdouble(0.0, 0.070, 0.015, 0.0, 0.0, 0.0),
            vetoCollections_=cms.VInputTag(cms.InputTag("vetoPhotons"),cms.InputTag("vetoJets")),
            veto=cms.double(0.8),
            ),
        ]
    )

### process.flashggPhotons.extraCaloIsolations.extend([
###         # photon and neutral iso w/ overlap removal
###         cms.PSet(overlapRemoval=cms.bool(True),
###                  type=cms.int32(4), # photon, use str to enum 
###                  vetoRegions=cms.vdouble(0.3,0.,0.,0.,0.,0.,0.)
###                  ),
###         cms.PSet(overlapRemoval=cms.bool(True),
###                  type=cms.int32(5), # neutral hadron, use str to enum 
###                  vetoRegions=cms.vdouble(0.3,0.1,0.1,0.,0.,0.,0.)
###                  ),
###         
###         # photon iso w/ veto cones
###         cms.PSet(overlapRemoval=cms.bool(False),
###                  type=cms.int32(4), # photon, use str to enum 
###                  vetoRegions=cms.vdouble(0.3,0.0,0.050,0.01,0.0,0.0,0.0)
###                  ),
###         cms.PSet(overlapRemoval=cms.bool(False),
###                  type=cms.int32(4), # photon, use str to enum 
###                  vetoRegions=cms.vdouble(0.3,0.0,0.060,0.012,0.0,0.0,0.0)
###                  ),
###         cms.PSet(overlapRemoval=cms.bool(False),
###                  type=cms.int32(4), # photon, use str to enum 
###                  vetoRegions=cms.vdouble(0.3,0.0,0.070,0.015,0.0,0.0,0.0)
###                  ),
###         cms.PSet(overlapRemoval=cms.bool(False),
###                  type=cms.int32(4), # photon, use str to enum 
###                  vetoRegions=cms.vdouble(0.3,0.0,0.080,0.02,0.0,0.0,0.0)
###                  ),
###         cms.PSet(overlapRemoval=cms.bool(True),
###                  type=cms.int32(4), # photon, use str to enum 
###                  vetoRegions=cms.vdouble(0.3,0.0,0.050,0.01,0.0,0.0,0.0)
###                  ),
### 
###                 
###         # netural iso rings
###          cms.PSet(overlapRemoval=cms.bool(True),
###                  type=cms.int32(5), # neutral hadron, use str to enum 
###                  vetoRegions=cms.vdouble(0.1,0.,0.,0.,0.,0.,0.)
###                  ),
###         cms.PSet(overlapRemoval=cms.bool(True),
###                  type=cms.int32(5), # neutral hadron, use str to enum 
###                  vetoRegions=cms.vdouble(0.12,0.1,0.1,0.,0.,0.,0.)
###                  ),
###         cms.PSet(overlapRemoval=cms.bool(True),
###                  type=cms.int32(5), # neutral hadron, use str to enum 
###                  vetoRegions=cms.vdouble(0.15,0.12,0.12,0.,0.,0.,0.)
###                  ),
###         cms.PSet(overlapRemoval=cms.bool(True),
###                  type=cms.int32(5), # neutral hadron, use str to enum 
###                  vetoRegions=cms.vdouble(0.2,0.15,0.15,0.,0.,0.,0.)
###                  ),
###         cms.PSet(overlapRemoval=cms.bool(True),
###                  type=cms.int32(5), # neutral hadron, use str to enum 
###                  vetoRegions=cms.vdouble(0.3,0.2,0.2,0.,0.,0.,0.)
###                  ),
###         
###         cms.PSet(overlapRemoval=cms.bool(False),
###                  type=cms.int32(5), # neutral hadron, use str to enum 
###                  vetoRegions=cms.vdouble(0.1,0.,0.,0.,0.,0.,0.)
###                  ),
###         cms.PSet(overlapRemoval=cms.bool(False),
###                  type=cms.int32(5), # neutral hadron, use str to enum 
###                  vetoRegions=cms.vdouble(0.12,0.1,0.1,0.,0.,0.,0.)
###                  ),
###         cms.PSet(overlapRemoval=cms.bool(False),
###                  type=cms.int32(5), # neutral hadron, use str to enum 
###                  vetoRegions=cms.vdouble(0.15,0.12,0.12,0.,0.,0.,0.)
###                  ),
###         cms.PSet(overlapRemoval=cms.bool(False),
###                  type=cms.int32(5), # neutral hadron, use str to enum 
###                  vetoRegions=cms.vdouble(0.2,0.15,0.15,0.,0.,0.,0.)
###                  ),
###         cms.PSet(overlapRemoval=cms.bool(False),
###                  type=cms.int32(5), # neutral hadron, use str to enum 
###                  vetoRegions=cms.vdouble(0.3,0.2,0.2,0.,0.,0.,0.)
###                  ),
### 
###         cms.PSet(overlapRemoval=cms.bool(True),
###                  type=cms.int32(4), # photon, use str to enum 
###                  vetoRegions=cms.vdouble(0.070,0.050,0.050,0.0,0.0,0.0,0.0)
###                  ),
###         cms.PSet(overlapRemoval=cms.bool(True),
###                  type=cms.int32(4), # photon, use str to enum 
###                  vetoRegions=cms.vdouble(0.080,0.070,0.070,0.0,0.0,0.0,0.0)
###                  ),
###         cms.PSet(overlapRemoval=cms.bool(True),
###                  type=cms.int32(4), # photon, use str to enum 
###                  vetoRegions=cms.vdouble(0.1,0.080,0.080,0.0,0.0,0.0,0.0)
###                  ),
###         cms.PSet(overlapRemoval=cms.bool(True),
###                  type=cms.int32(4), # photon, use str to enum 
###                  vetoRegions=cms.vdouble(0.15,0.1,0.1,0.0,0.0,0.0,0.0)
###                  ),
###         cms.PSet(overlapRemoval=cms.bool(True),
###                  type=cms.int32(4), # photon, use str to enum 
###                  vetoRegions=cms.vdouble(0.2,0.15,0.15,0.0,0.0,0.0,0.0)
###                  ),
###         cms.PSet(overlapRemoval=cms.bool(True),
###                  type=cms.int32(4), # photon, use str to enum 
###                  vetoRegions=cms.vdouble(0.3,0.2,0.2,0.0,0.0,0.0,0.0)
###                  ),
###         cms.PSet(overlapRemoval=cms.bool(False),
###                  type=cms.int32(4), # photon, use str to enum 
###                  vetoRegions=cms.vdouble(0.070,0.050,0.050,0.0,0.0,0.0,0.0)
###                  ),
###         cms.PSet(overlapRemoval=cms.bool(False),
###                  type=cms.int32(4), # photon, use str to enum 
###                  vetoRegions=cms.vdouble(0.080,0.070,0.070,0.0,0.0,0.0,0.0)
###                  ),
###         cms.PSet(overlapRemoval=cms.bool(False),
###                  type=cms.int32(4), # photon, use str to enum 
###                  vetoRegions=cms.vdouble(0.1,0.080,0.080,0.0,0.0,0.0,0.0)
###                  ),
###         cms.PSet(overlapRemoval=cms.bool(False),
###                  type=cms.int32(4), # photon, use str to enum 
###                  vetoRegions=cms.vdouble(0.15,0.1,0.1,0.0,0.0,0.0,0.0)
###                  ),
###         cms.PSet(overlapRemoval=cms.bool(False),
###                  type=cms.int32(4), # photon, use str to enum 
###                  vetoRegions=cms.vdouble(0.2,0.15,0.15,0.0,0.0,0.0,0.0)
###                  ),
###         cms.PSet(overlapRemoval=cms.bool(False),
###                  type=cms.int32(4), # photon, use str to enum 
###                  vetoRegions=cms.vdouble(0.3,0.2,0.2,0.0,0.0,0.0,0.0)
###                  ),
### 
###         ]
###                                                  )

from flashgg.MicroAOD.flashggMicroAODOutputCommands_cff import microAODDefaultOutputCommand, microAODDebugOutputCommand
 
process.out = cms.OutputModule("PoolOutputModule", fileName = cms.untracked.string('myOutputFile_relval.root'),
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
                      +process.flashggVertexMapUnique+process.flashggVertexMapNonUnique## +process.flashggElectrons
                      +process.flashggMicroAODGenSequence
                      )
                     *(process.flashggPhotons)*(process.vetoPhotons+process.vetoJets)
                     *process.flashggDiPhotons
                     )

process.e = cms.EndPath(process.out)

from flashgg.MicroAOD.MicroAODCustomize import customize
customize(process)
process.prune()

