import FWCore.ParameterSet.Config as cms

import flashgg.Taggers.dumperConfigTools as cfgTools

cShapeMVA_EB = cms.PSet(
    variables=cms.VPSet(),
    classifier=cms.string("BDTCShape_EB"),
    weights=cms.FileInPath("diphotons/Analysis/data/TMVAClassification_BDTCShape_EB.weights.xml")
    ## classifier=cms.string("BDTCShape_test_EB"),
    ## weights=cms.FileInPath("diphotons/Analysis/data/TMVAClassification_BDTCShape_test_EB.weights.xml")
    )

cShapeMVA_EE = cms.PSet(
    variables=cms.VPSet(),
    classifier=cms.string("BDTCShape_EE"),
    weights=cms.FileInPath("diphotons/Analysis/data/TMVAClassification_BDTCShape_EE.weights.xml")
    ## classifier=cms.string("BDTCShape_test_EE"),
    ## weights=cms.FileInPath("diphotons/Analysis/data/TMVAClassification_BDTCShape_test_EE.weights.xml")
    )

variables = ["full5x5_sigmaIetaIeta","etaWidth := superCluster.etaWidth","phiWidth := superCluster.phiWidth",
             "covarianceIetaIphi := sieip","r9","scEta := superCluster.eta","pt", "s4"
             "hadronicOverEm"
             ]

## variables = ["full5x5_sigmaIetaIeta","etaWidth := superCluster.etaWidth","phiWidth := superCluster.phiWidth",
##              "covarianceIetaIphi := sieip","r9","scEta := superCluster.eta","scRawEnergy := superCluster.rawEnergy","hadronicOverEm"
##              ]

cfgTools.addVariables(cShapeMVA_EB.variables,variables)
cfgTools.addVariables(cShapeMVA_EE.variables,variables)
