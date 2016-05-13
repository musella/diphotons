import FWCore.ParameterSet.Config as cms
import FWCore.Utilities.FileUtils as FileUtils

process = cms.Process("FLASHggMicroAOD")

process.load("FWCore.MessageService.MessageLogger_cfi")

process.load("Configuration.StandardSequences.GeometryDB_cff")
process.load("Configuration.StandardSequences.MagneticField_cff")
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff")
## process.GlobalTag.globaltag = 'POSTLS170_V5::All'
from Configuration.AlCa.GlobalTag import GlobalTag
## process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:run2_mc')
import os
if os.environ["CMSSW_VERSION"].count("CMSSW_7_6"):
    process.GlobalTag = GlobalTag(process.GlobalTag, '76X_mcRun2_asymptotic_v13')
elif os.environ["CMSSW_VERSION"].count("CMSSW_8_0"):
    process.GlobalTag = GlobalTag(process.GlobalTag,'80X_mcRun2_asymptotic_v11')
else:
    raise Exception,"The default setup for microAODstd.py does not support releases other than 76X and 80X"

process.RandomNumberGeneratorService = cms.Service("RandomNumberGeneratorService")
process.RandomNumberGeneratorService.flashggRandomizedPhotons = cms.PSet(
          initialSeed = cms.untracked.uint32(16253245)
        )

process.options=cms.untracked.PSet(
    makeTriggerResults = cms.untracked.bool(True),
    wantSummary = cms.untracked.bool(True)
    )

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32( 100000 ) )
process.MessageLogger.cerr.FwkReport.reportEvery = 100

process.source = cms.Source("PoolSource",
                            fileNames=cms.untracked.vstring(
        '/store/mc/RunIISpring16MiniAODv1/RSGravToGG_kMpl-001_M-745_TuneCUEP8M1_13TeV-pythia8/MINIAODSIM/PUSpring16RAWAODSIM_80X_mcRun2_asymptotic_2016_v3-v1/60000/14E03F30-4E10-E611-BA1A-0090FAA57330.root'
        # '/store/data/Run2015D/DoubleEG/MINIAOD/05Oct2015-v1/50000/0014E86F-656F-E511-9D3F-002618943831.root'
        )
                            )

process.load("flashgg/MicroAOD/flashggMicroAODSequence_cff")
process.flashggPhotons.copyExtraGenInfo = True
process.flashggPhotons.recomputeNonZsClusterShapes=False

process.flashggPrunedGenParticles.select.extend(["keep status = 3",
                                                 "keep status = 22",
                                                 "keep status = 23",
                                                 "++keep pdgId = 22",
                                                 "keep++ pdgId = 39",
                                                 "keep++ pdgId = 5000039",
                                                 ])

process.flashggGenPhotonsExtra.promptMothers=cms.vint32(39,5000039,5100039)

# for isolation cones
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
             algo=cms.string("FlashggRandomConeIsolationAlgo"),
             name=cms.string("rnd03"),
             coneSize=cms.double(0.3), doOverlapRemoval=cms.bool(False),
             charged=cms.vdouble(0.02,0.02,0.1),
             photon=cms.vdouble(0.0, 0.070, 0.015, 0.0, 0.0, 0.0),
             vetoCollections_=cms.VInputTag(cms.InputTag("vetoPhotons"),cms.InputTag("vetoJets")),
             veto=cms.double(0.699),
             ),
          cms.PSet(
              algo=cms.string("DiphotonsFootPrintRemovedIsolationAlgo"),
              name=cms.string("fprNoMap03"),
              coneSize=cms.double(0.3), doRandomCone=cms.bool(False), removePhotonsInMap=cms.int32(0),
              rechitLinkEnlargement=cms.double(0.25),
              photon=cms.vdouble(0.0, 0.070, 0.015, 0.0, 0.0, 0.0),
              charged=cms.vdouble(0.02,0.02,0.1),
              ),
         ]
     )


for icone,dphi in enumerate( [0.7,1.3,1.9,2.5,3.1,-2.5,-1.9,-1.3,-0.7] ):
    process.flashggPhotons.extraIsolations.append(
        cms.PSet(
            algo=cms.string("FlashggRandomConeIsolationAlgo"),
            name=cms.string("rnd03_%d" % icone), deltaPhi=cms.double(dphi),
            coneSize=cms.double(0.3), doOverlapRemoval=cms.bool(False),
            charged=cms.vdouble(0.02,0.02,0.1),
            photon=cms.vdouble(0.0, 0.070, 0.015, 0.0, 0.0, 0.0),
            vetoCollections_=cms.VInputTag(cms.InputTag("vetoPhotons"),cms.InputTag("vetoJets")),
            veto=cms.double(0.699),
            ),
        )

from flashgg.MicroAOD.flashggMicroAODOutputCommands_cff import microAODDefaultOutputCommand, microAODDebugOutputCommand, microAODHLTOutputCommand
 
process.out = cms.OutputModule("PoolOutputModule", 
                               fileName = cms.untracked.string('diphotonsMicroAOD.root'),
                               outputCommands = microAODDefaultOutputCommand,
                               SelectEvents = cms.untracked.PSet(SelectEvents = cms.vstring('p')),
                               )

### process.out.outputCommands.extend( microAODDebugOutputCommand )
process.out.outputCommands.extend([### "keep *_eventCount_*_*",
                                    ### "keep *_weightsCount_*_*",
                                    ### "keep *_prunedGenParticles_*_*",
                                    ### "keep *_packedGenParticles_*_*",
                                    ### "keep *_reducedEgamma_reduced*RecHits_*",
                                    "keep *_flashggDiPhotons_*_*",
                                    "keep *_selectedPatTrigger_*_*",
                                    "drop *_flashggFinalEGamma_*_*",
                                    "drop *_flashggPreselectedDiPhotons_*_*",
                                    "keep *_reducedEgamma_*_*",
                                    "drop *_reducedEgamma_reduced*RecHits_*",
                                    "keep *_flashggElectrons_*_*",
                                    "drop *_flashggSelectedElectrons_*_*",
                                    "keep *_BeamHaloSummary_*_*",
                                    "keep *_slimmedAddPileupInfo_*_*"
                                    ]
                                   )
process.out.outputCommands.extend(microAODHLTOutputCommand)

process.myPreselectedPhotons = cms.EDFilter("FlashggPhotonSelector",
                                            src=cms.InputTag("flashggPhotons"),
                                            cut=cms.string("(r9>0.8||egChargedHadronIso<20||egChargedHadronIso/pt<0.3) && pt>50 && egChargedHadronIso<15")
                                            )

process.myPreselectedElectrons = cms.EDFilter("ElectronSelector",
                                              src=cms.InputTag("flashggElectrons"),
                                              cut=cms.string("pt>20")
                                              )


# need to allow unscheduled processes otherwise reclustering function will fail
# this is because of the jet clustering tool, and we have to live with it for now.
process.options = cms.untracked.PSet(
    allowUnscheduled = cms.untracked.bool(True)
    )
### # import function which takes care of reclustering the jets using legacy vertex		
### from flashgg.MicroAOD.flashggJets_cfi import addFlashggPFCHSLegJets 
### # call the function, it takes care of everything else.
### addFlashggPFCHSLegJets(process)

process.p = cms.Path(process.flashggMicroAODSequence+process.myPreselectedPhotons+process.myPreselectedElectrons)

process.e = cms.EndPath(process.out)

process.e2 = cms.EndPath(process.eventCount+process.weightsCount)

from diphotons.Analysis.MicroAODCustomize import customize
customize(process)
## process.prune()

