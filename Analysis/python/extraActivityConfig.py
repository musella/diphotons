import FWCore.ParameterSet.Config as cms

def addGloabalFloat(globalVariables,process,producer,name,expr):    
    getattr(process,producer).variables.append( cms.PSet(tag=cms.untracked.string(name),quantity=cms.untracked.string(expr)) )
    setattr(globalVariables.extraFloats,name,cms.InputTag(producer,name))

def bookCandViewNtProducer(process,name,collection):    
    setattr(process,name,cms.EDProducer(
            "CandViewNtpProducer", 
            src = cms.InputTag(collection), lazyParser = cms.untracked.bool(True),
            variables = cms.VPSet()
            )
            )

def bookJets(process):
    process.flashggUnpackedJets = cms.EDProducer("FlashggVectorVectorJetUnpacker",
                                                 JetsTag = cms.InputTag("flashggFinalJets"),
                                                 NCollections = cms.uint32(8)
                                                 )
    process.selectedJsets60 = cms.EDFilter("FlashggJetSelector",
                                           src=cms.InputTag("flashggUnpackedJets","0"),
                                           cut=cms.string("pt>60 && abs(eta)<2.5"),
                                           )
    
    process.selectedJsets30 = cms.EDFilter("FlashggJetSelector",
                                           src=cms.InputTag("flashggUnpackedJets","0"),
                                           cut=cms.string("pt>30 && abs(eta)<2.5"),
                                           )
    
    process.selectedJsets60Fwd = cms.EDFilter("FlashggJetSelector",
                                              src=cms.InputTag("flashggUnpackedJets","0"),
                                              cut=cms.string("pt>30 && abs(eta)<4.7"),
                                              )

def addGlobalVariables(process,diphotonDumper,diphotonCollection="cicDiPhotons"):
    
    bookJets(process)
    
    process.MHT60 = cms.EDProducer("MyMHTProducer",src=cms.InputTag("selectedJsets60"))
    process.MHT30 = cms.EDProducer("MyMHTProducer",src=cms.InputTag("selectedJsets30"))
    process.MHT60Clean = cms.EDProducer("MyMHTProducer",src=cms.InputTag("selectedJsets60"),veto=cms.InputTag(diphotonCollection))
    process.MHT30Clean = cms.EDProducer("MyMHTProducer",src=cms.InputTag("selectedJsets30"),veto=cms.InputTag(diphotonCollection))
    process.dijet60Clean = cms.EDProducer("MyMHTProducer",src=cms.InputTag("selectedJsets60"),maxCand=cms.int32(2),veto=cms.InputTag(diphotonCollection))
    process.dijet30Clean = cms.EDProducer("MyMHTProducer",src=cms.InputTag("selectedJsets30"),maxCand=cms.int32(2),veto=cms.InputTag(diphotonCollection))
    
    bookCandViewNtProducer(process,"mht60","MHT60")
    bookCandViewNtProducer(process,"mht30","MHT30")
    bookCandViewNtProducer(process,"mht60clean","MHT60Clean")
    bookCandViewNtProducer(process,"mht30clean","MHT30Clean")
    bookCandViewNtProducer(process,"dijet60clean","dijet60Clean")
    bookCandViewNtProducer(process,"dijet30clean","dijet30Clean")
    
    addGloabalFloat(diphotonDumper.globalVariables,process,"mht60","mht60Mass","mass")
    addGloabalFloat(diphotonDumper.globalVariables,process,"mht60","mht60Pt","pt")
    addGloabalFloat(diphotonDumper.globalVariables,process,"mht60","mht60Rapidity","rapidity")
    addGloabalFloat(diphotonDumper.globalVariables,process,"mht60","mht60Phi","phi")
    
    addGloabalFloat(diphotonDumper.globalVariables,process,"mht60clean","mht60CleanMass","mass")
    addGloabalFloat(diphotonDumper.globalVariables,process,"mht60clean","mht60CleanPt","pt")
    addGloabalFloat(diphotonDumper.globalVariables,process,"mht60clean","mht60CleanRapidity","rapidity")
    addGloabalFloat(diphotonDumper.globalVariables,process,"mht60clean","mht60CleanPhi","phi")
    addGloabalFloat(diphotonDumper.globalVariables,process,"mht60clean","nJets60","numberOfDaughters")
    
    addGloabalFloat(diphotonDumper.globalVariables,process,"dijet60clean","dijet60CleanMass","mass")
    addGloabalFloat(diphotonDumper.globalVariables,process,"dijet60clean","dijet60CleanPt","pt")
    addGloabalFloat(diphotonDumper.globalVariables,process,"dijet60clean","dijet60CleanRapidity","rapidity")
    addGloabalFloat(diphotonDumper.globalVariables,process,"dijet60clean","dijet60CleanPhi","phi")
    
    addGloabalFloat(diphotonDumper.globalVariables,process,"dijet30clean","dijet30CleanMass","mass")
    addGloabalFloat(diphotonDumper.globalVariables,process,"dijet30clean","dijet30CleanPt","pt")
    addGloabalFloat(diphotonDumper.globalVariables,process,"dijet30clean","dijet30CleanRapidity","rapidity")
    addGloabalFloat(diphotonDumper.globalVariables,process,"dijet30clean","dijet30CleanPhi","phi")
    
    addGloabalFloat(diphotonDumper.globalVariables,process,"mht30","mht30Mass","mass")
    addGloabalFloat(diphotonDumper.globalVariables,process,"mht30","mht30Pt","pt")
    addGloabalFloat(diphotonDumper.globalVariables,process,"mht30","mht30Rapidity","rapidity")
    addGloabalFloat(diphotonDumper.globalVariables,process,"mht30","mht30Phi","phi")
    addGloabalFloat(diphotonDumper.globalVariables,process,"mht30","nJets30","numberOfDaughters")
    
    addGloabalFloat(diphotonDumper.globalVariables,process,"mht30clean","mht30CleanMass","mass")
    addGloabalFloat(diphotonDumper.globalVariables,process,"mht30clean","mht30CleanPt","pt")
    addGloabalFloat(diphotonDumper.globalVariables,process,"mht30clean","mht30CleanRapidity","rapidity")
    addGloabalFloat(diphotonDumper.globalVariables,process,"mht30clean","mht30CleanPhi","phi")
    addGloabalFloat(diphotonDumper.globalVariables,process,"mht30clean","nJets30","numberOfDaughters")
    
    addGloabalFloat(diphotonDumper.globalVariables,process,"mht30clean","jet1Btag","?numberOfDaughters>0?daughter(0).bDiscriminator('combinedInclusiveSecondaryVertexV2BJetTags'):0")
    addGloabalFloat(diphotonDumper.globalVariables,process,"mht30clean","jet1Pt","?numberOfDaughters>0?daughter(0).pt:0")
    addGloabalFloat(diphotonDumper.globalVariables,process,"mht30clean","jet1Eta","?numberOfDaughters>0?daughter(0).eta:0")
    addGloabalFloat(diphotonDumper.globalVariables,process,"mht30clean","jet1Phi","?numberOfDaughters>0?daughter(0).phi:0")
    
    addGloabalFloat(diphotonDumper.globalVariables,process,"mht30clean","jet2Btag","?numberOfDaughters>1?daughter(1).bDiscriminator('combinedInclusiveSecondaryVertexV2BJetTags'):0")
    addGloabalFloat(diphotonDumper.globalVariables,process,"mht30clean","jet2Pt","?numberOfDaughters>1?daughter(1).pt:0")
    addGloabalFloat(diphotonDumper.globalVariables,process,"mht30clean","jet2Eta","?numberOfDaughters>1?daughter(1).eta:0")
    addGloabalFloat(diphotonDumper.globalVariables,process,"mht30clean","jet2Phi","?numberOfDaughters>1?daughter(1).phi:0")
    
    addGloabalFloat(diphotonDumper.globalVariables,process,"mht30clean","jet3Btag","?numberOfDaughters>2?daughter(2).bDiscriminator('combinedInclusiveSecondaryVertexV2BJetTags'):0")
    addGloabalFloat(diphotonDumper.globalVariables,process,"mht30clean","jet3Pt","?numberOfDaughters>2?daughter(2).pt:0")
    addGloabalFloat(diphotonDumper.globalVariables,process,"mht30clean","jet3Eta","?numberOfDaughters>2?daughter(2).eta:0")
    addGloabalFloat(diphotonDumper.globalVariables,process,"mht30clean","jet3Phi","?numberOfDaughters>2?daughter(2).phi:0")
    
    addGloabalFloat(diphotonDumper.globalVariables,process,"mht30clean","jet4Btag","?numberOfDaughters>3?daughter(3).bDiscriminator('combinedInclusiveSecondaryVertexV2BJetTags'):0")
    addGloabalFloat(diphotonDumper.globalVariables,process,"mht30clean","jet4Pt","?numberOfDaughters>3?daughter(3).pt:0")
    addGloabalFloat(diphotonDumper.globalVariables,process,"mht30clean","jet4Eta","?numberOfDaughters>3?daughter(3).eta:0")
    addGloabalFloat(diphotonDumper.globalVariables,process,"mht30clean","jet4Phi","?numberOfDaughters>3?daughter(3).phi:0")
    
    bookCandViewNtProducer(process,"met","slimmedMETs")
    addGloabalFloat(diphotonDumper.globalVariables,process,"met","metPt","pt")
    addGloabalFloat(diphotonDumper.globalVariables,process,"met","metPhi","phi")
    addGloabalFloat(diphotonDumper.globalVariables,process,"met","sumEt","sumEt")


