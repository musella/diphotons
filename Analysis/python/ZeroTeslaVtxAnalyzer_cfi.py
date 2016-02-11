import FWCore.ParameterSet.Config as cms

diphotonsZeroTeslaVtx = cms.EDAnalyzer("diphotonsZeroTeslaVtxAnalyzer",
                                       src = cms.InputTag('flashggDiPhotons')
                                   )
