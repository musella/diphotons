#!/usr/bin/env cmsRun

import FWCore.ParameterSet.Config as cms
import FWCore.Utilities.FileUtils as FileUtils
import FWCore.ParameterSet.VarParsing as VarParsing

from flashgg.MetaData.samples_utils import SamplesManager

## CMD LINE OPTIONS ##
from diphotons.MetaData.JobConfig import customize
customize.setDefault("maxEvents",10000)
customize.setDefault("targetLumi",1.e+3)

customize.options.register ('massCut',
                            "200", # default value
                            VarParsing.VarParsing.multiplicity.singleton, # singleton or list
                            VarParsing.VarParsing.varType.string,          # string, int, or float
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
customize.parse()

# maxEvents is the max number of events processed of each file, not globally

## I/O SETUP ##
process = cms.Process("quickDumper")

process.load("FWCore.MessageService.MessageLogger_cfi")
process.maxEvents = cms.untracked.PSet(input = cms.untracked.int32(-1))
process.MessageLogger.cerr.FwkReport.reportEvery = cms.untracked.int32( 1000 )
# process.source = cms.Source ("PoolSource",
#                              fileNames = cms.untracked.vstring(options.inputFiles))
process.source = cms.Source ("PoolSource",
                             inputCommands = cms.untracked.vstring(
                                 "keep *",
                                 "drop *_slimmedMETsNoHF_*_*"
                                 ),
                             fileNames = cms.untracked.vstring(
                                 
                                     ""
                                 
                             )
)

#eventsToProcess = cms.untracked.VEventRange('1:1-1:'+str(options.maxEvents)))

process.TFileService = cms.Service("TFileService",
                                   fileName = cms.string(customize.options.outputFile))

## TAGS DUMPERS ##
from flashgg.Taggers.diphotonDumper_cfi import *
from flashgg.Taggers.photonDumper_cfi import *

## define categories and associated objects to dump
import flashgg.Taggers.dumperConfigTools as cfgTools

process.dumper = diphotonDumper
process.dumper.dumpTrees = True
process.dumper.nameTemplate = "gg_$LABEL"

dipho_variables=["dipho_sumpt      := sumPt",
                 "dipho_cosphi     := abs(cos(leadingPhoton.phi - subLeadingPhoton.phi))",
                 "mass             := mass",

                 # leading
                 "lPt            := leadingPhoton.pt",
                 "lEt            := leadingPhoton.et",
                 "lEta           := leadingPhoton.eta",
                 "lPhi           := leadingPhoton.phi",
                 "l_sieie        := leadingPhoton.sigmaIetaIeta",
                 "l_sipip        := sqrt(leadingPhoton.sipip)",
                 "l_hoe          := leadingPhoton.hadronicOverEm",
                 "l_sigmaEoE     := leadingPhoton.sigEOverE",
                 "l_ptoM         := leadingPhoton.pt/mass",
                 "l_R9           := leadingPhoton.r9",
                 "l_nTrk         := leadingPhoton.nTrkSolidConeDR03",
                 "l_nTrkHollow   := leadingPhoton.nTrkSolidConeDR03",
                 "l_phoIso       := leadingPhoton.egPhotonIso", 
                 "l_chgIsoVtx0   := leadingPhoton.pfChgIso03WrtVtx0",
                 "l_missingHits  := leadingPhoton.matchedGsfTrackInnerMissingHits",

                 # subleading 
                 "slPt           := subLeadingPhoton.pt",
                 "slEt           := subLeadingPhoton.et",
                 "slEta          := subLeadingPhoton.eta",
                 "slPhi          := subLeadingPhoton.phi",
                 "sl_sieie       := subLeadingPhoton.sigmaIetaIeta",
                 "sl_sipip       := sqrt(subLeadingPhoton.sipip)",
                 "sl_hoe         := subLeadingPhoton.hadronicOverEm",
                 "sl_sigmaEoE    := subLeadingPhoton.sigEOverE",
                 "sl_ptoM        := subLeadingPhoton.pt/mass",
                 "sl_R9          := subLeadingPhoton.r9",
                 "sl_nTrk        := subLeadingPhoton.nTrkSolidConeDR03",
                 "sl_nTrkHollow  := subLeadingPhoton.nTrkSolidConeDR03",
                 "sl_phoIso      := subLeadingPhoton.egPhotonIso", 
                 "sl_chgIsoVtx0  := subLeadingPhoton.pfChgIso03WrtVtx0",
                 "sl_missingHits := subLeadingPhoton.matchedGsfTrackInnerMissingHits",
                 
                 ## vertex
                 "genVtxZ       := genPV.z",
                 "recoVtxZ      := vtx.z",
                 "recoVtxIndex  := vertexIndex"

                 # "etaWidth := superCluster.etaWidth",
                 # "phiWidth := superCluster.phiWidth",
]

# CATEGORIES

if ":" in customize.massCut:
    massCutEB,massCutEE = map(float,customize.massCut.split(":"))
    massCut = min(massCutEB,massCutEE)
else:
    massCutEB,massCutEE = None,None
    massCut = float(customize.massCut)

if massCutEB or massCutEE:
    cfgTools.addCategory(diphotonDumper,"RejectLowMass",
                         "   (max(abs(leadingPhoton.superCluster.eta),abs(subLeadingPhoton.superCluster.eta))<1.4442 && mass <= %f)"
                         "|| (max(abs(leadingPhoton.superCluster.eta),abs(subLeadingPhoton.superCluster.eta))>1.566  && mass <= %f)" %
                                            (massCutEB,massCutEE),-1)

cfgTools.addCategories(process.dumper,
                       ## categories definition
                       [#("all","mass>200",0),
                        ("EB","max(abs(leadingPhoton.superCluster.eta), abs(subLeadingPhoton.superCluster.eta))<1.4442", 0),
                        ("EE","1",0)
                    ],                       
                       ## variables to be dumped in trees/datasets. Same variables for all categories
                       variables=dipho_variables,

                       ## histograms
                       histograms=[]
)

process.p1 = cms.Path(process.dumper)

customize(process)
