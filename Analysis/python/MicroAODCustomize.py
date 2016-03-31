from flashgg.MicroAOD.MicroAODCustomize import  MicroAODCustomize as  fggMicroAODCustomize
import FWCore.ParameterSet.Config as cms

class MicroAODCustomize(fggMicroAODCustomize):
    
    def __init__(self,*args,**kwargs):
        
        super(MicroAODCustomize,self).__init__()

    def userCustomize(self,process):

        if self.processType != "data":
            ## random cones added to data by default from 76x on
            self.customizeHighMassIsolations(process)
            
            ## keep generated graviton
            process.flashggPrunedGenParticles.select.extend(["keep status = 3",
                                                             "keep status = 22",
                                                             "keep status = 23",
                                                             "++keep pdgId = 22",
                                                             "keep++ pdgId = 39",
                                                             "keep++ pdgId = 5000039",
                                                             ])
            
            process.flashggGenPhotonsExtra.promptMothers=cms.vint32(39,5000039,5100039)


        ### process.out.outputCommands.extend([
        ###                             ]
        ###                            )


        preselEle = ["WWTo2L2Nu","ZZTo4L","WZJets","WJetsToLNu"]
        preselPhotons = ["QCD"]+preselEle
        
        if len(filter(lambda x: x in self.datasetName,preselPhotons) ) > 0:
            process.myPreselectedPhotons = cms.EDFilter("FlashggPhotonSelector",
                                                        src=cms.InputTag("flashggPhotons"),
                                                        cut=cms.string("(r9>0.8||egChargedHadronIso<20||egChargedHadronIso/pt<0.3) && pt>50 && egChargedHadronIso<15"),
                                                        filter=cms.bool(True)
                                                        )
            process.flashggMicroAODSequence += process.myPreselectedPhotons

        if len(filter(lambda x: x in self.datasetName,preselEle) ) > 0:
            process.myPreselectedElectrons = cms.EDFilter("FlashggElectronSelector",
                                                          src=cms.InputTag("flashggElectrons"),
                                                          cut=cms.string("pt>20"),
                                                          filter=cms.bool(True)
                                                          )
            process.flashggMicroAODSequence += process.myPreselectedElectrons

    
        ### if "QCD" in self.datasetName:
        ###     ##     process.flashggPreselectedDiPhotons.filter = cms.bool(True)
        ###     process.myPreselectedPhotons.filter = cms.bool(True)
        ### 
        ### if "WWTo2L2Nu" in self.datasetName:
        ###     process.myPreselectedElectrons.filter = cms.bool(True)
        ### 
        ### if "ZZTo4L" in self.datasetName:
        ###     process.myPreselectedElectrons.filter = cms.bool(True)
        ### 
        ### if "WZJets" in self.datasetName:
        ###     process.myPreselectedElectrons.filter = cms.bool(True)
        ### 
        ### if "WJetsToLNu" in self.datasetName:
        ###     process.myPreselectedElectrons.filter = cms.bool(True)
        ### 
        ### if "WWTo2L2Nu" in self.datasetName:
        ###     process.myPreselectedPhotons.filter = cms.bool(True)
        ### 
        ### if "ZZTo4L" in self.datasetName:
        ###     process.myPreselectedPhotons.filter = cms.bool(True)
        ### 
        ### if "WZJets" in self.datasetName:
        ###     process.myPreselectedPhotons.filter = cms.bool(True)
        ### 
        ### if "WJetsToLNu" in self.datasetName:
        ###     process.myPreselectedPhotons.filter = cms.bool(True)
            
    # signal specific customization
    def customizeSignal(self,process):
        process.flashggGenPhotonsExtra.defaultType = 1


# customization object
customize = MicroAODCustomize()
