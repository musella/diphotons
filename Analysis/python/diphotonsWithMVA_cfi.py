import FWCore.ParameterSet.Config as cms
from diphotons.Analysis.highMassMVAs_cff import cShapeMVA_EB, cShapeMVA_EE

diphotonsWithMVA = cms.EDProducer(
    "FlashggPerPhotonMVADiPhotonProducer",
    src = cms.InputTag("tmpKinDiPhotons"),
    rho = cms.InputTag("fixedGridRhoAll"),
    vertexes = cms.InputTag("offlineSlimmedPrimaryVertices"),
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
    )
