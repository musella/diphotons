import FWCore.ParameterSet.Config as cms

highMassCiCDiPhotons = cms.EDFilter(
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
        "hadTowOverEm",
        "(?r9>0.8||egChargedHadronIso<20||egChargedHadronIso/pt<0.3?full5x5_sigmaIetaIeta:sigmaIetaIeta)",
        "passElectronVeto"
        ),
    categories = cms.VPSet(
        cms.PSet(cut=cms.string("abs(superCluster.eta)<1.5 && r9>0.94"),
                 selection = cms.VPSet(
                cms.PSet(min=cms.string("-13000"),max=cms.string("5.95"),              
                         rhocorr=cms.string("0.")),
                cms.PSet(min=cms.string("-13000"),max=cms.string("2.87"), 
                         rhocorr=cms.string(
                         " (?abs(superCluster.eta)<=0.9?0.21:0)"
                         "+(?abs(superCluster.eta)>0.9?0.2:0)"
                         )),
                cms.PSet(min=cms.string("-13000"),max=cms.string("27.4"),
                         rhocorr=cms.string(
                         " (?abs(superCluster.eta)<=0.9?0.04:0)"
                         "+(?abs(superCluster.eta)>0.9?0.059:0)"
                         )),
                cms.PSet(min=cms.string("-13000"),max=cms.string("4.53e-1"),
                         rhocorr=cms.string("0")),
                cms.PSet(min=cms.string("0.001"), max=cms.string("1.05e-2"),
                         rhocorr=cms.string("0")),
                cms.PSet(min=cms.string("0.5"),max=cms.string("10"),
                         rhocorr=cms.string("0")),
                )
                 ),
        cms.PSet(cut=cms.string("abs(superCluster.eta)<1.5 && r9<=0.94"),
                 selection = cms.VPSet(
                cms.PSet(min=cms.string("-13000"),max=cms.string("7.08"),              
                         rhocorr=cms.string("0.")),
                cms.PSet(min=cms.string("-13000"),max=cms.string("5.47"), 
                         rhocorr=cms.string(
                         " (?abs(superCluster.eta)<=0.9?0.21:0)"
                         "+(?abs(superCluster.eta)>0.9?0.2:0)"
                         )),
                cms.PSet(min=cms.string("-13000"),max=cms.string("30."),
                         rhocorr=cms.string(
                         " (?abs(superCluster.eta)<=0.9?0.04:0)"
                         "+(?abs(superCluster.eta)>0.9?0.059:0)"
                         )),
                cms.PSet(min=cms.string("-13000"),max=cms.string("2.12e-1"),
                         rhocorr=cms.string("0")),
                cms.PSet(min=cms.string("0.001"), max=cms.string("1.05e-2"),
                         rhocorr=cms.string("0")),
                cms.PSet(min=cms.string("0.5"),max=cms.string("10"),
                         rhocorr=cms.string("0")),
                )
                 ),
        cms.PSet(cut=cms.string("abs(superCluster.eta)>=1.5 && r9>0.94"),
                 selection = cms.VPSet(
                cms.PSet(min=cms.string("-13000"),max=cms.string("6.10"),              
                         rhocorr=cms.string("0.")),
                cms.PSet(min=cms.string("-13000"),max=cms.string("5.98"), 
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
                cms.PSet(min=cms.string("-13000"),max=cms.string("6.3e-2"),
                         rhocorr=cms.string("0")),
                cms.PSet(min=cms.string("0.001"), max=cms.string("2.82e-2"),
                         rhocorr=cms.string("0")),
                cms.PSet(min=cms.string("0.5"),max=cms.string("10"),
                         rhocorr=cms.string("0")),
                )
                 ),
        cms.PSet(cut=cms.string("abs(superCluster.eta)>=1.5 && r9<=0.94"),
                 selection = cms.VPSet(
                cms.PSet(min=cms.string("-13000"),max=cms.string("5.07"),
                         rhocorr=cms.string("0.")),
                cms.PSet(min=cms.string("-13000"),max=cms.string("3.44"), 
                         rhocorr=cms.string(
                         " (?abs(superCluster.eta)<=2.0                           ?0.14:0)"
                         "+(?abs(superCluster.eta)>2.0&&abs(superCluster.eta)<=2.2?0.22:0)"
                         "+(?abs(superCluster.eta)>2.2                            ?0.31:0)"
                         )),
                cms.PSet(min=cms.string("-13000"),max=cms.string("15."),
                         rhocorr=cms.string(
                         " (?abs(superCluster.eta)<=2.0                           ?0.05:0)"
                         "+(?abs(superCluster.eta)>2.0&&abs(superCluster.eta)<=2.2?0.05:0)"
                         "+(?abs(superCluster.eta)>2.2                            ?0.15:0)"
                         )),
                cms.PSet(min=cms.string("-13000"),max=cms.string("7.8e-2"),
                         rhocorr=cms.string("0")),
                cms.PSet(min=cms.string("0.001"), max=cms.string("2.80-2"),
                         rhocorr=cms.string("0")),
                cms.PSet(min=cms.string("0.5"),max=cms.string("10"),
                         rhocorr=cms.string("0")),
                )
                 ),

        ),
    )
