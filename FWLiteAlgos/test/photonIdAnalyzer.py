#!/usr/bin/env photonIdAnalyzer

import FWCore.ParameterSet.Config as cms

import FWCore.ParameterSet.VarParsing as VarParsing
from flashgg.MetaData.samples_utils import SamplesManager

def addMiniTreeVar(miniTreeCfg,var,name=None):
    if not name:
        name = var.replace(".","_").replace("get","")
    miniTreeCfg.append( cms.untracked.PSet(var=cms.untracked.string(var),
                                           name=cms.untracked.string(name)),
                        )
        
def addMiniTreeVars(miniTreeCfg,lst):
    for var in lst:
        args = [var]
        if type(var) == list or type(var) == tuple:
            args = var
        addMiniTreeVar(miniTreeCfg,*args)
        
options = VarParsing.VarParsing ('analysis')
options.setDefault ('maxEvents',100)
options.register ('dataset',
                  "", # default value
                  VarParsing.VarParsing.multiplicity.singleton, # singleton or list
                  VarParsing.VarParsing.varType.string,          # string, int, or float
                  "dataset")
options.register ('campaign',
                  "isolation_studies", # default value
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
options.register ('nJobs',
                  0, # default value
                  VarParsing.VarParsing.multiplicity.singleton, # singleton or list
                  VarParsing.VarParsing.varType.int,          # string, int, or float
                  "nJobs")
options.register ('jobId',
                  -1, # default value
                  VarParsing.VarParsing.multiplicity.singleton, # singleton or list
                  VarParsing.VarParsing.varType.int,          # string, int, or float
                  "jobId")

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
    dataset = SamplesManager("$CMSSW_BASE/src/diphotons/MetaData/data/%s/datasets.json" % options.campaign,
                             ["$CMSSW_BASE/src/diphotons/MetaData/data/cross_sections.json"],
                             ).getDatasetMetaData(options.maxEvents,options.dataset,jobId=options.jobId,nJobs=options.nJobs)
    print dataset

outputFile=options.outputFile
if options.jobId != -1:
    outputFile = "%s_%d.root" % ( outputFile.replace(".root",""), options.jobId )

process = cms.Process("FWLitePlots")

process.fwliteInput = cms.PSet(
    fileNames = cms.vstring(),
    ## fileNames = cms.vstring('root://eoscms//eos/cms/store/group/phys_higgs/cmshgg/flashgg/isolation_studies/alphaV1-52-g0636fd0/GJets_HT-600toInf_Tune4C_13TeV-madgraph-tauola/isolation_studies-alphaV1-52-g0636fd0-v0/141030_224209/0000/myOutputFile_9.root'),
    ## fileNames = cms.vstring('root://xrootd-cms.infn.it//store/group/phys_higgs/cmshgg/flashgg/isolation_studies/alphaV1-52-g0636fd0/GJets_HT-600toInf_Tune4C_13TeV-madgra_ph-tauola/isolation_studies-alphaV1-52-g0636fd0-v0/141030_224209/0000/myOutputFile_9.root'),  ## mandatory
    maxEvents   = cms.int32(options.maxEvents),
    outputEvery = cms.uint32(200),
)


process.fwliteOutput = cms.PSet(
      fileName = cms.string(outputFile)      ## mandatory
)

process.photonIdAnalyzer = cms.PSet(
  photons = cms.InputTag('flashggPhotons'), ## input for the simple example above
  packedGenParticles = cms.InputTag('packedGenParticles'),
  lumiWeight = cms.double(1.),
  rhoFixedGrid = cms.InputTag('fixedGridRhoAll'),
  miniTreeCfg = cms.untracked.VPSet(
        ),
  vertexes = cms.InputTag("offlineSlimmedPrimaryVertices"),
)

addMiniTreeVars(process.photonIdAnalyzer.miniTreeCfg,
                ["phi","eta","pt","energy",
                 
                 ("superCluster.eta","scEta"),
                 ("superCluster.rawEnergy","scRawEnergy"),
                 ("userFloat('etrue')","etrue"),
                 ("userFloat('dRMatch')","dRMatch"),
                 ("userInt('seedRecoFlag')","seedRecoFlag"),

                ## charged isolation
                 ("userFloat('chgIsoWrtVtx0')","chgIsoWrtVtx0"),
                 ("userFloat('chgIsoWrtVtx1')","chgIsoWrtVtx1"),
                 ("userFloat('chgIsoWrtVtx2')","chgIsoWrtVtx2"),
                 ("userFloat('chgIsoWrtVtx3')","chgIsoWrtVtx3"),
                 ("userFloat('chgIsoWrtVtx4')","chgIsoWrtVtx4"),
                 ("getpfChgIsoWrtWorstVtx03","chgIsoWrtWorstVtx"),
                 
                 ## photon and neutral isolation
                 ("userIso(0)" ,"phoIsoBlock"),
                 ("userIso(1)" ,"neuIsoBlock"),
                 ("userIso(2)" ,"phoIsoVeto007"),
                 ("userIso(3)" ,"phoIsoVeto015"),
                 ("userIso(4)" ,"phoIsoBlockVeto015"),
                 ("userIso(5)" ,"neuIsoBlockRing005"),
                 ("userIso(6)" ,"neuIsoBlockRing010"),
                 ("userIso(7)" ,"neuIsoBlockRing015"),
                 ("userIso(8)" ,"neuIsoBlockRing020"),
                 ("userIso(9)" ,"neuIsoBlockRing030"),
                 ("userIso(10)","neuIsoRing005"),
                 ("userIso(11)","neuIsoRing010"),
                 ("userIso(12)","neuIsoRing015"),
                 ("userIso(13)","neuIsoRing020"),
                 ("userIso(14)","neuIsoRing030"),
                 "passElectronVeto","hasPixelSeed",
                 ## cluster shapes
                 "e1x5",           "full5x5_e1x5",           
                 "e2x5",           "full5x5_e2x5",           
                 "e3x3",           "full3x3_e3x3",           
                 "e5x5",           "full5x5_e5x5",           
                 "maxEnergyXtal",  "full5x5_maxEnergyXtal",  
                 "sigmaIetaIeta",  "full5x5_sigmaIetaIeta",  
                 "r1x5",           "full5x5_r1x5",           
                 "r2x5",           "full5x5_r2x5",           
                 "r9",             "full5x5_r9",             
                 ## the hcal full_5x5 only differ in the denominator and so aren't really worth saving
                 "hadronicDepth1OverEm",
                 "hadronicDepth2OverEm",
                 "hadronicOverEm",
                 "hadTowDepth1OverEm",
                 "hadTowDepth2OverEm",
                 "maxDR","maxDRDEta","maxDRDPhi","maxDRRawEnergy",
                 "hadTowOverEm",
                 ## more cluster shapes
                 ("getE2nd","e2nd"),
                 ("getE2x5right","e2x5right"),
                 ("getE2x5left","e2x5left"),
                 ("getE2x5top","e2x5top"),
                 ("getE2x5bottom","e2x5bottom"),
                 ("getE2x5max","e2x5max"),
                 ("getEright","eright"),
                 ("getEleft","eleft"),
                 ("getEtop","etop"),
                 ("getEbottom","ebottom"),
                 ("getE1x3","e1x3"),
                 ("getS4","s4"),
                 ("getESEffSigmaRR","eSEffSigmaRR"),

                 ]
                )

if dataset:
    name,xsec,totEvents,files = dataset
    if xsec != 0.:
        process.photonIdAnalyzer.lumiWeight = xsec["xs"]/float(totEvents)*options.targetLumi
    process.fwliteInput.fileNames.extend([ str("%s%s" % (options.filePrepend,f)) for f in  files])
    

print process.fwliteInput.fileNames
