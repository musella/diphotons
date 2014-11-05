#!/usr/bin/env photonIdAnalyzer

import FWCore.ParameterSet.Config as cms

import FWCore.ParameterSet.VarParsing as VarParsing
from flashgg.MetaData.samples_utils import SamplesManager


options = VarParsing.VarParsing ('analysis')
options.setDefault ('maxEvents',100)
options.register ('dataset',
                  "", # default value
                  VarParsing.VarParsing.multiplicity.singleton, # singleton or list
                  VarParsing.VarParsing.varType.string,          # string, int, or float
                  "dataset")
options.register ('campaign',
                  "CSA14", # default value
                  VarParsing.VarParsing.multiplicity.singleton, # singleton or list
                  VarParsing.VarParsing.varType.string,          # string, int, or float
                  "campaign")
options.register ('useAAA',
                  False, # default value
                  VarParsing.VarParsing.multiplicity.singleton, # singleton or list
                  VarParsing.VarParsing.varType.bool,          # string, int, or float
                  "useAAA")
options.register ('useEOS',
                  True, # default value
                  VarParsing.VarParsing.multiplicity.singleton, # singleton or list
                  VarParsing.VarParsing.varType.bool,          # string, int, or float
                  "useEOS")
options.register ('targetLumi',
                  1.e+3, # default value
                  VarParsing.VarParsing.multiplicity.singleton, # singleton or list
                  VarParsing.VarParsing.varType.float,          # string, int, or float
                  "targetLumi")

options.parseArguments()

if options.useAAA:
    print "Using AAA"
    options.filePrepend = "root://xrootd-cms.infn.it/"
elif options.useEOS:
    print "Using EOS"
    options.filePrepend = "root://eoscms//eos/cms"

dataset = None
if options.dataset != "":
    print "Reading dataset (%s) %s" % ( options.campaign, options.dataset)
    dataset = SamplesManager("$CMSSW_BASE/src/flashgg/Production/data/%s/datasets.json" % options.campaign).getDataset(options.maxEvents,options.dataset)
    print dataset

process = cms.Process("FWLitePlots")

process.fwliteInput = cms.PSet(
    fileNames = cms.vstring(),
    ## fileNames = cms.vstring('root://eoscms//eos/cms/store/group/phys_higgs/cmshgg/flashgg/isolation_studies/alphaV1-52-g0636fd0/GJets_HT-600toInf_Tune4C_13TeV-madgraph-tauola/isolation_studies-alphaV1-52-g0636fd0-v0/141030_224209/0000/myOutputFile_9.root'),
    ## fileNames = cms.vstring('root://xrootd-cms.infn.it//store/group/phys_higgs/cmshgg/flashgg/isolation_studies/alphaV1-52-g0636fd0/GJets_HT-600toInf_Tune4C_13TeV-madgra_ph-tauola/isolation_studies-alphaV1-52-g0636fd0-v0/141030_224209/0000/myOutputFile_9.root'),  ## mandatory
    maxEvents   = cms.int32(options.maxEvents),
    outputEvery = cms.uint32(200),
)


process.fwliteOutput = cms.PSet(
      fileName = cms.string(options.outputFile)      ## mandatory
)

process.photonIdAnalyzer = cms.PSet(
  photons = cms.InputTag('flashggPhotons'), ## input for the simple example above
  packedGenParticles = cms.InputTag('packedGenParticles'),
  lumi_weight = cms.double(1.)
)

if dataset:
    name,xsec,totEvents,files = dataset
    if xsec != 0.:
        process.photonIdAnalyzer.lumi_weight = xsec["xs"]/float(totEvents)*options.targetLumi
    process.fwliteInput.fileNames.extend([ str("%s%s" % (options.filePrepend,f)) for f in  files])
    

print process.fwliteInput.fileNames
