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
        
process = cms.Process("FWLitePlots")

process.fwliteInput = cms.PSet(
    fileNames = cms.vstring(),
    maxEvents   = cms.int32(100),
    outputEvery = cms.uint32(200),
)


process.fwliteOutput = cms.PSet(
      fileName = cms.string("output.root")      ## mandatory
)

process.photonIdAnalyzer = cms.PSet(
  photons = cms.InputTag('flashggPhotons'), ## input for the simple example above
  packedGenParticles = cms.InputTag('packedGenParticles'),
  lumiWeight = cms.double(1.),
  processId = cms.string(""),
  rho = cms.InputTag('fixedGridRhoAll'),
  miniTreeCfg = cms.untracked.VPSet(
        ),
  vertexes = cms.InputTag("offlineSlimmedPrimaryVertices"),
  
  dumpRecHits = cms.untracked.bool(False),
  dumpAllRechisInfo = cms.untracked.bool(False),
  barrelRecHits = cms.InputTag('reducedEgamma','reducedEBRecHits'),
  endcapRecHits = cms.InputTag('reducedEgamma','reducedEERecHits'),

  idleWatchdog = cms.PSet(checkEvery = cms.untracked.int32(1000),
                          minIdleFraction = cms.untracked.double(0.3),
                          tolerance = cms.untracked.int32(5)
                          ),
  
  mvas = cms.VPSet(),
  mvaPreselection = cms.string("0"),
  categories = cms.VPSet()

  ## dumpFakes = cms.untracked.bool(False),

  ## recomputeNoZsShapes = cms.untracked.bool(True),
)

addMiniTreeVars(process.photonIdAnalyzer.miniTreeCfg,
                ["phi","eta","pt","energy",
                 
                 ("superCluster.eta","scEta"),("superCluster.eta","scPhy"),
                 ("superCluster.rawEnergy","scRawEnergy"),
                 ("superCluster.preshowerEnergy","scPreshowerEnergy"),
                 ("superCluster.clustersSize","scClustersSize"),
                 ("superCluster.seed.energy","scSeedEnergy"),
                 ("superCluster.energy","scEnergy"),
                 
                 ("? hasMatchedGenPhoton ? matchedGenPhoton.energy : 0","etrue"),

                 ("userFloat('genIso')","genIso"),
                 ("userFloat('frixIso')","frixIso"),
                 ("userInt('seedRecoFlag')","seedRecoFlag"),
                                  
                 "passElectronVeto","hasPixelSeed",
                 ## cluster shapes
                 "e1x5",           "full5x5_e1x5",           
                 "e2x5",           "full5x5_e2x5",           
                 "e3x3",           "full5x5_e3x3",           
                 "e5x5",           "full5x5_e5x5",           
                 "maxEnergyXtal",  "full5x5_maxEnergyXtal",  
                 "sigmaIetaIeta",  "full5x5_sigmaIetaIeta",  
                 "r1x5",           "full5x5_r1x5",           
                 "r2x5",           "full5x5_r2x5",           
                 "r9",             "full5x5_r9",             
                 "eMax","e2nd","eTop","eBottom","eLeft","eRight",
                 "iEta","iPhi","cryEta","cryPhi",
                                  
                 "hadTowOverEm",
                 ## more cluster shapes
                 ("e2x5right" ,"e2x5Right"  ),
                 ("e2x5left"  ,"e2x5Left"   ),
                 ("e2x5top"   ,"e2x5Top"    ),
                 ("e2x5bottom","e2x5Bottom" ),
                 ("e2x5max"   ,"e2x5Max"    ),
                 ("e1x3"      ,"e1x3"       ),
                 ("s4"        ,"s4"         ),
                 
                 ("esEffSigmaRR","sigmaRR"),
                 ("sqrt(spp)","sigmaIphiIphi"),
                 ("sep","covarianceIetaIphi"),
                 ("superCluster.etaWidth","etaWidth"),("superCluster.phiWidth","phiWidth"),
                 
                 ("checkStatusFlag('kSaturated')","kSaturated"),("checkStatusFlag('kWeird')","kWeird"),
                 
                 ]
                )

# customization for job splitting, lumi weighting, etc.
from diphotons.MetaData.JobConfig import customize
customize.setDefault("maxEvents",500)
customize(process)

