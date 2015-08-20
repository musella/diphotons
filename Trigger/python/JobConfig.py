## from flashgg.MetaData.JobConfig import JobConfig
from flashgg.MicroAOD.MicroAODCustomize import MicroAODCustomize as JobConfig
import FWCore.ParameterSet.Config as cms


class MyJobConfig(JobConfig):
    
    def __init__(self,*args,**kwargs):
        
        super(MyJobConfig,self).__init__(*args,**kwargs)
        
        
    def customize(self,process):
        
        if not hasattr(process,"TFileService"):
            process.TFileService = cms.Service("TFileService",
                                               fileName = cms.string("test.root")
                                               )        
        trgList = []
        for key in process.__dict__.keys():
            if key.startswith("HLT_"):
                trgList.append( key )
                
        process.HLT_MyReference = cms.Path( process.HLTBeginSequence + process.HLTEndSequence )
        process.myTriggerSummary = cms.EDAnalyzer("MyTriggerResults",
                                                  genInfo=cms.InputTag("generator"),
                                                  tag=cms.InputTag("TriggerResults","",process.name_()),
                                                  ref=cms.string("HLT_MyReference"),
                                                  bits=cms.vstring(trgList),
                                                  processId=cms.string(""),
                                                  lumiWeight=cms.double(1.)
                                                  )
        ## print process.myTriggerSummary.dumpPython()
        process.mySummary = cms.EndPath(process.myTriggerSummary)
        
        JobConfig.customize(self,process)
        if hasattr(self,"dataset"):
            datasetName = self.dataset[0]
        else:
            datasetName = self.options.datasetName
        
        if "QCD" in datasetName and not "EMEnriched" in datasetName:
            empty,prim,sec,tier = datasetName.split("/")
            ptMin = float(prim.split("QCD_Pt_")[1].split("to")[0])
            if (ptMin < 170.) or ("AVE20" in datasetName and ptMin == 20.):
                process.emenrichingfilter = cms.EDFilter("EMEnrichingFilter",
                                                         filterAlgoPSet = cms.PSet(isoGenParETMin=cms.double(20.),
                                                                                   isoGenParConeSize=cms.double(0.1),
                                                                                   clusterThreshold=cms.double(20.),
                                                                                   isoConeSize=cms.double(0.2),
                                                                                   hOverEMax=cms.double(0.5),
                                                                                   tkIsoMax=cms.double(5.),
                                                                                   caloIsoMax=cms.double(10.),
                                                                                   requireTrackMatch=cms.bool(False),
                                                                                   genParSource = cms.InputTag("genParticlesForFilter")
                                                                                   )
                                                         )
                process.genParticlesForFilter = cms.EDProducer("GenParticleProducer",
                                                               saveBarCodes = cms.untracked.bool(True),
                                                               src = cms.InputTag("generator"),
                                                               abortOnUnknownPDGCode = cms.untracked.bool(False)
                                                               )
                process.bctoefilter = cms.EDFilter("BCToEFilter",
                                                   filterAlgoPSet = cms.PSet(eTThreshold = cms.double(10),
                                                                             genParSource = cms.InputTag("genParticlesForFilter")
                                                                             )
                                                   )
                
                
                process.HLTBeginSequence.insert(0,~process.emenrichingfilter)
                process.HLTBeginSequence.insert(0,process.genParticlesForFilter)
            

        print process.HLTBeginSequence.dumpPython(None)
            
## customize = MyJobConfig(metaDataSrc="diphotons",
##                        crossSections=["$CMSSW_BASE/src/flashgg/MetaData/data/cross_sections.json",
##                                       "$CMSSW_BASE/src/diphotons/MetaData/data/cross_sections.json"])

customize = MyJobConfig()
