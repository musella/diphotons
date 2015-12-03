#!/usr/bin/env cmsRun

import FWCore.ParameterSet.Config as cms
import FWCore.Utilities.FileUtils as FileUtils

process = cms.Process("Analysis")

#
# general stuff
#
process.load("FWCore.MessageService.MessageLogger_cfi")

process.load("PhysicsTools.PatAlgos.slimming.genParticles_cff")
## process.load("PhysicsTools.PatAlgos.slimming.packedGenParticles_cfi")
## process.load("PhysicsTools.PatAlgos.slimming.offlineSlimmedPrimaryVertices_cfi")
process.offlineSlimmedPrimaryVertices = cms.EDProducer("DummyVertexProducer")

process.load("flashgg.MicroAOD.flashggGenPhotons_cfi")
process.load("flashgg.MicroAOD.flashggGenPhotonsExtra_cfi")
process.load("flashgg.MicroAOD.flashggGenDiPhotons_cfi")

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )
process.MessageLogger.cerr.FwkReport.reportEvery = cms.untracked.int32( 1000 )


#
# load job options
#
from diphotons.MetaData.JobConfig import customize
customize.setDefault("maxEvents",10000)
customize.setDefault("targetLumi",1.e+3)

import FWCore.ParameterSet.VarParsing as VarParsing
customize.options.register ('massCut',
                            200, # default value
                            VarParsing.VarParsing.multiplicity.singleton, # singleton or list
                            VarParsing.VarParsing.varType.float,          # string, int, or float
                            "massCut")
customize.options.register ('ptLead',
                            100, # default value
                            VarParsing.VarParsing.multiplicity.singleton, # singleton or list
                            VarParsing.VarParsing.varType.float,          # string, int, or float
                            "ptLead")
customize.options.register ('ptSublead',
                            100, # default value
                            VarParsing.VarParsing.multiplicity.singleton, # singleton or list
                            VarParsing.VarParsing.varType.float,          # string, int, or float
                            "ptSublead")
customize.options.register ('scaling',
                            False, # default value
                            VarParsing.VarParsing.multiplicity.singleton, # singleton or list
                            VarParsing.VarParsing.varType.bool,          # string, int, or float
                            "scaling")

customize.parse()

#
# input and output
#
process.source = cms.Source("PoolSource",
                            fileNames=cms.untracked.vstring("/store/user/musella/RSGravitonToGG_kMpl001_M_1000_TuneCUEP8M1_13TeV_pythia8/CMSSW_7_4_15-Private-GEN/151020_211512/0000/RSGravitonToGammaGamma_kMpl001_M_1000_TuneCUEP8M1_13TeV_pythia8_GEN_1.root"
        )
)
process.TFileService = cms.Service("TFileService",
                                   fileName = cms.string("test.root")
)


#
# analysis configuration
#

import flashgg.Taggers.dumperConfigTools as cfgTools
from flashgg.Taggers.genDiphotonDumper_cfi import genDiphotonDumper
cfgTools.addCategories(genDiphotonDumper,
                       [("EB","max(abs(leadingPhoton.eta),abs(subLeadingPhoton.eta))<1.4442",0),
                        ("EE","1",0)
                        ],
                       variables=["genMass := mass","pt",
                                  "genLeadPt := leadingPhoton.pt",
                                  "genSubeadPt := subLeadingPhoton.pt",
                                  ],
                       histograms=["genMass>>genmass(1500,0,15000)",
                                   ]
                       )


from diphotons.Analysis.DiPhotonAnalysis import DiPhotonAnalysis
analysis = DiPhotonAnalysis(None,
                            massCut=customize.massCut,ptLead=customize.ptLead,ptSublead=customize.ptSublead,scaling=customize.scaling, ## kinematic cuts
                            genIsoDefinition=("genIso",10.),
                            dataTriggers=[],
                            mcTriggers=[],
                            )

analysis.addGenOnlySelection(process,genDiphotonDumper)

if not customize.lastAttempt:
    # make sure process doesn't get stuck due to slow I/O
    process.watchDog = cms.EDAnalyzer("IdleWatchdog",
                                      minIdleFraction=cms.untracked.double(0.1),
                                      tolerance=cms.untracked.int32(10),
                                      checkEvery=cms.untracked.int32(100),
                                      )
    process.watch = cms.Path(
        process.watchDog
        )


# this will call customize(process), configure the analysis paths and make the process unscheduled
analysis.customize(process,customize)

## print process.dumpPython()
