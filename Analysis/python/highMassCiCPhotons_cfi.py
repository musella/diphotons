import FWCore.ParameterSet.Config as cms

from diphotons.Analysis.highMassCiCDiPhotons_cfi import highMassCiCVariables, highMassCiCCuts, highMassCiCCutsSB, highMassCiCVariablesV2, highMassCiCCutsV2, highMassCiCCutsV2SB

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

highMassCiCPhotonsV2 = cms.EDFilter(
    "GenericPhotonSelector",
    src = cms.InputTag("kinPhotons"),
    rho = cms.InputTag("fixedGridRhoAll"),
    cut = cms.string(
        "    (r9>0.8||egChargedHadronIso<20||egChargedHadronIso/pt<0.3)"
        "&&  egChargedHadronIso < 15"
        )
    ,
    variables = highMassCiCVariablesV2,
    categories = highMassCiCCutsV2,
    )

highMassCiCPhotonsSBV2 = highMassCiCPhotons.clone( categories=highMassCiCCutsV2SB )
