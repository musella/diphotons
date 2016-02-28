import FWCore.ParameterSet.Config as cms

diphotonsZeroTeslaVtx = cms.EDProducer("diphotonsBSVertexProducer",
                                       bsTag = cms.InputTag('offlineBeamSpot')
                                   )
