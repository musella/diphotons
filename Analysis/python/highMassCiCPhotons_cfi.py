import FWCore.ParameterSet.Config as cms

from diphotons.Analysis.highMassCiCDiPhotons_cfi import highMassCiCVariables, highMassCiCCuts, highMassCiCCutsSB

highMassCiCPhotons = cms.EDFilter(
    "GenericPhotonSelector",
    src = cms.InputTag("kinPhotons"),
    rho = cms.InputTag("fixedGridRhoAll"),
    cut = cms.string(
        "    (r9>0.8||egChargedHadronIso<20||egChargedHadronIso/pt<0.3)"
        "&&  egChargedHadronIso < 15"
        )
    ,
    variables = highMassCiCVariables,
    categories = highMassCiCCuts,
    )

highMassCiCPhotonsSB = highMassCiCPhotons.clone( categories=highMassCiCCutsSB )
