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
        if len(args) == 1 and ":=" in args[0]:
            args = [ a.lstrip(" ").rstrip(" ") for a in args[0].split(":=") ]
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

from diphotons.Analysis.highMassMVAs_cff import cShapeMVA_EB, cShapeMVA_EE

process.photonIdAnalyzer = cms.PSet(
  photons = cms.InputTag('flashggPhotons'), ## input for the simple example above
  packedGenParticles = cms.InputTag('packedGenParticles'),
  lumiWeight = cms.double(1.),
  processId = cms.string(""),
  rho = cms.InputTag('fixedGridRhoAll'),
  miniTreeCfg = cms.untracked.VPSet(
        ),
  vertexes = cms.InputTag("offlineSlimmedPrimaryVertices"),
  
  dumpRecHits = cms.untracked.bool(True),
  barrelRecHits = cms.InputTag('reducedEgamma','reducedEBRecHits'),
  endcapRecHits = cms.InputTag('reducedEgamma','reducedEERecHits'),
  
  idleWatchdog = cms.PSet(checkEvery = cms.untracked.int32(100),
                          minIdleFraction = cms.untracked.double(0.5),
                          tolerance = cms.untracked.int32(5)
                          ),

  mvas = cms.VPSet(
        cms.PSet(name=cms.string("cShapeMVA"), default=cms.double(-2.))
        ),
  mvaPreselection = cms.string("r9>0.8||egChargedHadronIso<20||egChargedHadronIso/pt<0.3"),
  categories = cms.VPSet(
        cms.PSet(cut=cms.string("abs(superCluster.eta)<1.5"),
                 cShapeMVA = cShapeMVA_EB
                 ),
        cms.PSet(cut=cms.string("abs(superCluster.eta)>=1.5"),
                 cShapeMVA = cShapeMVA_EE
                 ),        
        )
  ## recomputeNoZsShapes = cms.untracked.bool(True),
)

addMiniTreeVars(process.photonIdAnalyzer.miniTreeCfg,
                ["phi","eta","pt","energy",
                 
                 ("userFloat('cShapeMVA')","cShapeMVA"),
                 
                 ("superCluster.eta","scEta"),
                 ("superCluster.rawEnergy","scRawEnergy"),
                 ("? hasMatchedGenPhoton ? matchedGenPhoton.energy : 0","etrue"),
                 ## ("userFloat('dRMatch')","dRMatch"),
                 ("userFloat('genIso')","genIso"),
                 ("userFloat('frixIso')","frixIso"),
                 ("userInt('seedRecoFlag')","seedRecoFlag"),

                ## charged isolation
                 ("userFloat('chgIsoWrtVtx0')","chgIsoWrtVtx0"),
                 ("userFloat('chgIsoWrtVtx1')","chgIsoWrtVtx1"),
                 ("pfChgIsoWrtWorstVtx03","chgIsoWrtWorstVtx"),
                 
                 ## photon and neutral isolation
                 "egChargedHadronIso" ,"egNeutralHadronIso","egPhotonIso" ,
                 
                 "rndConeChIso := extraChIsoWrtChoosenVtx('rnd03')",
                 "stdChIso := extraChIsoWrtChoosenVtx('std03')",
                 "fprRndConeChIso := extraChIsoWrtChoosenVtx('fprRnd03')",
                 "fprChIso := extraChIsoWrtChoosenVtx('fpr03')",
                 
                 "rndConePhoIso := photon.extraPhoIso('rnd03')",
                 "stdPhoIso := photon.extraPhoIso('std03')",
                 "fprRndConePhoIso := photon.extraPhoIso('fprRnd03')",
                 "fprPhoIso := photon.extraPhoIso('fpr03')",
                 
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
                 ("superCluster.etaWidth","etaWidth"),("superCluster.phiWidth","phiWidth"),
                 ("sqrt(sipip)","sigmaIphiIphi"),
                 ("sieip","covarianceIetaIphi"),

                 ## the hcal full_5x5 only differ in the denominator and so aren't really worth saving
                 ## "hadronicDepth1OverEm",
                 ## "hadronicDepth2OverEm",
                 "hadronicOverEm",
                 "hadTowDepth1OverEm",
                 "hadTowDepth2OverEm",
                 ## "maxDR","maxDRDEta","maxDRDPhi","maxDRRawEnergy",
                 
                 ### "hadTowOverEm",
                 ## more cluster shapes
                 ## ("getE2nd","e2nd"),
                 ## ("getE2x5right","e2x5right"),
                 ## ("getE2x5left","e2x5left"),
                 ## ("getE2x5top","e2x5top"),
                 ## ("getE2x5bottom","e2x5bottom"),
                 ## ("getE2x5max","e2x5max"),
                 ## ("getEright","eright"),
                 ## ("getEleft","eleft"),
                 ## ("getEtop","etop"),
                 ## ("getEbottom","ebottom"),
                 ## ("getE1x3","e1x3"),
                 "s4",
                 ("esEffSigmaRR","sigmaRR"),
                 
                 ]
                )

# customization for job splitting, lumi weighting, etc.
from diphotons.MetaData.JobConfig import customize
customize.setDefault("maxEvents",500)
customize(process)
