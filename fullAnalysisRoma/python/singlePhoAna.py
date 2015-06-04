import FWCore.ParameterSet.Config as cms
import FWCore.Utilities.FileUtils as FileUtils

process = cms.Process("singlePhoAna")

process.load("FWCore.MessageService.MessageLogger_cfi")

process.load("Configuration.StandardSequences.GeometryDB_cff")
process.load("Configuration.StandardSequences.MagneticField_cff")
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
#process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff")
process.GlobalTag.globaltag = 'POSTLS170_V5::All'

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32( 100 ) )
process.MessageLogger.cerr.FwkReport.reportEvery = cms.untracked.int32( 100 )
process.MessageLogger.cerr.threshold = 'ERROR' # can't get suppressWarning to work: disable all warnings for now

process.source = cms.Source("PoolSource",
                            fileNames=cms.untracked.vstring(

        # G+jets, production v4-5
        #"/store/group/phys_higgs/cmshgg/musella/flashgg/ExoPhys14_v5/diphotonsPhys14V2/GJets_HT-600toInf_Tune4C_13TeV-madgraph-tauola/ExoPhys14_v5-diphotonsPhys14V2-v0-Phys14DR-PU20bx25_PHYS14_25_V1-v1/150209_144839/0000/myOutputFile_1.root"

        # RS graviton, kMpl001, MG=1500, production v4-5       
        #"/store/group/phys_higgs/cmshgg/musella/flashgg/ExoPhys14_v4/diphotonsPhys14V2/RSGravToGG_kMpl-001_M-1500_Tune4C_13TeV-pythia8/ExoPhys14_v4-diphotonsPhys14V2-v0-Phys14DR-PU20bx25_PHYS14_25_V1-v1/150204_005812/0000/myOutputFile_1.root"

        # Phys14, RSGrav, kMpl001, mG=5k, production v2
        #"/store/group/phys_higgs/cmshgg/musella/flashgg/ExoPhys14_v2/diphotonsPhys14V2/RSGravToGG_kMpl001_M_5000_Tune4C_13TeV_pythia8/ExoPhys14_v2-diphotonsPhys14V2-v0-Phys14DR-PU20bx25_PHYS14_25_V1-v1/150128_133931/0000/myOutputFile_1.root"

        # Phys14, RSGrav, kMpl02, mG=5k, production v2  
        #"/store/group/phys_higgs/cmshgg/musella/flashgg/ExoPhys14_v2/diphotonsPhys14V2/RSGravToGG_kMpl-02_M-5000_Tune4C_13TeV-pythia8/ExoPhys14_v2-diphotonsPhys14V2-v0-Phys14DR-PU20bx25_PHYS14_25_V1-v1/150128_133856/0000/myOutputFile_1.root"

        # Phys14, RSGrav, kMpl01, mG=5k, production v1
        #"/store/group/phys_higgs/cmshgg/flashgg/ExoPhys14/diphotonsPhys14V1/RSGravToGG_kMpl01_M-5000_Tune4C_13TeV-pythia8/ExoPhys14-diphotonsPhys14V1-v0-Phys14DR-PU40bx25_PHYS14_25_V1-v1/141205_002243/0000/myOutputFile_1.root"

        # Phys14, RSGrav, kMpl001, mG=5k, production v1
        #"/store/group/phys_higgs/cmshgg/flashgg/ExoPhys14/diphotonsPhys14V1/RSGravToGG_kMpl001_M_5000_Tune4C_13TeV_pythia8/ExoPhys14-diphotonsPhys14V1-v0-Phys14DR-PU20bx25_PHYS14_25_V1-v1/141205_002242/0000/myOutputFile_1.root"

        # Phys14, RSGrav, kMpl02, mG=5k, production v1    
        #"/store/group/phys_higgs/cmshgg/flashgg/ExoPhys14/diphotonsPhys14V1/RSGravToGG_kMpl-02_M-5000_Tune4C_13TeV-pythia8/ExoPhys14-diphotonsPhys14V1-v0-Phys14DR-PU20bx25_PHYS14_25_V1-v1/141205_002206/0000/myOutputFile_1.root"

        # Phys14, RSGrav, kMpl02, mG=3k, production v1   
        #"/store/group/phys_higgs/cmshgg/flashgg/ExoPhys14/diphotonsPhys14V1/RSGravToGG_kMpl-02_M-3000_Tune4C_13TeV-pythia8/ExoPhys14-diphotonsPhys14V1-v0-Phys14DR-PU20bx25_PHYS14_25_V1-v1/141205_002242/0000/myOutputFile_1.root"

        # Phys14, RSGrav, kMpl01, mG=3k, production v1 
        #"/store/group/phys_higgs/cmshgg/flashgg/ExoPhys14/diphotons-phys14-v1/RSGravToGG_kMpl-01_M-3000_Tune4C_13TeV-pythia8/ExoPhys14-diphotons-phys14-v1-v0-Phys14DR-PU30bx50_PHYS14_25_V1-v1/141202_121354/0000/myOutputFile_1.root",
        #"/store/group/phys_higgs/cmshgg/flashgg/ExoPhys14/diphotons-phys14-v1/RSGravToGG_kMpl-01_M-3000_Tune4C_13TeV-pythia8/ExoPhys14-diphotons-phys14-v1-v0-Phys14DR-PU30bx50_PHYS14_25_V1-v1/141202_121354/0000/myOutputFile_2.root",
        #"/store/group/phys_higgs/cmshgg/flashgg/ExoPhys14/diphotons-phys14-v1/RSGravToGG_kMpl-01_M-3000_Tune4C_13TeV-pythia8/ExoPhys14-diphotons-phys14-v1-v0-Phys14DR-PU30bx50_PHYS14_25_V1-v1/141202_121354/0000/myOutputFile_3.root" 

        # Phys14, RSGrav, kMpl01, mG=1.5k, production v1 
        #"/store/group/phys_higgs/cmshgg/flashgg/ExoPhys14/diphotons-phys14-v1/RSGravToGG_kMpl-01_M-1500_Tune4C_13TeV-pythia8/ExoPhys14-diphotons-phys14-v1-v0-Phys14DR-PU30bx50_PHYS14_25_V1-v1/141202_121328/0000/myOutputFile_1.root", 
        #"/store/group/phys_higgs/cmshgg/flashgg/ExoPhys14/diphotons-phys14-v1/RSGravToGG_kMpl-01_M-1500_Tune4C_13TeV-pythia8/ExoPhys14-diphotons-phys14-v1-v0-Phys14DR-PU30bx50_PHYS14_25_V1-v1/141202_121328/0000/myOutputFile_2.root", 
        #"/store/group/phys_higgs/cmshgg/flashgg/ExoPhys14/diphotons-phys14-v1/RSGravToGG_kMpl-01_M-1500_Tune4C_13TeV-pythia8/ExoPhys14-diphotons-phys14-v1-v0-Phys14DR-PU30bx50_PHYS14_25_V1-v1/141202_121328/0000/myOutputFile_3.root"

        # Phys14, RSGrav, kMpl001, mG=1.5k, production v1     
        #"/store/group/phys_higgs/cmshgg/flashgg/ExoPhys14/diphotons-phys14-v1/RSGravToGG_kMpl-001_M-1500_Tune4C_13TeV-pythia8/ExoPhys14-diphotons-phys14-v1-v0-Phys14DR-PU20bx25_PHYS14_25_V1-v1/141202_121235/0000/myOutputFile_1.root",
        #"/store/group/phys_higgs/cmshgg/flashgg/ExoPhys14/diphotons-phys14-v1/RSGravToGG_kMpl-001_M-1500_Tune4C_13TeV-pythia8/ExoPhys14-diphotons-phys14-v1-v0-Phys14DR-PU20bx25_PHYS14_25_V1-v1/141202_121235/0000/myOutputFile_2.root",
        #"/store/group/phys_higgs/cmshgg/flashgg/ExoPhys14/diphotons-phys14-v1/RSGravToGG_kMpl-001_M-1500_Tune4C_13TeV-pythia8/ExoPhys14-diphotons-phys14-v1-v0-Phys14DR-PU20bx25_PHYS14_25_V1-v1/141202_121235/0000/myOutputFile_3.root"

        # Phys14, GG+jets, AnV1                                                                                             
        "/store/group/phys_higgs/cmshgg/musella/flashgg/ExoPhys14ANv1/diphotonsPhys14AnV1/GGJets_M-500To1000_Pt-50_13TeV-sherpa/ExoPhys14ANv1-diphotonsPhys14AnV1-v0-Phys14DR-PU20bx25_PHYS14_25_V1-v1/150330_141301/0000/diphotonsMicroAOD_1.root"
        )
                            )

process.load("flashgg/MicroAOD/flashggPhotons_cfi")

process.eventCount = cms.EDProducer("EventCountProducer")

from flashgg.MicroAOD.flashggMicroAODOutputCommands_cff import microAODDefaultOutputCommand

#process.out = cms.OutputModule("PoolOutputModule", fileName = cms.untracked.string('myOutputFile.root'),
#                               outputCommands = microAODDefaultOutputCommand
#                               )
#process.out.outputCommands.append("keep *_*_*_*")


process.TFileService = cms.Service("TFileService",fileName = cms.string("singlePhotonTree.root"))
process.singlePhoAna = cms.EDAnalyzer('SinglePhoAnalyzer',
                                      packedGenParticles = cms.untracked.InputTag('flashggGenPhotons'),
                                      reducedBarrelRecHitCollection = cms.InputTag('reducedEgamma','reducedEBRecHits'),
                                      reducedEndcapRecHitCollection = cms.InputTag('reducedEgamma','reducedEERecHits')
                                      )

process.p = cms.Path(process.singlePhoAna)

#process.e = cms.EndPath(process.out)
