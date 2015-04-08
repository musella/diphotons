import FWCore.ParameterSet.Config as cms
import FWCore.ParameterSet.VarParsing as VarParsing

from flashgg.MicroAOD.flashggPreselectedDiPhotons_cfi import flashggPreselectedDiPhotons as simpleTemplate
from diphotons.Analysis.diphotonsWithMVA_cfi import diphotonsWithMVA

from HLTrigger.HLTfilters.hltHighLevel_cfi import hltHighLevel

# -------------------------------------------------------------------------------------------------------------------------
class DiPhotonAnalysis(object):
    
    # ----------------------------------------------------------------------------------------------------------------------
    def __init__(self,dumperTemplate,
                 massCut=500.,ptLead=200.,ptSublead=200.,scaling=False,computeMVA=False,
                 genIsoDefinition=("userFloat('genIso')",10.),
                 dataTriggers=["HLT_DoublePhoton85*","HLT_Photon250_NoHE*"],
                 mcTriggers=["HLT_DoublePhoton85*","HLT_Photon250_NoHE*"],
                 askTriggerOnMc=False):
        
        super(DiPhotonAnalysis,self).__init__()
        
        self.dumperTemplate = dumperTemplate
        self.massCut = massCut
        self.ptLead  = ptLead
        self.ptSublead  = ptSublead
        self.scalingFunc = ""
        self.computeMVA=computeMVA
        self.mcTriggers = mcTriggers
        self.dataTriggers = dataTriggers
        self.askTriggerOnMc = askTriggerOnMc
        
        self.analysisSelections = []
        self.splitByIso = []
        self.splitDumpers = []
        
        if scaling:
            self.scalingFunc = "*%f/mass" % massCut
        
        self.isoCut = {}
        if genIsoDefinition:
            self.isoCut["genIsoVar"] = genIsoDefinition[0]

            self.isoCut["genIsoCut"] = genIsoDefinition[1]
    
    
    # ----------------------------------------------------------------------------------------------------------------------
    def getHltFilter(self,process):
        if not hasattr(process,"hltHighLevel"):
            process.hltHighLevel = hltHighLevel.clone()
        return process.hltHighLevel

    # ----------------------------------------------------------------------------------------------------------------------
    def customize(self,process,jobConfig):
        
        jobConfig.register('dumpConfig',
                           False, # default value
                           VarParsing.VarParsing.multiplicity.singleton, # singleton or list
                           VarParsing.VarParsing.varType.bool,          # string, int, or float
                           "dumpConfig")
        jobConfig(process)

        trg = None
        splitByIso = False
        print jobConfig.processId
        print jobConfig.processType
        if jobConfig.processType == "data":
            ## data and MC menus may not be identical
            trg = self.getHltFilter(process)
            trg.HLTPaths=self.dataTriggeres            
        
        else:
            if "GGJet" in jobConfig.processId or "DiPhoton" in jobConfig.processId or "RSGravToGG" in jobConfig.processId or jobConfig.processType == "signal":
                splitByIso = True

            if self.mcTriggers and len(self.mcTriggers)>0:
                ## data and MC menus may not be identical
                trg = self.getHltFilter(process)
                trg.HLTPaths=self.mcTriggers
                
                ## if askTriggerOnMc is false, only add dedicated dumper to study the efficiencies
                if not self.askTriggerOnMc:
                    self.addTriggeredDumpers(process,splitByIso=splitByIso)
                    trg = None
        
        print jobConfig.processId, jobConfig.processType, self.splitByIso, splitByIso
        ## add execution paths for analysis selections
        for coll,dumper in self.analysisSelections:
            if not self.useDumper(process,dumper,splitByIso): 
                continue
            self.addPath(process,dumper,trg)
        
        if jobConfig.dumpConfig:
            print 
            print "------------------------------------------------------------------------------------"
            print "DiPhotonAnalysis: dumping configuration summary "
            print "  to see full process dump use process.dumpPython() "
            print "------------------------------------------------------------------------------------"

            print 
            print "Configured paths :"
            print "------------------------------------------------------------------------------------"
            selectors = []
            for pathName in process.paths:
                if "triggeredPath" in pathName: 
                    continue
                path = getattr(process,pathName)
                ## print pathName, ":", getattr(process,pathName)
                print pathName, ":",
                for modName in path.moduleNames():
                    print modName, 
                    module = getattr(process,modName)
                    if hasattr(module,"src"):
                        src = getattr(module,"src").getModuleLabel()
                        print "(src=%s)" % src,
                        selectors.append( src  )
                print
            print
            
            print 
            print "Selectors :"
            print "------------------------------------------------------------------------------------"
            for selName in selectors:
                selector = getattr(process,selName)
                print
                print selector, " = cms.PSet("
                if hasattr(selector,"src"):
                    print "  src =", getattr(selector,"src")
                if hasattr(selector,"cut"):
                    print "  cut =", getattr(selector,"cut")
                if hasattr(selector,"ignore"):
                    print "  ignore =", getattr(selector,"ignore")
                if hasattr(selector,"invert"):
                    print "  invert =", getattr(selector,"invert")
                if hasattr(selector,"variables"):
                    print "  variables =", getattr(selector,"variables")
                print ")"
            print
        
            print
            print "------------------------------------------------------------------------------------"
            print "DiPhotonAnalysis: end configuration summary "
            print "  to see full process dump use process.dumpPython() "
            print "------------------------------------------------------------------------------------"
            print
            
        ## unscheduled execution: cmsRun will figure out the dependencies
        process.options = cms.untracked.PSet( allowUnscheduled = cms.untracked.bool(True) )
        
        
    # ----------------------------------------------------------------------------------------------------------------------
    def addKinematicSelection(self,process,dumperTemplate=None,
                              dumpTrees=True,dumpWorkspace=False,dumpHistos=True,splitByIso=True):
        """
        Add basic kinematic selection to the process    
        """
        if not dumperTemplate:
            dumperTemplate = self.dumperTemplate
        
        template = simpleTemplate.clone(src=cms.InputTag("flashggDiPhotons"),
                                        cut = cms.string(
                "mass > %(massCut)f"
                " && leadingPhoton.pt > %(ptLead)f %(scalingFunc)s && subLeadingPhoton.pt > %(ptSublead)f %(scalingFunc)s"
                " && abs(leadingPhoton.superCluster.eta)<2.5 && abs(subLeadingPhoton.superCluster.eta)<2.5 "
                " && ( abs(leadingPhoton.superCluster.eta)<1.4442 || abs(leadingPhoton.superCluster.eta)>1.566)"
                " && ( abs(subLeadingPhoton.superCluster.eta)<1.4442 || abs(subLeadingPhoton.superCluster.eta)>1.566)" 
                % { "massCut" : self.massCut, 
                    "ptLead"  : self.ptLead,
                    "ptSublead" : self.ptSublead,
                    "scalingFunc" : self.scalingFunc
                    }
                )
                                        )
        
        if self.computeMVA:
            setattr(process,"tmpKinDiphotons",template)
            template = diphotonsWithMVA.clone(src=cms.InputTag("tmpKinDiphotons"))
        
        self.analysisSelections += self.addDiphoSelection(process,"kin",template,dumperTemplate,
                                                          dumpTrees=dumpTrees,dumpWorkspace=dumpWorkspace,dumpHistos=dumpHistos,splitByIso=splitByIso)
        
        
    # ----------------------------------------------------------------------------------------------------------------------
    def addAnalysisSelection(self,process,label,selectorTemplate,
                     nMinusOne=None,dumperTemplate=None,
                     dumpTrees=False,dumpWorkspace=False,dumpHistos=True,splitByIso=True):
        """
        Add analysis selection to the process.
        Also adds the kinematic selection in case it hasn't been, but no tree is asked for it.
        """
    
        if not hasattr(process,"kinDiPhotons"):
            modules += addKinematicSelection(process,dumpTrees=False,dumperTemplate=dumperTemplate)
            
        template = selectorTemplate.clone(src=cms.InputTag("kinDiPhotons"))
        self.analysisSelections += self.addDiphoSelection(process,label,template,dumperTemplate=dumperTemplate,splitByIso=splitByIso)
        if nMinusOne:
            self.analysisSelections += self.addNMinusOneDiphoSelections(process,label,template,nMinusOne,
                                                                        dumperTemplate=dumperTemplate,splitByIso=splitByIso)
    
    # ----------------------------------------------------------------------------------------------------------------------
    def addDiphoSelection(self,process,label,selectorTemplate,dumperTemplate=None,dumpTrees=False,dumpWorkspace=False,dumpHistos=True,splitByIso=False):
        """
        Add diphoton producer and dumper to the process
        """
        
        modules = []
        
        if not dumperTemplate:
            dumperTemplate = self.dumperTemplate

        diphoColl  = "%sDiPhotons" % label
        dumperName = label
        
        ## register diphoton selector and associated dumper
        setattr(process,diphoColl,selectorTemplate.clone())
        setattr(process,dumperName,dumperTemplate.clone(src=cms.InputTag(diphoColl), 
                                                        dumpTrees=cms.untracked.bool(dumpTrees),
                                                        dumpWorkspace=cms.untracked.bool(dumpWorkspace),
                                                        dumpHistos=cms.untracked.bool(dumpHistos),
                                                        )
                )
        
        dumper = getattr(process,dumperName)
        modules.append( (diphoColl,dumperName) )
        
        ## split into isolated and non isolated parts
        if splitByIso:
            self.splitByIso.append(dumperName)
            setattr(process,diphoColl+"GenIso",simpleTemplate.clone(src=cms.InputTag(diphoColl),
                                                                 cut=cms.string("    leadingPhoton.%(genIsoVar)s < %(genIsoCut)f"
                                                                                " && subLeadingPhoton.%(genIsoVar)s < %(genIsoCut)f" 
                                                                                % self.isoCut
                                                                                )
                                                                 )
                    )
            setattr(process,dumperName+"GenIso",dumperTemplate.clone(src=cms.InputTag(diphoColl+"GenIso"), 
                                                                  dumpTrees=cms.untracked.bool(dumpTrees),
                                                                  dumpWorkspace=cms.untracked.bool(dumpWorkspace),
                                                                  dumpHistos=cms.untracked.bool(dumpHistos),
                                                                  )
                    )
            
            setattr(process,diphoColl+"NonGenIso",simpleTemplate.clone(src=cms.InputTag(diphoColl),
                                                                    cut=cms.string("    leadingPhoton.%(genIsoVar)s < %(genIsoCut)f"
                                                                                   " || subLeadingPhoton.%(genIsoVar)s < %(genIsoCut)f"
                                                                                   % self.isoCut
                                                                                   )
                                                                    )
                    )
            setattr(process,dumperName+"NonGenIso",dumperTemplate.clone(src=cms.InputTag(diphoColl+"NonGenIso"), 
                                                                     dumpTrees=cms.untracked.bool(dumpTrees),
                                                                     dumpWorkspace=cms.untracked.bool(dumpWorkspace),
                                                                     dumpHistos=cms.untracked.bool(dumpHistos),
                                                                     )
                    )
            
            modules.append( (diphoColl+"GenIso",dumperName+"GenIso"))
            modules.append( (diphoColl+"NonGenIso",dumperName+"NonGenIso"))
            self.splitDumpers.extend( [dumperName+"GenIso",dumperName+"NonGenIso"] )
            
        return modules
        
            
    # ----------------------------------------------------------------------------------------------------------------------
    def addNMinusOneDiphoSelections(self,process,label,selectorTemplate,nMinusOne,dumperTemplate=None,splitByIso=False):
        """
        Add diphoton producer n-1 selection and corresponding dumper to the process
        """
        modules = []
        if not dumperTemplate:
            dumperTemplate = self.dumperTemplate

        for settings in nMinusOne:
            if len(settings) == 7:
                ignore,invert,ilabel,dumpTrees,dumpWorkspace,dumpHistos,doSplitByIso = settings
                nMinus1Template = selectorTemplate.clone(ignore=cms.vint32(ignore),
                                                         invert=cms.vint32(invert[0]),
                                                         invertNtimes=cms.int32(invert[1]),
                                                         )
            else:
                ignore,ilabel,dumpTrees,dumpWorkspace,dumpHistos = settings[:5]
                nMinus1Template = selectorTemplate.clone(ignore=cms.vint32(ignore))
                if len(settings) == 0:
                    doSplitByIso = settings[5]
                else:
                    doSplitByIso = splitByIso
                
            modules += self.addDiphoSelection(process,"%s%s" % (label,ilabel),nMinus1Template,
                                              dumpTrees=dumpTrees,
                                              dumpWorkspace=dumpWorkspace,
                                              dumpHistos=dumpHistos,
                                              splitByIso=doSplitByIso
                                              )
            
        return modules
    
    # ----------------------------------------------------------------------------------------------------------------------
    def addTriggeredDumpers(self,process,dumperTemplate=None,splitByIso=False):
        
        for coll,dumper in self.analysisSelections:
            if not self.useDumper(process,dumper,splitByIso):
                continue
            self.addTriggeredDumper(process,coll,dumperTemplate)
            
    # ----------------------------------------------------------------------------------------------------------------------
    def addTriggeredDumper(self,process,label,dumperTemplate=None):
        if not dumperTemplate:
            dumperTemplate = self.dumperTemplate
        
        if not "DiPhotons" in label:
            diphoColl  = "%sDiPhotons" % label
        else:
            diphoColl  = label
        dumperName = "%sTrg" % label
        
        trg = self.getHltFilter(process)
        setattr(process,dumperName,dumperTemplate.clone(src=cms.InputTag(diphoColl), 
                                                        dumpTrees=cms.untracked.bool(False),
                                                        dumpWorkspace=cms.untracked.bool(False),
                                                        dumpHistos=cms.untracked.bool(True),
                                                        )
                )
                
        self.addPath(process,dumperName,trg)
                
    
    # ----------------------------------------------------------------------------------------------------------------------
    def addPath(self,process,module,trg):
        
        if trg:
            setattr(process,"triggeredPath%s"%module, cms.Path(trg*getattr(process,module)) )
        else:
            setattr(process,"path%s"%module, cms.Path(getattr(process,module)) )
            
    # ----------------------------------------------------------------------------------------------------------------------
    def useDumper(self,process,dumper,splitByIso):
        
        if dumper in self.splitDumpers and not splitByIso:
            return False
        if dumper in self.splitByIso and splitByIso:
            getattr(process,dumper).dumpTrees = False
            
        return True