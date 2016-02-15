#!/usr/bin/env cmsRun

import FWCore.ParameterSet.Config as cms
import FWCore.Utilities.FileUtils as FileUtils
from FWCore.ParameterSet.VarParsing import VarParsing

## CMD LINE OPTIONS ##
from diphotons.MetaData.JobConfig import customize
customize.setDefault("maxEvents", -1)
customize.setDefault("inputFiles", "myMicroAODOutputFile.root")
customize.setDefault("outputFile", "vtxs_file.root")

process = cms.Process("Analysis")

process.load("FWCore.MessageService.MessageLogger_cfi")
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(customize.options.maxEvents) )
process.MessageLogger.cerr.FwkReport.reportEvery = cms.untracked.int32( 1000 )

## I/O FILES
process.source = cms.Source ("PoolSource",
                             fileNames = cms.untracked.vstring(customize.options.inputFiles))

process.TFileService = cms.Service("TFileService",
                                   fileName = cms.string(customize.options.outputFile))

## PROCESS
process.load("diphotons.Analysis.ZeroTeslaVtxAnalyzer_cfi") 

process.idleWatchdog=cms.EDAnalyzer("IdleWatchdog",
                                    checkEvery = cms.untracked.int32(100),
                                    minIdleFraction = cms.untracked.double(0.5),
                                    tolerance = cms.untracked.int32(5)
                                    )

process.p1 = cms.Path(process.diphotonsZeroTeslaVtx)

## process.e = cms.EndPath(process.out)

customize(process)

