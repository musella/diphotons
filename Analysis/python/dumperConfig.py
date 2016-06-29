def makeOneLegInputs(label,obj,inputs):
    out = []
    for inp in inputs:
        if type(inp) == str: 
            name, expr = inp, inp
        else:
            expr, name = inp
        if not name.startswith("_"): name = name[0].capitalize()+name[1:]
        name = "%s%s"  % ( label, name )        
        expr = "%s.%s" % ( obj,   expr )
        ## print expr[14]
        out.append( "%s := %s" % (name,expr) )
        print out[-1]
    return out

def addPreshowerEnergy(variables):
    variables.extend( makeOneLegInputs("lead","leadingPhoton",[("superCluster.preshowerEnergy","scPreshowerEnergy"),
                                                               ("superCluster.rawEnergy","scRawEnergy"),
                                                               ("superCluster.preshowerEnergyPlane1","scPreshowerEnergyPlane1"),
                                                               ("superCluster.preshowerEnergyPlane2","scPreshowerEnergyPlane2")]) ),
    variables.extend( makeOneLegInputs("sublead","subLeadingPhoton",[("superCluster.preshowerEnergy","scPreshowerEnergy"),
                                                                     ("superCluster.rawEnergy","scRawEnergy"),
                                                                     ("superCluster.preshowerEnergyPlane1","preshowerEnergyPlane1"),
                                                                     ("superCluster.preshowerEnergyPlane2","preshowerEnergyPlane2")]) ),
    

def addRegressionInput(variables)
    regInputs = [
        ("superCluster.clustersSize","scClustersSize"),
        ("superCluster.seed.energy","scSeedEnergy"),
        ("superCluster.energy","scEnergy"),
        
        # ("? hasMatchedGenPhoton ? matchedGenPhoton.energy : 0","etrue"),
        
        ("userInt('seedRecoFlag')","seedRecoFlag"),
        
        ## cluster shapes
        "e1x5",           "full5x5_e1x5",           
        "e2x5",           "full5x5_e2x5",           
        "e3x3",           "full5x5_e3x3",           
        "e5x5",           "full5x5_e5x5",           
        "maxEnergyXtal",  "full5x5_maxEnergyXtal",  
        "sigmaIetaIeta",  "full5x5_sigmaIetaIeta",  
        "r1x5",           "full5x5_r1x5",           
        "r2x5",           "full5x5_r2x5",           
        "r9",             "full5x5_r9",             
        "eMax","e2nd","eTop","eBottom","eLeft","eRight",
        "iEta","iPhi","cryEta","cryPhi",
        
        ## more cluster shapes
        ("e2x5right" ,"e2x5Right"  ),
        ("e2x5left"  ,"e2x5Left"   ),
        ("e2x5top"   ,"e2x5Top"    ),
        ("e2x5bottom","e2x5Bottom" ),
        ("e2x5max"   ,"e2x5Max"    ),
        ("e1x3"      ,"e1x3"       ),
        ("s4"        ,"s4"         ),
        
        ("esEffSigmaRR","sigmaRR"),
        ("spp","covarianceIphiIphi"),
        ("sep","covarianceIetaIphi"),
        ("superCluster.etaWidth","etaWidth"),("superCluster.phiWidth","phiWidth"),
        
        ("checkStatusFlag('kSaturated')","kSaturated"),("checkStatusFlag('kWeird')","kWeird"),
        ]
    
    variables.extend( makeOneLegInputs("lead","leadingPhoton",regInputs))
    variables.extend( makeOneLegInputs("sublead","subLeadingPhoton",regInputs))



def addRandomCones(variables,histograms):
    variables.extend( [
                "leadRndConeChIso := leadingView.extraChIsoWrtChoosenVtx('rnd03')",
                "leadRndConeChIso0 := leadingView.extraChIsoWrtChoosenVtx('rnd03_0')",
                "leadRndConeChIso1 := leadingView.extraChIsoWrtChoosenVtx('rnd03_1')",
                "leadRndConeChIso2 := leadingView.extraChIsoWrtChoosenVtx('rnd03_2')",
                "leadRndConeChIso3 := leadingView.extraChIsoWrtChoosenVtx('rnd03_3')",
                "leadRndConeChIso4 := leadingView.extraChIsoWrtChoosenVtx('rnd03_4')",
                "leadRndConeChIso5 := leadingView.extraChIsoWrtChoosenVtx('rnd03_5')",
                "leadRndConeChIso6 := leadingView.extraChIsoWrtChoosenVtx('rnd03_6')",
                "leadRndConeChIso7 := leadingView.extraChIsoWrtChoosenVtx('rnd03_7')",
                "leadRndConeChIso8 := leadingView.extraChIsoWrtChoosenVtx('rnd03_8')",
                
                "subleadRndConeChIso := subLeadingView.extraChIsoWrtChoosenVtx('rnd03')",
                "subleadRndConeChIso0 := subLeadingView.extraChIsoWrtChoosenVtx('rnd03_0')",
                "subleadRndConeChIso1 := subLeadingView.extraChIsoWrtChoosenVtx('rnd03_1')",
                "subleadRndConeChIso2 := subLeadingView.extraChIsoWrtChoosenVtx('rnd03_2')",
                "subleadRndConeChIso3 := subLeadingView.extraChIsoWrtChoosenVtx('rnd03_3')",
                "subleadRndConeChIso4 := subLeadingView.extraChIsoWrtChoosenVtx('rnd03_4')",
                "subleadRndConeChIso5 := subLeadingView.extraChIsoWrtChoosenVtx('rnd03_5')",
                "subleadRndConeChIso6 := subLeadingView.extraChIsoWrtChoosenVtx('rnd03_6')",
                "subleadRndConeChIso7 := subLeadingView.extraChIsoWrtChoosenVtx('rnd03_7')",
                "subleadRndConeChIso8 := subLeadingView.extraChIsoWrtChoosenVtx('rnd03_8')",
                ])
    
    histograms.extend(["leadRndConeChIso0>>leadRndConeChIso(120,-10,50)",
                       "leadRndConeChIso1>>leadRndConeChIso(120,-10,50)",
                       "leadRndConeChIso2>>leadRndConeChIso(120,-10,50)",
                       "leadRndConeChIso3>>leadRndConeChIso(120,-10,50)",
                       "leadRndConeChIso4>>leadRndConeChIso(120,-10,50)",
                       "leadRndConeChIso5>>leadRndConeChIso(120,-10,50)",
                       "leadRndConeChIso6>>leadRndConeChIso(120,-10,50)",
                       "leadRndConeChIso7>>leadRndConeChIso(120,-10,50)",
                       "leadRndConeChIso8>>leadRndConeChIso(120,-10,50)",
                       
                       "subleadRndConeChIso0>>subleadRndConeChIso(120,-10,50)",
                       "subleadRndConeChIso1>>subleadRndConeChIso(120,-10,50)",
                       "subleadRndConeChIso2>>subleadRndConeChIso(120,-10,50)",
                       "subleadRndConeChIso3>>subleadRndConeChIso(120,-10,50)",
                       "subleadRndConeChIso4>>subleadRndConeChIso(120,-10,50)",
                       "subleadRndConeChIso5>>subleadRndConeChIso(120,-10,50)",
                       "subleadRndConeChIso6>>subleadRndConeChIso(120,-10,50)",
                       "subleadRndConeChIso7>>subleadRndConeChIso(120,-10,50)",
                       "subleadRndConeChIso8>>subleadRndConeChIso(120,-10,50)",
                       
                       "leadRndConeChIso0>>phoRndConeChIso(120,-10,50)",
                       "leadRndConeChIso1>>phoRndConeChIso(120,-10,50)",
                       "leadRndConeChIso2>>phoRndConeChIso(120,-10,50)",
                       "leadRndConeChIso3>>phoRndConeChIso(120,-10,50)",
                       "leadRndConeChIso4>>phoRndConeChIso(120,-10,50)",
                       "leadRndConeChIso5>>phoRndConeChIso(120,-10,50)",
                       "leadRndConeChIso6>>phoRndConeChIso(120,-10,50)",
                       "leadRndConeChIso7>>phoRndConeChIso(120,-10,50)",
                       "leadRndConeChIso8>>phoRndConeChIso(120,-10,50)",
                       
                       "subleadRndConeChIso0>>phoRndConeChIso(120,-10,50)",
                       "subleadRndConeChIso1>>phoRndConeChIso(120,-10,50)",
                       "subleadRndConeChIso2>>phoRndConeChIso(120,-10,50)",
                       "subleadRndConeChIso3>>phoRndConeChIso(120,-10,50)",
                       "subleadRndConeChIso4>>phoRndConeChIso(120,-10,50)",
                       "subleadRndConeChIso5>>phoRndConeChIso(120,-10,50)",
                       "subleadRndConeChIso6>>phoRndConeChIso(120,-10,50)",
                       "subleadRndConeChIso7>>phoRndConeChIso(120,-10,50)",
                       "subleadRndConeChIso8>>phoRndConeChIso(120,-10,50)",
                       ])
    
    variablesSinglePho.extend(["phoRndConeChIso := extraChgIsoWrtVtx0('rnd03')",
                               "phoRndConeChIso0 := extraChgIsoWrtVtx0('rnd03_0')",
                               "phoRndConeChIso1 := extraChgIsoWrtVtx0('rnd03_1')",
                               "phoRndConeChIso2 := extraChgIsoWrtVtx0('rnd03_2')",
                               "phoRndConeChIso3 := extraChgIsoWrtVtx0('rnd03_3')",
                               "phoRndConeChIso4 := extraChgIsoWrtVtx0('rnd03_4')",
                               "phoRndConeChIso5 := extraChgIsoWrtVtx0('rnd03_5')",
                               "phoRndConeChIso6 := extraChgIsoWrtVtx0('rnd03_6')",
                               "phoRndConeChIso7 := extraChgIsoWrtVtx0('rnd03_7')",
                               "phoRndConeChIso8 := extraChgIsoWrtVtx0('rnd03_8')",
                               ])
    
    histogramsSinglePho.extend(["phoRndConeChIso0>>phoRndConeChIso(120,-10,50)",
                                "phoRndConeChIso1>>phoRndConeChIso(120,-10,50)",
                                "phoRndConeChIso2>>phoRndConeChIso(120,-10,50)",
                                "phoRndConeChIso3>>phoRndConeChIso(120,-10,50)",
                                "phoRndConeChIso4>>phoRndConeChIso(120,-10,50)",
                                "phoRndConeChIso5>>phoRndConeChIso(120,-10,50)",
                                "phoRndConeChIso6>>phoRndConeChIso(120,-10,50)",
                                "phoRndConeChIso7>>phoRndConeChIso(120,-10,50)",
                                "phoRndConeChIso8>>phoRndConeChIso(120,-10,50)"
                                ])


def getDefaultConfig():

    variables=["mass","pt","rapidity","eta",
               "vertexZ  := vtx.z", 
               "vertexId := vtx.key",
               ## "satRegressedMass := sqrt( (leadingPhoton.energyAtStep('satRegressedEnergy','initial')*subLeadingPhoton.energyAtStep('satRegressedEnergy','initial')) / (leadingPhoton.energy*subLeadingPhoton.energy) ) * genP4.mass",
               ## "regressedMass := sqrt( (leadingPhoton.energyAtStep('regressedEnergy')*subLeadingPhoton.energyAtStep('regressedEnergy')) / (leadingPhoton.energy*subLeadingPhoton.energy) ) * genP4.mass",
               "genMass := genP4.mass",
               ### "leadSatRegressedEnergy := leadingPhoton.userFloat('satRegressedEnergy')",
               ### "subLeadSatRegressedEnergy := subLeadingPhoton.userFloat('satRegressedEnergy')",
               ### "leadRegressedEnergy := leadingPhoton.userFloat('regressedEnergy')",
               ### "subLeadRegressedEnergy := subLeadingPhoton.userFloat('regressedEnergy')",
               "leadInitialEnergy := leadingPhoton.energyAtStep('initial')",
               "subLeadInitialEnergy := subLeadingPhoton.energyAtStep('initial')",
               "leadEnergy := leadingPhoton.p4.energy",
               "subLeadEnergy := subLeadingPhoton.p4.energy",
               "lead_5x5_Energy := leadingPhoton.full5x5_e5x5",
               "subLead_5x5_Energy := subLeadingPhoton.full5x5_e5x5",
               "mass_5x5 := mass*sqrt(leadingPhoton.full5x5_e5x5*subLeadingPhoton.full5x5_e5x5/(leadingPhoton.p4.energy*subLeadingPhoton.p4.energy))",
               "leadIsSat := leadingPhoton.checkStatusFlag('kSaturated')",
               "subLeadIsSat := subLeadingPhoton.checkStatusFlag('kSaturated')",
               "leadIsWeird := leadingPhoton.checkStatusFlag('kWeird')",
               "subLeadIsWeird := subLeadingPhoton.checkStatusFlag('kWeird')",
               "genLeadPt := ?leadingPhoton.hasMatchedGenPhoton?leadingPhoton.matchedGenPhoton.pt:0",
               "genSubLeadPt := ?subLeadingPhoton.hasMatchedGenPhoton?subLeadingPhoton.matchedGenPhoton.pt:0",
               "deltaEta                 := abs( leadingPhoton.eta - subLeadingPhoton.eta )",
               "cosDeltaPhi              := cos( leadingPhoton.phi - subLeadingPhoton.phi )",
               "leadPt                   :=leadingPhoton.pt",
               "subleadPt                :=subLeadingPhoton.pt",
               "leadEta                  :=leadingPhoton.eta",
               "subleadEta               :=subLeadingPhoton.eta",
               "leadR9                   :=leadingPhoton.r9",
               "subleadR9                :=subLeadingPhoton.r9",
               "leadScEta                :=leadingPhoton.superCluster.eta",
               "subleadScEta             :=subLeadingPhoton.superCluster.eta",
               "leadPhi                  :=leadingPhoton.phi",
               "subleadPhi               :=subLeadingPhoton.phi",
               "minR9                    :=min(leadingPhoton.r9,subLeadingPhoton.r9)",
               "maxEta                   :=max(abs(leadingPhoton.superCluster.eta),abs(subLeadingPhoton.superCluster.eta))",
               
               "leadBlockChIso   := leadingView.pfChIso03WrtChosenVtx", 
               "leadBlockPhoIso  := leadingPhoton.pfPhoIso03", 
               
               "leadPhoIsoEA :=  map( abs(leadingPhoton.superCluster.eta) :: 0.,0.9,1.5,2.0,2.2,3. :: 0.21,0.2,0.14,0.22,0.31 )",
               "subleadPhoIsoEA :=  map( abs(subLeadingPhoton.superCluster.eta) :: 0.,0.9,1.5,2.0,2.2,3. :: 0.21,0.2,0.14,0.22,0.31 )",
               
               "leadMatchType            :=leadingPhoton.genMatchType",
               "leadGenIso               :=?leadingPhoton.hasUserFloat('genIso')?leadingPhoton.userFloat('genIso'):0",
               "subleadMatchType         :=subLeadingPhoton.genMatchType",
               "subleadGenIso            :=?subLeadingPhoton.hasUserFloat('genIso')?subLeadingPhoton.userFloat('genIso'):0",
               
               "leadChIso   := leadingPhoton.egChargedHadronIso", 
               "leadPhoIso  := leadingPhoton.egPhotonIso", 
               "leadNeutIso := leadingPhoton.egNeutralHadronIso",
               "leadHoE     := leadingPhoton.hadTowOverEm",
               "leadSigmaIeIe := leadingPhoton.full5x5_sigmaIetaIeta",
               "leadPixSeed  := leadingPhoton.hasPixelSeed",
               "leadPassEleVeto := leadingPhoton.passElectronVeto",
           
           
               ## "subleadBlockChIso   := subLeadingView.pfChIso03WrtChosenVtx", 
               "subleadBlockPhoIso  := subLeadingPhoton.pfPhoIso03", 
               ## "subleadRndConePhoIso:= subLeadingPhoton.extraPhoIso('rnd03')",
               
               "subleadChIso   := subLeadingPhoton.egChargedHadronIso", 
               "subleadPhoIso  := subLeadingPhoton.egPhotonIso", 
               "subleadNeutIso := subLeadingPhoton.egNeutralHadronIso",
               "subleadHoE     := subLeadingPhoton.hadTowOverEm",
               "subleadSigmaIeIe := subLeadingPhoton.full5x5_sigmaIetaIeta",
               "subleadPixSeed := subLeadingPhoton.hasPixelSeed",
               "subleadPassEleVeto := subLeadingPhoton.passElectronVeto",
               ]
    
    histograms=["mass>>mass(1500,0,15000)",
                "mass>>lowmass(560,60,200)",
                "genMass>>genmass(1500,0,15000)",            
                "pt>>pt(200,0,200)",
                "rapidity>>rapidity(200,-5,5)",
                "deltaEta>>deltaEta(200,0,5)",
                "cosDeltaPhi>>cosDeltaPhi(200,0,1)",
                "global.rho>>rho(20,0,50)",
                "global.nvtx>>nvtx(51,0.5,50.5)",
                
                ### "global.mht60>>mht60(1500,0,15000)",
                ### "global.mht30>>mht30(1500,0,15000)",
                ### "global.metPt>>met(200,0,200)",
                ### "global.sumEt>>sumEt(1500,0,15000)",
                
                "leadPt>>phoPt(150,0,3000)",
                "subleadPt>>phoPt(150,0,3000)",
                "leadR9>>leadR9(110,0,1.1)",
                "subleadR9>>subleadR9(110,0,1.1)",
                "leadR9>>phoR9(110,0,1.1)",
                "subleadR9>>phoR9(110,0,1.1)",
                
                "leadPt>>leadPt(200,0,800)",
                "subleadPt>>subleadPt(200,0,800)",
                "leadEta>>leadEta(55,-2.75,2.75)",
                "subleadEta>>subleadEta(55,-2.75,2.75)",
                
                "leadBlockChIso>>leadBlockChIso(120,-10,50)",
                "leadBlockPhoIso>>leadBlockPhoIso(120,-10,50)",
                "leadChIso>>leadChIso(120,-10,50)",
                "leadPhoIso>>leadPhoIso(120,-10,50)",
                "leadNeutIso>>leadNeutIso(120,-10,50)",
                "leadHoE>>leadHoE(40,0,0.2)",
                "leadSigmaIeIe>>leadSigmaIeIe(320,0,3.2e-2)",
                "leadPixSeed>>leadPixSeed(2,-0.5,1.5)",
                "leadPassEleVeto>>leadPassEleVeto(2,-0.5,1.5)",
                
                "subleadBlockChIso>>subleadBlockChIso(120,-10,50)",
                "subleadBlockPhoIso>>subleadBlockPhoIso(120,-10,50)",
                "subleadChIso>>subleadChIso(120,-10,50)",
                "subleadPhoIso>>subleadPhoIso(120,-10,50)",
                "subleadNeutIso>>subleadNeutIso(120,-10,50)",
                "subleadHoE>>subleadHoE(40,0,0.2)",
                "subleadSigmaIeIe>>subleadSigmaIeIe(320,0,3.2e-2)",
                "subleadPixSeed>>subleadPixSeed(2,-0.5,1.5)",
                "subleadPassEleVeto>>subleadPassEleVeto(2,-0.5,1.5)",
                
                
                "leadChIso>>phoChIso(120,-10,50)",
                "subleadChIso>>phoChIso(120,-10,50)",
                "leadPhoIso>>phoPhoIso(120,-10,50)",
                "subleadPhoIso>>phoPhoIso(120,-10,50)",
                "leadSigmaIeIe>>phoSigmaIeIe(320,0,3.2e-2)",
                "subleadSigmaIeIe>>phoSigmaIeIe(320,0,3.2e-2)",
                "leadHoE>>phoHoE(40,0,0.2)",                                   
                "subleadHoE>>phoHoE(40,0,0.2)",                                   
                "leadPassEleVeto>>phoPassEleVeto(2,-0.5,1.5)",
                "subleadPassEleVeto>>phoPassEleVeto(2,-0.5,1.5)",
                
                "subleadPt:leadPt>>ptSubVsLead(145,100,3000:145,100,3000)",
                "minR9>>minR9(110,0,1.1)",
                "maxEta>>maxEta(250,0,2.5)"
                ]
    
    
    variablesSinglePho=[
        "phoPt                   :=pt",
        "genPt                   :=?hasMatchedGenPhoton?matchedGenPhoton.pt:0",
        "phoEta                  :=eta",
        "phoR9                   :=r9",
        "phoScEta                :=superCluster.eta",
        "phoPhi                  :=phi",
        
        "phoBlockChIso   := pfChgIso03WrtVtx0", 
        
        "phoBlockPhoIso  := pfPhoIso03", 
        ## "phoRndConePhoIso:= extraPhoIso('rnd03')",
        
        "phoPhoIsoEA :=  map( abs(superCluster.eta) :: 0.,0.9,1.5,2.0,2.2,3. :: 0.21,0.2,0.14,0.22,0.31 )",
        
        "phoMatchType            :=genMatchType",
        "phoGenIso               :=?hasUserFloat('genIso')?userFloat('genIso'):0",
        "phoChIso   := egChargedHadronIso", 
        "phoPhoIso  := egPhotonIso", 
        "phoNeutIso := egNeutralHadronIso",
        "phoHoE     := hadTowOverEm",
        "phoSigmaIeIe := full5x5_sigmaIetaIeta",
        "phoSigmaIpIp := sqrt(sipip)",
        "eMax","e2nd","eTop","eBottom","eLeft","eRight",
        "phoPixSeed  := hasPixelSeed",
        "phoPassEleVeto := passElectronVeto",
        ]
    
    
    histogramsSinglePho = [
        "phoPt>>phoPt(145,100,3000)",
        "genPt>>phoPt(145,100,3000)",
        "phoEta>>phoEta(55,-2.75,2.75)",
        "phoPhi>>phoPhi(65,-3.25,3.25)",
        
        "phoBlockChIso>>phoBlockChIso(120,-10,50)",
        "phoBlockPhoIso>>phoBlockPhoIso(120,-10,50)",
        "phoChIso>>phoChIso(120,-10,50)",
        "phoPhoIso>>phoPhoIso(120,-10,50)",
        "phoNeutIso>>phoNeutIso(120,-10,50)",
        "phoHoE>>phoHoE(40,0,0.2)",
        "phoSigmaIeIe>>phoSigmaIeIe(50,0,5.e-2)",
        "phoPixSeed>>phoPixSeed(2,-0.5,1.5)",
        "phoScEta:phoPhi>>phoEtaVsPhi(65,-3.25,3.25:55,-2.75,2.75)"
        ]

    return variables, histograms, variablesSinglePho, histogramsSinglePho

