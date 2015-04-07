import FWCore.ParameterSet.Config as cms

highMassMVADiphotons = cms.EDFilter(
    "GenericDiPhotonCandidateSelector",
    src = cms.InputTag("kinDiPhotons"),
    rho = cms.InputTag("fixedGridRhoAll"),
    cut = cms.string(
        "    (leadingPhoton.r9>0.8||leadingPhoton.egChargedHadronIso<20||leadingPhoton.egChargedHadronIso/leadingPhoton.pt<0.3)"
        " && (subLeadingPhoton.r9>0.8||subLeadingPhoton.egChargedHadronIso<20||subLeadingPhoton.egChargedHadronIso/subLeadingPhoton.pt<0.3)" 
        )
    ,
    variables = cms.vstring(
        "egChargedHadronIso", 
        "egPhotonIso", 
        "egNeutralHadronIso",
        "userFloat('cShapeMVA')",
        "passElectronVeto"
        ),
    categories = cms.VPSet(
        cms.PSet(cut=cms.string("abs(superCluster.eta)<1.5"),
                 selection = cms.VPSet(
                cms.PSet(min=cms.string("-13000"),max=cms.string("12.8"),              
                         rhocorr=cms.string("0.")),
                cms.PSet(min=cms.string("-13000"),max=cms.string("26"), 
                         rhocorr=cms.string(
                         " (?abs(superCluster.eta)<=0.9?0.21:0)"
                         "+(?abs(superCluster.eta)>0.9?0.2:0)"
                         )),
                cms.PSet(min=cms.string("-13000"),max=cms.string("30"),
                         rhocorr=cms.string(
                         " (?abs(superCluster.eta)<=0.9?0.04:0)"
                         "+(?abs(superCluster.eta)>0.9?0.059:0)"
                         )),
                cms.PSet(min=cms.string("2.89e-2"),max=cms.string("1000."),
                         rhocorr=cms.string("0")),
                cms.PSet(min=cms.string("0.5"),max=cms.string("10"),
                         rhocorr=cms.string("0")),
                )
                 ),
        cms.PSet(cut=cms.string("abs(superCluster.eta)>=1.5"),
                 selection = cms.VPSet(
                cms.PSet(min=cms.string("-13000"),max=cms.string("12.0"),              
                         rhocorr=cms.string("0.")),
                cms.PSet(min=cms.string("-13000"),max=cms.string("19.6"), 
                         rhocorr=cms.string(
                         " (?abs(superCluster.eta)<=2.0                           ?0.14:0)"
                         "+(?abs(superCluster.eta)>2.0&&abs(superCluster.eta)<=2.2?0.22:0)"
                         "+(?abs(superCluster.eta)>2.2                            ?0.31:0)"
                         )),
                cms.PSet(min=cms.string("-13000"),max=cms.string("30."),
                         rhocorr=cms.string(
                         " (?abs(superCluster.eta)<=2.0                           ?0.05:0)"
                         "+(?abs(superCluster.eta)>2.0&&abs(superCluster.eta)<=2.2?0.05:0)"
                         "+(?abs(superCluster.eta)>2.2                            ?0.15:0)"
                         )),
                cms.PSet(min=cms.string("-2.34e-2"),max=cms.string("-13000"),
                         rhocorr=cms.string("0")),
                cms.PSet(min=cms.string("0.5"),max=cms.string("10"),
                         rhocorr=cms.string("0")),
                )
                ),
        ),
    )


