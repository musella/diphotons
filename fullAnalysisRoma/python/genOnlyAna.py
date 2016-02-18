import FWCore.ParameterSet.Config as cms
import FWCore.Utilities.FileUtils as FileUtils

process = cms.Process("genOnlyAna")

process.load("FWCore.MessageService.MessageLogger_cfi")

#process.load("Configuration.StandardSequences.GeometryDB_cff")
#process.load("Configuration.StandardSequences.MagneticField_cff")
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff")
process.GlobalTag.globaltag = 'MCRUN2_71_V1'

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32( -1 ) )
process.MessageLogger.cerr.FwkReport.reportEvery = cms.untracked.int32( 100 )

process.source = cms.Source("PoolSource",
                            fileNames=cms.untracked.vstring(

        # RunIIWinter - gen-sim only
        #"/store/mc/RunIIWinter15GS/RSGravToGG_kMpl-02_M-750_TuneCUEP8M1_13TeV-pythia8/GEN-SIM/MCRUN2_71_V1-v1/60000/FCA23148-0AE7-E411-8303-02163E011455.root"
        #
        #"/store/mc/RunIIWinter15GS/RSGravToGG_kMpl-02_M-500_TuneCUEP8M1_13TeV-pythia8/GEN-SIM/MCRUN2_71_V1-v1/80000/30BF8F18-FFE6-E411-9C10-02163E012D9C.root",
        #"/store/mc/RunIIWinter15GS/RSGravToGG_kMpl-02_M-500_TuneCUEP8M1_13TeV-pythia8/GEN-SIM/MCRUN2_71_V1-v1/80000/5A6CBDAC-CFE6-E411-9475-003048FE66DC.root",
        #"/store/mc/RunIIWinter15GS/RSGravToGG_kMpl-02_M-500_TuneCUEP8M1_13TeV-pythia8/GEN-SIM/MCRUN2_71_V1-v1/80000/5E8FFFB4-D6E6-E411-97F7-02163E013C73.root",
        #"/store/mc/RunIIWinter15GS/RSGravToGG_kMpl-02_M-500_TuneCUEP8M1_13TeV-pythia8/GEN-SIM/MCRUN2_71_V1-v1/80000/AC23DE7B-C7E6-E411-ACFD-02163E013E4C.root",
        #"/store/mc/RunIIWinter15GS/RSGravToGG_kMpl-02_M-500_TuneCUEP8M1_13TeV-pythia8/GEN-SIM/MCRUN2_71_V1-v1/80000/F015ABA6-CBE6-E411-A1EA-02163E00F6D6.root"
        #
        #"/store/mc/RunIIWinter15GS/RSGravToGG_kMpl-02_M-1000_TuneCUEP8M1_13TeV-pythia8/GEN-SIM/MCRUN2_71_V1-v1/70000/2E13FE03-99E7-E411-93B9-02163E00EAB5.root"
        #
        #"/store/mc/RunIIWinter15GS/RSGravToGG_kMpl-02_M-1500_TuneCUEP8M1_13TeV-pythia8/GEN-SIM/MCRUN2_71_V1-v1/80000/1828A3CF-CAE6-E411-8C13-002590495074.root",
        #"/store/mc/RunIIWinter15GS/RSGravToGG_kMpl-02_M-1500_TuneCUEP8M1_13TeV-pythia8/GEN-SIM/MCRUN2_71_V1-v1/80000/30CD3EA4-D4E6-E411-B4F1-02163E00CA8B.root",
        #"/store/mc/RunIIWinter15GS/RSGravToGG_kMpl-02_M-1500_TuneCUEP8M1_13TeV-pythia8/GEN-SIM/MCRUN2_71_V1-v1/80000/7AFF90AD-9CE7-E411-A2B4-02163E00EB64.root",
        #"/store/mc/RunIIWinter15GS/RSGravToGG_kMpl-02_M-1500_TuneCUEP8M1_13TeV-pythia8/GEN-SIM/MCRUN2_71_V1-v1/80000/BC51DA62-DDE6-E411-A01A-002590494D2E.root"
        #
        #"/store/mc/RunIIWinter15GS/RSGravToGG_kMpl-02_M-3000_TuneCUEP8M1_13TeV-pythia8/GEN-SIM/MCRUN2_71_V1-v1/60000/0065C39D-7FE7-E411-96F0-02163E00EA90.root"
        #
        #"/store/mc/RunIIWinter15GS/RSGravToGG_kMpl-02_M-4000_TuneCUEP8M1_13TeV-pythia8/GEN-SIM/MCRUN2_71_V1-v1/80000/1612DA54-C7E6-E411-89D3-0025904B5B28.root",
        #"/store/mc/RunIIWinter15GS/RSGravToGG_kMpl-02_M-4000_TuneCUEP8M1_13TeV-pythia8/GEN-SIM/MCRUN2_71_V1-v1/80000/30A199D1-CDE6-E411-AD4C-0025904B5B2C.root",
        #"/store/mc/RunIIWinter15GS/RSGravToGG_kMpl-02_M-4000_TuneCUEP8M1_13TeV-pythia8/GEN-SIM/MCRUN2_71_V1-v1/80000/86CB6BDE-45E7-E411-BB96-02163E00B6FE.root",
        #"/store/mc/RunIIWinter15GS/RSGravToGG_kMpl-02_M-4000_TuneCUEP8M1_13TeV-pythia8/GEN-SIM/MCRUN2_71_V1-v1/80000/C601FC6A-D1E6-E411-9298-02163E013C86.root",
        #"/store/mc/RunIIWinter15GS/RSGravToGG_kMpl-02_M-4000_TuneCUEP8M1_13TeV-pythia8/GEN-SIM/MCRUN2_71_V1-v1/80000/E80843B9-DBE6-E411-B152-002590494D2E.root",
        #
        #"/store/mc/RunIIWinter15GS/RSGravToGG_kMpl-02_M-5000_TuneCUEP8M1_13TeV-pythia8/GEN-SIM/MCRUN2_71_V1-v1/80000/8E6D392A-E7E6-E411-A122-02163E00C762.root"
        #"/store/mc/RunIIWinter15GS/RSGravToGG_kMpl-02_M-5000_TuneCUEP8M1_13TeV-pythia8/GEN-SIM/MCRUN2_71_V1-v1/80000/DE90E549-16E7-E411-B71F-02163E00E927.root",

        "file:/tmp/crovelli/RSGravitonToGammaGamma_2750_k02.root" 
        )
                            )

process.TFileService = cms.Service("TFileService",fileName = cms.string("genOnlyPhotons.root"))

process.genOnlyAna = cms.EDAnalyzer('GenOnlyAnalyzer')

process.p = cms.Path(process.genOnlyAna)

