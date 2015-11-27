import FWCore.ParameterSet.Config as cms

import flashgg.Taggers.dumperConfigTools as cfgTools

variables = [
    "scEta := superCluster.eta",
    "r9 := full5x5_r9",
    "hoe := hadronicOverEm",
    "rho := global.rho",
    "scRawEnergy := superCluster.rawEnergy",
    "eMax",
    "eTop",
    "eBottom",
    "eLeft",
    "eRight",
    "e2x5Top := e2x5top",
    "e2x5Bottom := e2x5bottom",
    "e2x5Left := e2x5left",
    "e2x5Right := e2x5right",
    "e1x5 := full5x5_e1x5",
    "e3x3 := full5x5_e3x3",
    "e5x5 := full5x5_e5x5",
    "etaWidth := superCluster.etaWidth",
    "phiWidth := superCluster.phiWidth"
]

def mkRegression(name,weights,variabs):
    pset =  cms.PSet(    
        variables=cms.VPSet(),
        classifier=cms.string(name),
        weights=cms.FileInPath(weights),
        regression=cms.bool(True),
        )
    
    cfgTools.addVariables(pset.variables,variabs)
    return pset



regressionSatEB = mkRegression("BDTG","diphotons/Analysis/data/regression/Sat_EB.xml",variables)
regressionSatEE = mkRegression("BDTG","diphotons/Analysis/data/regression/Sat_EE.xml",variables)
regressionNonSatEB = mkRegression("BDTG","diphotons/Analysis/data/regression/nonSat_EB.xml",variables)
regressionNonSatEE = mkRegression("BDTG","diphotons/Analysis/data/regression/nonSat_EE.xml",variables)
