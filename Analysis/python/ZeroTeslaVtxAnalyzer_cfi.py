import FWCore.ParameterSet.Config as cms

diphotonsZeroTeslaVtx = cms.EDAnalyzer("diphotonsZeroTeslaVtxAnalyzer",
                                       src = cms.InputTag('flashggDiPhotons'),
                                       vtxs = cms.InputTag('offlineSlimmedPrimaryVertices'),
                                       dummyVtx = cms.InputTag('dummyVtx')
                                   )
