import FWCore.ParameterSet.Config as cms

scaleBins = cms.PSet(
    variables = cms.vstring("abs(superCluster.eta)","r9"),
    bins = cms.VPSet(
        cms.PSet( lowBounds = cms.vdouble(0.,-999.), upBounds = cms.vdouble(1.,0.940),
                  values = cms.vdouble( 1./.9988 - 1. ), uncertainties = cms.vdouble( 0.0 ) ),
        cms.PSet( lowBounds = cms.vdouble(0.,0.940), upBounds = cms.vdouble(1.,999.),
                  values = cms.vdouble( 1./.9954 - 1.  ), uncertainties = cms.vdouble( 0.0 ) ),

        cms.PSet( lowBounds = cms.vdouble(1.,-999.), upBounds = cms.vdouble(1.500,0.940),
                  values = cms.vdouble( 1./1.0065 - 1. ), uncertainties = cms.vdouble( 0.0 ) ),
        cms.PSet( lowBounds = cms.vdouble(1.,0.940), upBounds = cms.vdouble(1.500,999.),
                  values = cms.vdouble( 1./0.9966 - 1. ), uncertainties = cms.vdouble( 0.0 ) ),

        cms.PSet( lowBounds = cms.vdouble(1.500,-999.), upBounds = cms.vdouble(2.,0.940),
                  values = cms.vdouble( 1./0.9953 - 1. ), uncertainties = cms.vdouble( 0.0 ) ),
        cms.PSet( lowBounds = cms.vdouble(1.500,0.940), upBounds = cms.vdouble(2.,999.),
                  values = cms.vdouble( 1./0.9863 - 1. ), uncertainties = cms.vdouble( 0.0 ) ),

        cms.PSet( lowBounds = cms.vdouble(2.,-999.), upBounds = cms.vdouble(999.,0.940),
                  values = cms.vdouble( 1./0.9857 - 1. ), uncertainties = cms.vdouble( 0.0 ) ),
        cms.PSet( lowBounds = cms.vdouble(2.,0.940), upBounds = cms.vdouble(999.,999.),
                  values = cms.vdouble( 1./0.9786 - 1. ), uncertainties = cms.vdouble( 0.0 ) ),
        ))

smearBins = cms.PSet(
    variables = cms.vstring("abs(superCluster.eta)","r9"),
    bins = cms.VPSet(
        cms.PSet( lowBounds = cms.vdouble(0.,-999.), upBounds = cms.vdouble(1.,0.940),
                  values = cms.vdouble( 0.141 ), uncertainties = cms.vdouble( 0.0 ) ),
        cms.PSet( lowBounds = cms.vdouble(0.,0.940), upBounds = cms.vdouble(1.,999.),
                  values = cms.vdouble( 0.136 ), uncertainties = cms.vdouble( 0.0 ) ),

        cms.PSet( lowBounds = cms.vdouble(1.,-999.), upBounds = cms.vdouble(1.500,0.940),
                  values = cms.vdouble( 0.171 ), uncertainties = cms.vdouble( 0.0 ) ),
        cms.PSet( lowBounds = cms.vdouble(1.,0.940), upBounds = cms.vdouble(1.500,999.),
                  values = cms.vdouble( 0.208 ), uncertainties = cms.vdouble( 0.0 ) ),

        cms.PSet( lowBounds = cms.vdouble(1.500,-999.), upBounds = cms.vdouble(2.,0.940),
                  values = cms.vdouble( 0.273 ), uncertainties = cms.vdouble( 0.0 ) ),
        cms.PSet( lowBounds = cms.vdouble(1.500,0.940), upBounds = cms.vdouble(2.,999.),
                  values = cms.vdouble( 0.280 ), uncertainties = cms.vdouble( 0.0 ) ),

        cms.PSet( lowBounds = cms.vdouble(2.,-999.), upBounds = cms.vdouble(999.,0.940),
                  values = cms.vdouble( 0.308 ), uncertainties = cms.vdouble( 0.0 ) ),
        cms.PSet( lowBounds = cms.vdouble(2.,0.940), upBounds = cms.vdouble(999.,999.),
                  values = cms.vdouble( 0.317 ), uncertainties = cms.vdouble( 0.0 ) ),
        ))

highMassCorrectedDiphotonsData = cms.EDProducer('FlashggDiPhotonSystematicProducer',
                                            src = cms.InputTag("flashggDiphotons"),
                                            SystMethods2D = cms.VPSet(),
                                            SystMethods = cms.VPSet(cms.PSet( PhotonMethodName = cms.string("FlashggPhotonScale"),
                                                                              MethodName = cms.string("FlashggDiPhotonFromPhoton"),
                                                                              Label = cms.string(""),
                                                                              NSigmas = cms.vint32(),
                                                                              OverallRange = cms.string("1"),
                                                                              BinList = scaleBins,
                                                                              Debug = cms.untracked.bool(False)
                                                                              ),
                                                                    )
                                            )

highMassCorrectedDiphotonsMC = cms.EDProducer('FlashggDiPhotonSystematicProducer',
                                            src = cms.InputTag("flashggDiphotons"),
                                            SystMethods2D = cms.VPSet(),
                                            SystMethods = cms.VPSet(cms.PSet( PhotonMethodName = cms.string("FlashggPhotonSmearConstant"),
                                                                              MethodName = cms.string("FlashggDiPhotonFromPhoton"),
                                                                              Label = cms.string(""),
                                                                              NSigmas = cms.vint32(),
                                                                              OverallRange = cms.string("1"),
                                                                              BinList = scaleBins,
                                                                              Debug = cms.untracked.bool(False)
                                                                              ),
                                                                    )
                                            )
