import FWCore.ParameterSet.Config as cms
from diphotons.Analysis.highMassMVAs_cff import cShapeMVA_EB, cShapeMVA_EE

tmpKinDiPhotons = cms.EDFilter("DiPhotonCandidateSelector",
                               src = cms.InputTag("flashggDiPhotons"),
                               cut = cms.string(
        "mass > 500"
        " && leadingPhoton.pt > 200 && subLeadingPhoton.pt > 200"
        " && abs(leadingPhoton.superCluster.eta)<2.5 && abs(subLeadingPhoton.superCluster.eta)<2.5 "
        " && ( abs(leadingPhoton.superCluster.eta)<1.4442 || abs(leadingPhoton.superCluster.eta)>1.566)"
        " && ( abs(subLeadingPhoton.superCluster.eta)<1.4442 || abs(subLeadingPhoton.superCluster.eta)>1.566)"),
                               )

kinDiPhotons = cms.EDProducer(
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
        "! passElectronVeto"
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
                cms.PSet(min=cms.string("-13000"),max=cms.string("0.5"),
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
                cms.PSet(min=cms.string("-13000"),max=cms.string("0.5"),
                         rhocorr=cms.string("0")),
                )
                 ),
        ),
    )




hcic4DiPhotons = cms.EDFilter(
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
        "! passElectronVeto"
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
                cms.PSet(min=cms.string("-13000"),max=cms.string("0.5"),
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
                cms.PSet(min=cms.string("-13000"),max=cms.string("0.5"),
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
                cms.PSet(min=cms.string("-13000"),max=cms.string("0.5"),
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
                cms.PSet(min=cms.string("-13000"),max=cms.string("0.5"),
                         rhocorr=cms.string("0")),
                )
                 ),

        ),
    )

## <Cuts cutMin_0="-1.0000000000000000e+30" cutMax_0="1.2762186772110889e+01" cutMin_1="-1.0000000000000000e+30" cutMax_1="3.0300864188972763e+01" cutMin_2="-1.0000000000000000e+30" cutMax_2="2.5671530841299219e+01" cutMin_3="2.8853144968385869e-02" cutMax_3="1.0000000000000000e+30"/>
## <Cuts cutMin_0="-1.0000000000000000e+30" cutMax_0="1.1960475365942637e+01" cutMin_1="-1.0000000000000000e+30" cutMax_1="3.0211805986267645e+01" cutMin_2="-1.0000000000000000e+30" cutMax_2="1.9656277915618631e+01" cutMin_3="-2.3423149176130628e-02" cutMax_3="1.0000000000000000e+30"/>

hmvaDiPhotons = cms.EDFilter(
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
        "! passElectronVeto"
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
                cms.PSet(min=cms.string("-13000"),max=cms.string("0.5"),
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
                cms.PSet(min=cms.string("-13000"),max=cms.string("0.5"),
                         rhocorr=cms.string("0")),
                )
                ),
        ),
    )


