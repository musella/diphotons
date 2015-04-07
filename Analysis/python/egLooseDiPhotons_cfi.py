import FWCore.ParameterSet.Config as cms

egLooseDiPhotons = cms.EDFilter(
    "GenericDiPhotonCandidateSelector",
    src = cms.InputTag("kinDiPhotons"),
    rho = cms.InputTag("fixedGridRhoAll"),
    cut = cms.string(
        "1")
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
        cms.PSet(cut=cms.string("abs(superCluster.eta)<1.5"),
                 selection = cms.VPSet(
                cms.PSet(min=cms.string("-13000"),max=cms.string("2.94"),              
                         rhocorr=cms.string(
                         " (?abs(superCluster.eta)<=1?0.0089:0)"
                         "+(?abs(superCluster.eta)>1?0.0062:0)"
                         )),
                cms.PSet(min=cms.string("-13000"),max=cms.string("4.43 + 0.0004*pt"), 
                         rhocorr=cms.string(
                         " (?abs(superCluster.eta)<=1?0.0894:0)"
                         "+(?abs(superCluster.eta)>1?0.0750:0)"
                         )),
                cms.PSet(min=cms.string("-13000"),max=cms.string("3.16 + 0.0023*pt"),
                         rhocorr=cms.string(
                         " (?abs(superCluster.eta)<=1?0.0049:0)"
                         "+(?abs(superCluster.eta)>1?0.0108:0)"
                         )),
                cms.PSet(min=cms.string("-13000"),max=cms.string("0.032"),
                         rhocorr=cms.string("0")),
                cms.PSet(min=cms.string("0.001"), max=cms.string("0.01"),
                         rhocorr=cms.string("0")),
                cms.PSet(min=cms.string("0.5"),max=cms.string("10"),
                         rhocorr=cms.string("0")),
                )
                 ),
        cms.PSet(cut=cms.string("abs(superCluster.eta)>1.5"),
                 selection = cms.VPSet(
                cms.PSet(min=cms.string("-13000"),max=cms.string("3.07"),              
                         rhocorr=cms.string(
                         " (?abs(superCluster.eta)<=2?0.0086:0)"
                         "+(?abs(superCluster.eta)>2&&abs(superCluster.eta)<=2.2?0.0041:0)"
                         "+(?abs(superCluster.eta)>2.2&&abs(superCluster.eta)<=2.3?0.0113:0)"
                         "+(?abs(superCluster.eta)>2.3&&abs(superCluster.eta)<=2.4?0.0085:0)"
                         "+(?abs(superCluster.eta)>2.4?0.0039:0)"
                         )),
                cms.PSet(min=cms.string("-13000"),max=cms.string("2.11 + 0.0037*pt "), 
                         rhocorr=cms.string(
                         " (?abs(superCluster.eta)<=2?0.0423:0)"
                         "+(?abs(superCluster.eta)>2&&abs(superCluster.eta)<=2.2?0.0561:0)"
                         "+(?abs(superCluster.eta)>2.2&&abs(superCluster.eta)<=2.3?0.0882:0)"
                         "+(?abs(superCluster.eta)>2.3&&abs(superCluster.eta)<=2.4?0.1144:0)"
                         "+(?abs(superCluster.eta)>2.4?0.1684:0)"
                         )),
                cms.PSet(min=cms.string("-13000."),max=cms.string("17.16 + 0.0116*pt "),
                         rhocorr=cms.string(
                         " (?abs(superCluster.eta)<=2?0.0019:0)"
                         "+(?abs(superCluster.eta)>2&&abs(superCluster.eta)<=2.2?0.0037:0)"
                         "+(?abs(superCluster.eta)>2.2&&abs(superCluster.eta)<=2.3?0.0062:0)"
                         "+(?abs(superCluster.eta)>2.3&&abs(superCluster.eta)<=2.4?0.0130:0)"
                         "+(?abs(superCluster.eta)>2.4?0.1699:0)"
                         )),
                cms.PSet(min=cms.string("-13000"),max=cms.string("0.023"),
                         rhocorr=cms.string("0")),
                cms.PSet(min=cms.string("-13000"),max=cms.string("0.0270"),
                         rhocorr=cms.string("0")),
                cms.PSet(min=cms.string("0.5"),max=cms.string("10"),
                         rhocorr=cms.string("0")),
                )
                 ),
        ),
    )
