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
process.GlobalTag = GlobalTag(process.GlobalTag, 'MCRUN2_74_V9', '')


process.options=cms.untracked.PSet(
    makeTriggerResults = cms.untracked.bool(True),
    wantSummary = cms.untracked.bool(True)
    )

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32( 100000 ) )
process.MessageLogger.cerr.FwkReport.reportEvery = 100
### process.MessageLogger.cerr.threshold = 'ERROR' # can't get suppressWarning to work: disable all warnings for now
### # process.MessageLogger.suppressWarning.extend(['SimpleMemoryCheck','MemoryCheck']) # this would have been better...
### 
### # Uncomment the following if you notice you have a memory leak
### # This is a lightweight tool to digg further
### process.SimpleMemoryCheck = cms.Service("SimpleMemoryCheck",
###                                         ignoreTotal = cms.untracked.int32(1),
###                                         monitorPssAndPrivate = cms.untracked.bool(True)
###                                         )

process.source = cms.Source("PoolSource",
                            fileNames=cms.untracked.vstring(
        ## "/store/mc/Phys14DR/RSGravToGG_kMpl-001_M-1500_Tune4C_13TeV-pythia8/MINIAODSIM/PU20bx25_PHYS14_25_V1-v1/10000/065B2E1D-1468-E411-946C-002590200A7C.root"
        ## "/store/relval/CMSSW_7_2_0/RelValTTbar_13/MINIAODSIM/PU25ns_PHYS14_25_V1_Phys14-v2/00000/603403B7-9E59-E411-B5E0-0026189438B3.root"
        ## "file:/tmp/musella/1830E403-9F59-E411-9BAC-0025905A48BC.root"
        ## "/store/relval/CMSSW_7_2_0/RelValH130GGgluonfusion_13/MINIAODSIM/PU25ns_PHYS14_25_V1_Phys14-v2/00000/1830E403-9F59-E411-9BAC-0025905A48BC.root"
        ## "/store/mc/Phys14DR/GJets_HT-100to200_Tune4C_13TeV-madgraph-tauola/MINIAODSIM/PU20bx25_PHYS14_25_V1-v1/00000/00D67F78-2873-E411-B3BB-0025907DC9C0.root",
        ## "/store/mc/Phys14DR/GGJets_M-1000To2000_Pt-50_13TeV-sherpa/MINIAODSIM/PU20bx25_PHYS14_25_V1-v1/10000/6844B8AF-D06B-E411-9CE0-002590D0B040.root"
        
        ## "/store/mc/Phys14DR/QCD_HT-100To250_13TeV-madgraph/MINIAODSIM/PU20bx25_PHYS14_25_V1-v1/00000/0441A526-7C87-E411-BE19-002590574604.root")
        ## "/store/data/Run2015B/DoubleEG/MINIAOD/PromptReco-v1/000/251/096/00000/8A2D533C-5626-E511-AF3C-02163E011FAB.root")
        ## "/store/mc/RunIISpring15DR74/GGJets_M-1000To2000_Pt-50_13TeV-sherpa/MINIAODSIM/Asympt25ns_MCRUN2_74_V9-v3/40000/14933A04-1A3C-E511-979C-AC162DABCAF8.root"
        ## "/store/data/Run2015B/DoubleEG/MINIAOD/PromptReco-v1/000/251/244/00000/C47A9CF9-6227-E511-908E-02163E014509.root"
        ## "/store/data/Run2015D/DoubleEG/MINIAOD/PromptReco-v3/000/256/630/00000/827EBF7D-5D5F-E511-A2A8-02163E01454A.root"
        ## "root://eoscms//eos/cms/store/mc/RunIISpring15DR74/ADDGravToGG_MS-6000_NED-4_KK-1_M-2000To4000_13TeV-sherpa/MINIAODSIM/Asympt25ns_MCRUN2_74_V9-v1/20000/76CA81A9-1024-E511-8D9F-3417EBE6471D.root"
        "/store/mc/RunIISpring15DR74/RSGravToGG_kMpl-001_M-6000_TuneCUEP8M1_13TeV-pythia8/MINIAODSIM/Asympt50ns_MCRUN2_74_V9A-v1/30000/3A9530E2-292B-E511-9F05-003048FFCB8C.root"
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

process.flashggPreselectedDiPhotons.cut = "(leadingPhoton.r9>0.8||leadingPhoton.egChargedHadronIso<20||leadingPhoton.egChargedHadronIso/leadingPhoton.pt<0.3) && (subLeadingPhoton.r9>0.8||subLeadingPhoton.egChargedHadronIso<20||subLeadingPhoton.egChargedHadronIso/subLeadingPhoton.pt<0.3)"

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
###          cms.PSet(
###              algo=cms.string("DiphotonsFootPrintRemovedIsolationAlgo"),
###              name=cms.string("fpr03"),
###              coneSize=cms.double(0.3), doRandomCone=cms.bool(False), removePhotonsInMap=cms.int32(1),
###              rechitLinkEnlargement=cms.double(0.25),
###              charged=cms.vdouble(0.02,0.02,0.1),
###              photon=cms.vdouble(0.0, 0.070, 0.015, 0.0, 0.0, 0.0),
###              ),
          cms.PSet(
              algo=cms.string("DiphotonsFootPrintRemovedIsolationAlgo"),
              name=cms.string("fprNoMap03"),
              coneSize=cms.double(0.3), doRandomCone=cms.bool(False), removePhotonsInMap=cms.int32(0),
              rechitLinkEnlargement=cms.double(0.25),
              photon=cms.vdouble(0.0, 0.070, 0.015, 0.0, 0.0, 0.0),
              charged=cms.vdouble(0.02,0.02,0.1),
              ),
         #### cms.PSet(
         ####     algo=cms.string("DiphotonsFootPrintRemovedIsolationAlgo"),
         ####     name=cms.string("fprRnd03"),
         ####     coneSize=cms.double(0.3), doRandomCone=cms.bool(True), removePhotonsInMap=cms.int32(1),
         ####     rechitLinkEnlargement=cms.double(0.25),
         ####     charged=cms.vdouble(0.02,0.02,0.1),
         ####     vetoCollections_=cms.VInputTag(cms.InputTag("vetoPhotons"),cms.InputTag("vetoJets")),
         ####     veto=cms.double(0.699),
         ####     ),
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
            ## maxVtx=cms.int32(1), computeWorstVtx=cms.bool(False)
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
                                    "keep *_flashggElectrons_*_*",
                                    "keep *_BeamHaloSummary_*_*"
                                    ]
                                   )
process.out.outputCommands.extend(microAODHLTOutputCommand)

process.myPreselectedPhotons = cms.EDFilter("PhotonSelector",
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

