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
  rhoFixedGrid = cms.InputTag('fixedGridRhoAll'),
  miniTreeCfg = cms.untracked.VPSet(
        ),
  vertexes = cms.InputTag("offlineSlimmedPrimaryVertices"),
  
  dumpRecHits = cms.untracked.bool(True),
  barrelRecHits = cms.InputTag('reducedEgamma','reducedEBRecHits'),
  endcapRecHits = cms.InputTag('reducedEgamma','reducedEERecHits'),

  ## recomputeNoZsShapes = cms.untracked.bool(True),
)

addMiniTreeVars(process.photonIdAnalyzer.miniTreeCfg,
                ["phi","eta","pt","energy",
                 
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
                 ### ("userFloat('chgIsoWrtVtx2')","chgIsoWrtVtx2"),
                 ### ("userFloat('chgIsoWrtVtx3')","chgIsoWrtVtx3"),
                 ### ("userFloat('chgIsoWrtVtx4')","chgIsoWrtVtx4"),
                 ("getpfChgIsoWrtWorstVtx03","chgIsoWrtWorstVtx"),
                 
                 ## photon and neutral isolation
                 "egChargedHadronIso" ,"egNeutralHadronIso","egPhotonIso" ,
                 #### ("userIso(0)" ,"phoIsoBlock"),
                 #### ("userIso(1)" ,"neuIsoBlock"),
                 #### 
                 #### ("userIso(2)" ,"phoIsoVeto005"),
                 #### ("userIso(3)" ,"phoIsoVeto006"),
                 #### ("userIso(4)" ,"phoIsoVeto007"),
                 #### ("userIso(5)" ,"phoIsoVeto008"),
                 #### ## ("userIso(6)" ,"phoIsoBlockVeto005"),
                 #### 
                 #### ("userIso(8) +userIso(9) +userIso(10)+userIso(11)"  ,"neuIsoBlockVeto010"),
                 #### ("userIso(13)+userIso(14)+userIso(15)+userIso(16)"  ,"neuIsoVeto010"),
                 #### 
                 #### ("userIso(9) +userIso(10)+userIso(11)"  ,"neuIsoBlockVeto015"),
                 #### ("userIso(14)+userIso(15)+userIso(16)"  ,"neuIsoVeto015"),
                 #### 
                 #### ### ("userIso(7)"  ,"neuIsoBlockRing010"),
                 #### ### ("userIso(8)"  ,"neuIsoBlockRing012"),
                 #### ### ("userIso(9)"  ,"neuIsoBlockRing015"),
                 #### ### ("userIso(10)" ,"neuIsoBlockRing020"),
                 #### ### ("userIso(11)" ,"neuIsoBlockRing030"),
                 #### 
                 #### ### ("userIso(12)" ,"neuIsoRing010"),
                 #### ### ("userIso(13)" ,"neuIsoRing012"),
                 #### ### ("userIso(14)" ,"neuIsoRing015"),
                 #### ### ("userIso(15)" ,"neuIsoRing020"),
                 #### ### ("userIso(16)" ,"neuIsoRing030"),
                 #### 
                 #### 
                 #### ("userIso(2)"
                 ####  "-(userIso(17)+userIso(18)+userIso(19)+userIso(20)+userIso(21)+userIso(22))"
                 ####  "+(userIso(23)+userIso(24)+userIso(25)+userIso(26)+userIso(27)+userIso(28))",
                 ####  "phoIsoHybrid005"),
                 #### ("userIso(2)"
                 ####  "-(userIso(18)+userIso(19)+userIso(20)+userIso(21)+userIso(22))"
                 ####  "+(userIso(24)+userIso(25)+userIso(26)+userIso(27)+userIso(28))",
                 ####  "phoIsoHybrid007"),
                 #### ("userIso(2)"
                 ####  "-(userIso(19)+userIso(20)+userIso(21)+userIso(22))"
                 ####  "+(userIso(25)+userIso(26)+userIso(27)+userIso(28))",
                 ####  "phoIsoHybrid008"),
                 #### ("userIso(2)"
                 ####  "-(userIso(20)+userIso(21)+userIso(22))"
                 ####  "+(userIso(26)+userIso(27)+userIso(28))",
                 ####  "phoIsoHybrid010"),
                 #### ("userIso(2)"
                 ####  "-(userIso(21)+userIso(22))"
                 ####  "+(userIso(27)+userIso(28))",
                 ####  "phoIsoHybrid025"),
                 #### ("userIso(2)"
                 ####  "-(userIso(22))"
                 ####  "+(userIso(28))",
                 ####  "phoIsoHybrid020"),
                 

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
                 ("sqrt(getSipip)","sigmaIphiIphi"),
                 ("getSieip","covarianceIetaIphi"),

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
                 ("getS4","s4"),
                 ("getESEffSigmaRR","sigmaRR"),
                 
                 ]
                )

# customization for job splitting, lumi weighting, etc.
from diphotons.MetaData.JobConfig import customize
customize.setDefault("maxEvents",500)
customize(process)

