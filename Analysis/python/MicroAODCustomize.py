from flashgg.MicroAOD.MicroAODCustomize import  MicroAODCustomize as  fggMicroAODCustomize
import FWCore.ParameterSet.Config as cms

class MicroAODCustomize(fggMicroAODCustomize):
    
    def __init__(self,*args,**kwargs):
        
        super(MicroAODCustomize,self).__init__()

    def userCustomize(self,process):
        
        if "QCD" in self.datasetName:
            ##     process.flashggPreselectedDiPhotons.filter = cms.bool(True)
            process.myPreselectedPhotons.filter = cms.bool(True)

        if "WWTo2L2Nu" in self.datasetName:
            process.myPreselectedElectrons.filter = cms.bool(True)

        if "ZZTo4L" in self.datasetName:
            process.myPreselectedElectrons.filter = cms.bool(True)

        if "WZJets" in self.datasetName:
            process.myPreselectedElectrons.filter = cms.bool(True)

        if "WJetsToLNu" in self.datasetName:
            process.myPreselectedElectrons.filter = cms.bool(True)

        if "WWTo2L2Nu" in self.datasetName:
            process.myPreselectedPhotons.filter = cms.bool(True)

        if "ZZTo4L" in self.datasetName:
            process.myPreselectedPhotons.filter = cms.bool(True)

        if "WZJets" in self.datasetName:
            process.myPreselectedPhotons.filter = cms.bool(True)

        if "WJetsToLNu" in self.datasetName:
            process.myPreselectedPhotons.filter = cms.bool(True)
            
    # signal specific customization
    def customizeSignal(self,process):
        process.flashggGenPhotonsExtra.defaultType = 1


# customization object
customize = MicroAODCustomize()
