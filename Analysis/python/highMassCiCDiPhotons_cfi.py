import FWCore.ParameterSet.Config as cms

phoEffAreaV1=cms.PSet( var=cms.string("abs(superCluster.eta)"), bins=cms.vdouble(0.,0.9,1.5,2,2.2,3), vals=cms.vdouble(0.21,0.2,0.14,0.22,0.31) )
neuEffAreaV1=cms.PSet( var=cms.string("abs(superCluster.eta)"), bins=cms.vdouble(0.,0.9,1.5,2,2.2,3), vals=cms.vdouble(0.04,0.059,0.05,0.05,0.15) )

phoEffAreaV2=cms.PSet( var=cms.string("abs(superCluster.eta)"), bins=cms.vdouble(0.,0.9,1.5,2,2.2,3), vals=cms.vdouble(0.17,0.14,0.11,0.14,0.22) )
neuEffAreaV2=cms.PSet( var=cms.string("abs(superCluster.eta)"), bins=cms.vdouble(0.,3), vals=cms.vdouble(0.) )

phoEffArea=phoEffAreaV1
neuEffArea=neuEffAreaV1

highMassCiCVariables = cms.vstring(
        "egChargedHadronIso", 
        "egPhotonIso", 
        "egNeutralHadronIso",
        "hadTowOverEm",
        "full5x5_sigmaIetaIeta",
        "passElectronVeto"
        )

highMassCiCVariablesV2=highMassCiCVariables

highMassCiCCutsV0 = cms.VPSet(
        cms.PSet(cut=cms.string("abs(superCluster.eta)<1.5 && r9>0.94"),
                 selection = cms.VPSet(
                cms.PSet(max=cms.string("5.95")),
                cms.PSet(max=cms.string("2.87"), 
                         rhocorr=phoEffAreaV1,
                        ),
                cms.PSet(# no neutra iso cut
                    ),
                cms.PSet(max=cms.string("4.53e-1")),
                cms.PSet(min=cms.string("0.001"), max=cms.string("1.05e-2")),
                cms.PSet(min=cms.string("0.5"))
                 ),
                 ),
        cms.PSet(cut=cms.string("abs(superCluster.eta)<1.5 && r9<=0.94"),
                 selection = cms.VPSet(
                cms.PSet(max=cms.string("7.08")),
                cms.PSet(max=cms.string("5.47"), 
                         rhocorr=phoEffAreaV1
                        ),
                cms.PSet(# no neutra iso cut
                    ),
                cms.PSet(max=cms.string("2.12e-1")),
                cms.PSet(min=cms.string("0.001"), max=cms.string("1.05e-2")),
                cms.PSet(min=cms.string("0.5"))
                ),
                ),
         cms.PSet(cut=cms.string("abs(superCluster.eta)>=1.5 && r9>0.94"),
                  selection = cms.VPSet(
                cms.PSet(max=cms.string("6.10")),
                cms.PSet(max=cms.string("5.98"), 
                         rhocorr=phoEffAreaV1),
                cms.PSet(# no neutra iso cut
                    ),
                cms.PSet(max=cms.string("6.3e-2")),
                cms.PSet(min=cms.string("0.001"), max=cms.string("2.82e-2")),
                cms.PSet(min=cms.string("0.5"))
                 ),
                 ),
        cms.PSet(cut=cms.string("abs(superCluster.eta)>=1.5 && r9<=0.94"),
                 selection = cms.VPSet(
                cms.PSet(max=cms.string("5.07")),
                cms.PSet(max=cms.string("3.44"), 
                         rhocorr=phoEffAreaV1),
                cms.PSet(# no neutra iso cut
                    ),
                cms.PSet(max=cms.string("7.8e-2")),
                cms.PSet(min=cms.string("0.001"), max=cms.string("2.80e-2")),
                cms.PSet(min=cms.string("0.5"))
                ),
                 )
        )

highMassCiCCutsV1 = cms.VPSet(
        cms.PSet(cut=cms.string("abs(superCluster.eta)<1.5 && r9>0.94"),
                 selection = cms.VPSet(
                cms.PSet(max=cms.string("5.")),
                cms.PSet(max=cms.string("1.+0.002*pt"), 
                         rhocorr=phoEffAreaV1,
                        ),
                cms.PSet(# no neutral iso cut
                    ),
                cms.PSet(max=cms.string("5.e-2")),
                cms.PSet(min=cms.string("0.001"), max=cms.string("1.05e-2")),
                cms.PSet(min=cms.string("0.5"))
                 ),
                 ),
        cms.PSet(cut=cms.string("abs(superCluster.eta)<1.5 && r9<=0.94"),
                 selection = cms.VPSet(
                cms.PSet(max=cms.string("5.")),
                cms.PSet(max=cms.string("1.+0.002*pt"), 
                         rhocorr=phoEffAreaV1
                        ),
                cms.PSet(# no neutral iso cut
                    ),
                cms.PSet(max=cms.string("5.e-2")),
                cms.PSet(min=cms.string("0.001"), max=cms.string("1.05e-2")),
                cms.PSet(min=cms.string("0.5"))
                ),
                ),
         cms.PSet(cut=cms.string("abs(superCluster.eta)>=1.5 && r9>0.94"),
                  selection = cms.VPSet(
                cms.PSet(max=cms.string("5.")),
                cms.PSet(max=cms.string("0.002*pt"), 

                         rhocorr=phoEffAreaV1),
                cms.PSet(# no neutral iso cut
                    ),
                cms.PSet(max=cms.string("5.e-2")),
                cms.PSet(min=cms.string("0.001"), max=cms.string("2.82e-2")),
                cms.PSet(min=cms.string("0.5"))
                 ),
                 ),
        cms.PSet(cut=cms.string("abs(superCluster.eta)>=1.5 && r9<=0.94"),
                 selection = cms.VPSet(
                cms.PSet(max=cms.string("5.")),
                cms.PSet(max=cms.string("0.002*pt"), 

                         rhocorr=phoEffAreaV1),
                cms.PSet(# no neutral iso cut
                    ),
                cms.PSet(max=cms.string("5.e-2")),
                cms.PSet(min=cms.string("0.001"), max=cms.string("2.80e-2")),
                cms.PSet(min=cms.string("0.5"))
                ),
                 )
        )

highMassCiCCutsV1SB = cms.VPSet(
        cms.PSet(cut=cms.string("abs(superCluster.eta)<1.5 && r9>0.94"),
                 selection = cms.VPSet(
                cms.PSet(max=cms.string("5.")),
                cms.PSet(max=cms.string("15.+0.002*pt"), 
                         rhocorr=phoEffAreaV1,
                        ),
                cms.PSet(# no neutral iso cut
                    ),
                cms.PSet(max=cms.string("5.e-2")),
                cms.PSet(min=cms.string("0.001"), max=cms.string("1.05e-2")),
                cms.PSet(min=cms.string("0.5"))
                 ),
                 ),
        cms.PSet(cut=cms.string("abs(superCluster.eta)<1.5 && r9<=0.94"),
                 selection = cms.VPSet(
                cms.PSet(max=cms.string("5.")),
                cms.PSet(max=cms.string("15.+0.002*pt"), 
                         rhocorr=phoEffAreaV1
                        ),
                cms.PSet(# no neutral iso cut
                    ),
                cms.PSet(max=cms.string("5.e-2")),
                cms.PSet(min=cms.string("0.001"), max=cms.string("1.05e-2")),
                cms.PSet(min=cms.string("0.5"))
                ),
                ),
         cms.PSet(cut=cms.string("abs(superCluster.eta)>=1.5 && r9>0.94"),
                  selection = cms.VPSet(
                cms.PSet(max=cms.string("5.")),
                cms.PSet(max=cms.string("15.+0.002*pt"), 

                         rhocorr=phoEffAreaV1),
                cms.PSet(# no neutral iso cut
                    ),
                cms.PSet(max=cms.string("5.e-2")),
                cms.PSet(min=cms.string("0.001"), max=cms.string("2.82e-2")),
                cms.PSet(min=cms.string("0.5"))
                 ),
                 ),
        cms.PSet(cut=cms.string("abs(superCluster.eta)>=1.5 && r9<=0.94"),
                 selection = cms.VPSet(
                cms.PSet(max=cms.string("5.")),
                cms.PSet(max=cms.string("15.+0.002*pt"), 

                         rhocorr=phoEffAreaV1),
                cms.PSet(# no neutral iso cut
                    ),
                cms.PSet(max=cms.string("5.e-2")),
                cms.PSet(min=cms.string("0.001"), max=cms.string("2.80e-2")),
                cms.PSet(min=cms.string("0.5"))
                ),
                 )
        )


highMassCiCCutsV2 = cms.VPSet(
        cms.PSet(cut=cms.string("abs(superCluster.eta)<1.5 && r9>0.94"),
                 selection = cms.VPSet(
                cms.PSet(max=cms.string("5.")),
                cms.PSet(max=cms.string("0.5+4.5e-3*pt"), 
                         rhocorr=phoEffAreaV2,
                        ),
                cms.PSet(# no neutral iso cut
                    ),
                cms.PSet(max=cms.string("5.e-2")),
                cms.PSet(min=cms.string("0.001"), max=cms.string("1.05e-2")),
                cms.PSet(min=cms.string("0.5"))
                 ),
                 ),
        cms.PSet(cut=cms.string("abs(superCluster.eta)<1.5 && r9<=0.94"),
                 selection = cms.VPSet(
                cms.PSet(max=cms.string("5.")),
                cms.PSet(max=cms.string("0.5+4.5e-3*pt"), 
                         rhocorr=phoEffAreaV2
                        ),
                cms.PSet(# no neutral iso cut
                    ),
                cms.PSet(max=cms.string("5.e-2")),
                cms.PSet(min=cms.string("0.001"), max=cms.string("1.05e-2")),
                cms.PSet(min=cms.string("0.5"))
                ),
                ),
         cms.PSet(cut=cms.string("abs(superCluster.eta)>=1.5 && r9>0.94"),
                  selection = cms.VPSet(
                cms.PSet(max=cms.string("5.")),
                cms.PSet(max=cms.string("-0.5+3e-3*pt"), 

                         rhocorr=phoEffAreaV2),
                cms.PSet(# no neutral iso cut
                    ),
                cms.PSet(max=cms.string("5.e-2")),
                cms.PSet(min=cms.string("0.001"), max=cms.string("2.82e-2")),
                cms.PSet(min=cms.string("0.5"))
                 ),
                 ),
        cms.PSet(cut=cms.string("abs(superCluster.eta)>=1.5 && r9<=0.94"),
                 selection = cms.VPSet(
                cms.PSet(max=cms.string("5.")),
                cms.PSet(max=cms.string("-0.5+3e-3*pt"), 

                         rhocorr=phoEffAreaV2),
                cms.PSet(# no neutral iso cut
                    ),
                cms.PSet(max=cms.string("5.e-2")),
                cms.PSet(min=cms.string("0.001"), max=cms.string("2.80e-2")),
                cms.PSet(min=cms.string("0.5"))
                ),
                 )
        )

highMassCiCCutsV2SB = cms.VPSet(
        cms.PSet(cut=cms.string("abs(superCluster.eta)<1.5 && r9>0.94"),
                 selection = cms.VPSet(
                cms.PSet(max=cms.string("5.")),
                cms.PSet(max=cms.string("15+4.5e-3*pt"), 
                         rhocorr=phoEffAreaV2,
                        ),
                cms.PSet(# no neutral iso cut
                    ),
                cms.PSet(max=cms.string("5.e-2")),
                cms.PSet(min=cms.string("0.001"), max=cms.string("1.05e-2")),
                cms.PSet(min=cms.string("0.5"))
                 ),
                 ),
        cms.PSet(cut=cms.string("abs(superCluster.eta)<1.5 && r9<=0.94"),
                 selection = cms.VPSet(
                cms.PSet(max=cms.string("5.")),
                cms.PSet(max=cms.string("15+4.5e-3*pt"), 
                         rhocorr=phoEffAreaV2
                        ),
                cms.PSet(# no neutral iso cut
                    ),
                cms.PSet(max=cms.string("5.e-2")),
                cms.PSet(min=cms.string("0.001"), max=cms.string("1.05e-2")),
                cms.PSet(min=cms.string("0.5"))
                ),
                ),
         cms.PSet(cut=cms.string("abs(superCluster.eta)>=1.5 && r9>0.94"),
                  selection = cms.VPSet(
                cms.PSet(max=cms.string("5.")),
                cms.PSet(max=cms.string("15+3e-3*pt"), 

                         rhocorr=phoEffAreaV2),
                cms.PSet(# no neutral iso cut
                    ),
                cms.PSet(max=cms.string("5.e-2")),
                cms.PSet(min=cms.string("0.001"), max=cms.string("2.82e-2")),
                cms.PSet(min=cms.string("0.5"))
                 ),
                 ),
        cms.PSet(cut=cms.string("abs(superCluster.eta)>=1.5 && r9<=0.94"),
                 selection = cms.VPSet(
                cms.PSet(max=cms.string("5.")),
                cms.PSet(max=cms.string("15+3e-3*pt"), 

                         rhocorr=phoEffAreaV2),
                cms.PSet(# no neutral iso cut
                    ),
                cms.PSet(max=cms.string("5.e-2")),
                cms.PSet(min=cms.string("0.001"), max=cms.string("2.80e-2")),
                cms.PSet(min=cms.string("0.5"))
                ),
                 )
        )



highMassCiCCuts = highMassCiCCutsV1
highMassCiCCutsSB = highMassCiCCutsV1SB

highMassCiCDiPhotons = cms.EDFilter(
    "GenericDiPhotonCandidateSelector",
    src = cms.InputTag("kinDiPhotons"),
    rho = cms.InputTag("fixedGridRhoAll"),
    cut = cms.string(
        "    (leadingPhoton.r9>0.8||leadingPhoton.egChargedHadronIso<20||leadingPhoton.egChargedHadronIso/leadingPhoton.pt<0.3)"
        " && (subLeadingPhoton.r9>0.8||subLeadingPhoton.egChargedHadronIso<20||subLeadingPhoton.egChargedHadronIso/subLeadingPhoton.pt<0.3)" 
        " && leadingPhoton.egChargedHadronIso < 15  && subLeadingPhoton.egChargedHadronIso < 15" ## preselect in chargedIso
        )
    ,
    variables = highMassCiCVariables,
    categories = highMassCiCCuts,
    )

highMassCiCDiPhotonsSB = highMassCiCDiPhotons.clone( categories=highMassCiCCutsSB )

highMassCiCDiPhotonsV2 = cms.EDFilter(
    "GenericDiPhotonCandidateSelector",
    src = cms.InputTag("kinDiPhotons"),
    rho = cms.InputTag("fixedGridRhoAll"),
    cut = cms.string(
        "    (leadingPhoton.r9>0.8||leadingPhoton.egChargedHadronIso<20||leadingPhoton.egChargedHadronIso/leadingPhoton.pt<0.3)"
        " && (subLeadingPhoton.r9>0.8||subLeadingPhoton.egChargedHadronIso<20||subLeadingPhoton.egChargedHadronIso/subLeadingPhoton.pt<0.3)" 
        " && leadingPhoton.egChargedHadronIso < 15  && subLeadingPhoton.egChargedHadronIso < 15" ## preselect in chargedIso
        )
    ,
    variables = highMassCiCVariablesV2,
    categories = highMassCiCCutsV2,
    )

highMassCiCDiPhotonsSBV2 = highMassCiCDiPhotons.clone( categories=highMassCiCCutsV2SB )
