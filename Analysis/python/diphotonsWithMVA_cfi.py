import FWCore.ParameterSet.Config as cms
# from diphotons.Analysis.highMassMVAs_cff import cShapeMVA_EB, cShapeMVA_EE
from diphotons.Analysis.highMassRegressions_cff import regressionSatEB, regressionSatEE, regressionNonSatEB, regressionNonSatEE


diphotonsWithMVA = cms.EDProducer(
    "FlashggPerPhotonMVADiPhotonProducer",
    src = cms.InputTag("tmpKinDiPhotons"),
    rho = cms.InputTag("fixedGridRhoAll"),
    vertexes = cms.InputTag("offlineSlimmedPrimaryVertices"),
    mvas = cms.VPSet(
        ## cms.PSet(name=cms.string("cShapeMVA"), default=cms.double(-2.))
        cms.PSet(name=cms.string("regressedEnergy"), default=cms.double(0.)),
        cms.PSet(name=cms.string("satRegressedEnergy"), default=cms.double(0.)),
        ),
    mvaPreselection = cms.string("abs(superCluster.eta)<1.4442 || abs(superCluster.eta)>1.566 && abs(superCluster.eta)<2.5"),
    categories = cms.VPSet(
        cms.PSet(cut=cms.string("abs(superCluster.eta)<1.5  && checkStatusFlag('kSaturated') && ! checkStatusFlag('kWeird')"),
                 satRegressedEnergy = regressionSatEB,
                 regressedEnergy = regressionSatEB
                 ),
        cms.PSet(cut=cms.string("abs(superCluster.eta)<1.5"),
                 regressedEnergy = regressionNonSatEB
                 ),
        cms.PSet(cut=cms.string("abs(superCluster.eta)>=1.5  && checkStatusFlag('kSaturated') && ! checkStatusFlag('kWeird')"),
                 satRegressedEnergy = regressionSatEE,
                 regressedEnergy = regressionSatEE
                 ),
        cms.PSet(cut=cms.string("abs(superCluster.eta)>=1.5"),
                 regressedEnergy = regressionNonSatEE
                 ),
    )
    )
