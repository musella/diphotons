import FWCore.ParameterSet.Config as cms
import FWCore.ParameterSet.VarParsing as VarParsing

simpleTemplate = cms.EDFilter( "DiPhotonCandidateSelector", src = cms.InputTag("flashggDiPhotons") )
singlePhoSimpleTemplate = cms.EDFilter("PhotonSelector",src = cms.InputTag("flashggPhotons"),)

from diphotons.Analysis.diphotonsWithMVA_cfi import diphotonsWithMVA

from HLTrigger.HLTfilters.hltHighLevel_cfi import hltHighLevel

import sys

# -------------------------------------------------------------------------------------------------------------------------
class DiPhotonAnalysis(object):
    
    # ----------------------------------------------------------------------------------------------------------------------
    def __init__(self,dumperTemplate,
                 massCut=500.,ptLead=200.,ptSublead=200.,scaling=False,computeMVA=False,
                 genIsoDefinition=("genIso",10.),
                 dataTriggers=["HLT_DoublePhoton60*","HLT_DoublePhoton85*","HLT_Photon250_NoHE*"],
                 mcTriggers=["HLT_DoublePhoton60*","HLT_DoublePhoton85*","HLT_Photon250_NoHE*"],
                 askTriggerOnMc=False,sortTemplate=False,singlePhoDumperTemplate=False,computeRechitFlags=False,removeEEEE=True,
                 applySmearingCorrections=False,applyEnergyCorrections=False):
        
        super(DiPhotonAnalysis,self).__init__()
        
        self.dumperTemplate = dumperTemplate
        self.singlePhoDumperTemplate = singlePhoDumperTemplate
        self.massCut = massCut
        self.ptLead  = ptLead
        self.ptSublead  = ptSublead
        self.scalingFunc = ""
        self.removeEEEE = removeEEEE
        self.computeMVA = computeMVA
        self.mcTriggers = mcTriggers
        self.dataTriggers = dataTriggers
        self.askTriggerOnMc = askTriggerOnMc
        self.applyEnergyCorrections = applyEnergyCorrections
        self.applySmearingCorrections = applySmearingCorrections
        
        self.computeRechitFlags = computeRechitFlags
        
        self.analysisSelections = []
        self.photonSelections = []
        self.splitByIso = []
        self.splitDumpers = []

        self.keepFFOnly = False
        self.keepPFOnly = False
        self.keepPOnly  = False
        self.keepFOnly  = False
        self.vetoGenDiphotons = False

        if sortTemplate:
            self.sortTemplate = sortTemplate.clone()
        else:
            self.sortTemplate = cms.EDFilter("DiPhotonCandidateSorter",maxNumber=cms.uint32(1))
        
        if scaling:
            self.scalingFunc = "*(%f)/mass" % massCut

        self.isoCut = {}
        if genIsoDefinition:
            self.isoCut["genIsoVar"] = "userFloat('%s')" % genIsoDefinition[0]
            self.isoCut["genGenIsoVar"] = genIsoDefinition[0]
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
        jobConfig.parse()
        
        trg = None
        splitByIso = False
        print jobConfig.processId
        print jobConfig.processType
        if jobConfig.processType == "data":
            ## data and MC menus may not be identical
            trg = self.getHltFilter(process)
            trg.HLTPaths=self.dataTriggers            
        
        else:
            if "GGJet" in jobConfig.processId or "DiPhoton" in jobConfig.processId or "RSGravToGG" in jobConfig.processId or "RSGravitonToGG" in jobConfig.processId  or jobConfig.processType == "signal":
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
        for coll,dumper in self.analysisSelections+self.photonSelections:
            if not self.useDumper(process,dumper,splitByIso): 
                continue
            self.addPath(process,dumper,trg)
        
        print jobConfig.parsed
        jobConfig(process)

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
                    print "(",
                    if hasattr(module,"src"):
                        src = getattr(module,"src").getModuleLabel()
                        print "src=%s" % src,
                        selectors.append( src  )
                    if hasattr(module,"dumpTrees"):
                        print "dumpTrees=%s" % str(getattr(module,"dumpTrees"))
                    print ")",
                print
            print
            
            print 
            print "Selectors :"
            print "------------------------------------------------------------------------------------"
            for selName in selectors:
                selector = getattr(process,selName)                
                print
                print selector, ' = cms.EDFilter("%s",' % selector._TypedParameterizable__type
                if hasattr(selector,"src"):
                    src = getattr(selector,"src")
                    print "  src =", src
                    if hasattr(process,src.getModuleLabel()) and not src.getModuleLabel() in selectors:
                        selectors.append( src.getModuleLabel() )
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

        if jobConfig.dumpPython != "":
            from gzip import open
            pyout = open("%s.gz" % jobConfig.dumpPython,"w+")
            pyout.write( process.dumpPython() )
            pyout.close()
        
        
    # ----------------------------------------------------------------------------------------------------------------------
    def addKinematicSelection(self,process,dumperTemplate=None,
                              dumpTrees=True,dumpWorkspace=False,dumpHistos=True,splitByIso=True):
        """
        Add basic kinematic selection to the process    
        """
        if not dumperTemplate:
            dumperTemplate = self.dumperTemplate
        
        src = "flashggDiPhotons"
        if self.computeRechitFlags:
            process.flashggDiPhotonsWithFlags = cms.EDProducer("DiphotonsDiPhotonsRechiFlagProducer",
                                                               src=cms.InputTag(src),
                                                               reducedBarrelRecHitCollection = cms.InputTag('reducedEgamma','reducedEBRecHits'),
                                                               reducedEndcapRecHitCollection = cms.InputTag('reducedEgamma','reducedEERecHits'),
                                                               reducedPreshowerRecHitCollection = cms.InputTag('reducedEgamma','reducedESRecHits')                
                                                               )
            src = "flashggDiPhotonsWithFlags"
        if self.applyEnergyCorrections:
            process.load("diphotons.Analysis.highMassCorrectedDiphotons_cfi")
            process.highMassCorrectedDiphotonsData.src=src
            src = "highMassCorrectedDiphotonsData"
        elif self.applySmearingCorrections:
            process.load("diphotons.Analysis.highMassCorrectedDiphotons_cfi")
            process.highMassCorrectedDiphotonsMC.src=src
            src = "highMassCorrectedDiphotonsMC"
            

        template = simpleTemplate.clone(src=cms.InputTag(src),
                                        cut = cms.string(
                "mass > %(massCut)f"
                " && leadingPhoton.pt > %(ptLead)f %(scalingFunc)s && subLeadingPhoton.pt > %(ptSublead)f %(scalingFunc)s"
                " && abs(leadingPhoton.superCluster.eta)<2.5 && abs(subLeadingPhoton.superCluster.eta)<2.5 "
                " && ( abs(leadingPhoton.superCluster.eta)<1.4442 || abs(leadingPhoton.superCluster.eta)>1.566)"
                " && ( abs(subLeadingPhoton.superCluster.eta)<1.4442 || abs(subLeadingPhoton.superCluster.eta)>1.566)" 
                " && ( abs(leadingPhoton.superCluster.eta) < 1.5 || abs(subLeadingPhoton.superCluster.eta) < 1.5 )"
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
                                                          dumpTrees=dumpTrees,dumpWorkspace=dumpWorkspace,dumpHistos=dumpHistos,splitByIso=splitByIso,selectN=False)

    # ----------------------------------------------------------------------------------------------------------------------
    def addGenOnlySelection(self,process,dumperTemplate,
                            dumpTrees=True,dumpWorkspace=False,dumpHistos=True,splitByIso=True):
        """
        Add basic kinematic selection to the process    
        """
        if not hasattr(process,"flashggGenDiPhotons"):
            process.load("flashgg.MicroAOD.flashggGenDiPhotons_cfi")
                        
        extraCut = ""
        if self.vetoGenDiphotons:
            extraCut = "&& (mass <= %1.5g)" % self.vetoGenDiphotons
        if self.removeEEEE:
            extraCut += "&& (abs(leadingPhoton.eta)    < 1.5    || abs(subLeadingPhoton.eta) < 1.5  )" 
        selectorTemplate = cms.EDFilter("GenDiPhotonSelector",src=cms.InputTag("flashggGenDiPhotons"),
                                        cut=cms.string("mass > %(massCut)f"
                                                       "&& leadingPhoton.pt > %(ptLead)f %(scalingFunc)s && subLeadingPhoton.pt > %(ptSublead)f %(scalingFunc)s"
                                                       "&& (abs(leadingPhoton.eta)    < 1.4442 || abs(leadingPhoton.eta)    > 1.566)"
                                                       "&& (abs(subLeadingPhoton.eta) < 1.4442 || abs(subLeadingPhoton.eta) > 1.566)"
                                                       "&& (abs(leadingPhoton.eta)    < 2.5    && abs(subLeadingPhoton.eta) < 2.5  )"
                                                       "%(extraCut)s"
                                                       % { "massCut" : self.massCut, 
                                                           "ptLead"  : self.ptLead,
                                                           "ptSublead" : self.ptSublead,
                                                           "scalingFunc" : self.scalingFunc,
                                                           "extraCut" : extraCut
                                                           }
                                                       )
                                        )
        sorterTemplate = cms.EDFilter("GenDiPhotonSorter",src=cms.InputTag("flashggGenDiPhotons"),maxNumber=cms.uint32(1))

        self.analysisSelections += self.addGenDiphoSelection(process,"gen",selectorTemplate,sorterTemplate,dumperTemplate,
                                                             dumpTrees=dumpTrees,dumpWorkspace=dumpWorkspace,dumpHistos=dumpHistos,splitByIso=splitByIso,selectN=False)
        
        
    # ----------------------------------------------------------------------------------------------------------------------
    def addAnalysisSelection(self,process,label,selectorTemplate,
                     nMinusOne=None,dumperTemplate=None,
                     dumpTrees=False,dumpWorkspace=False,dumpHistos=True,splitByIso=True):
        """
        Add analysis selection to the process.
        Also adds the kinematic selection in case it hasn't been, but no tree is asked for it.
        """

        if not hasattr(process,"kinDiPhotons"):
            self.addKinematicSelection(process,dumpTrees=False,dumperTemplate=dumperTemplate)
            
        template = selectorTemplate.clone(src=cms.InputTag("kinDiPhotons"))
        self.analysisSelections += self.addDiphoSelection(process,label,template,dumperTemplate=dumperTemplate,splitByIso=splitByIso,
                                                          dumpTrees=dumpTrees,dumpHistos=dumpHistos)
        if nMinusOne:
            self.analysisSelections += self.addNMinusOneDiphoSelections(process,label,template,nMinusOne,
                                                                        dumperTemplate=dumperTemplate,splitByIso=splitByIso)
    
    # ----------------------------------------------------------------------------------------------------------------------
    def addDiphoSelection(self,process,label,selectorTemplate,dumperTemplate=None,dumpTrees=False,dumpWorkspace=False,dumpHistos=True,splitByIso=False,
                          selectN=False):
        """
        Add diphoton producer and dumper to the process
        """
        
        modules = []
        
        if not dumperTemplate:
            dumperTemplate = self.dumperTemplate

        diphoColl  = "%sDiPhotons" % label
        postSelect = None
        if self.keepFFOnly:
            postSelect = "leadingPhoton.genMatchType != 1 && subLeadingPhoton.genMatchType != 1"
        elif self.keepPFOnly:
            postSelect = "(leadingPhoton.genMatchType != 1) != (subLeadingPhoton.genMatchType != 1 )"
        elif self.vetoGenDiphotons:
            postSelect = "genP4.mass <= %1.5g" % self.vetoGenDiphotons
        sortDiPhoColl = "sorted"+diphoColl if postSelect else diphoColl
        
        
        dumperName = label
        ## register diphoton selector and associated dumper
        setattr(process,"all"+diphoColl,selectorTemplate.clone())
        if selectN:
            setattr(process,sortDiPhoColl,self.sortTemplate.clone(src=cms.InputTag("all"+diphoColl),
                                                                  maxNumber=cms.uint32(selectN),
                                                                  ))
        else:
            setattr(process,sortDiPhoColl,self.sortTemplate.clone(src=cms.InputTag("all"+diphoColl)))
            
        if postSelect:
            setattr(process,diphoColl,simpleTemplate.clone(src=cms.InputTag(sortDiPhoColl),
                                                           cut=cms.string(postSelect))
                                                           )
            
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
                                                                 cut=cms.string("leadingPhoton.genMatchType == 1 && subLeadingPhoton.genMatchType == 1 "
                                                                                " && leadingPhoton.%(genIsoVar)s < %(genIsoCut)f"
                                                                                " && subLeadingPhoton.%(genIsoVar)s < %(genIsoCut)f" 
                                                                                % self.isoCut
                                                                                )
                                                                 )
                    )
            print ("leadingPhoton.genMatchType == 1 && subLeadingPhoton.genMatchType == 1 "
                   " && leadingPhoton.%(genIsoVar)s < %(genIsoCut)f"
                   " && subLeadingPhoton.%(genIsoVar)s < %(genIsoCut)f"
                   % self.isoCut)
            setattr(process,dumperName+"GenIso",dumperTemplate.clone(src=cms.InputTag(diphoColl+"GenIso"), 
                                                                  dumpTrees=cms.untracked.bool(dumpTrees),
                                                                  dumpWorkspace=cms.untracked.bool(dumpWorkspace),
                                                                  dumpHistos=cms.untracked.bool(dumpHistos),
                                                                  )
                    )
            
            setattr(process,diphoColl+"NonGenIso",simpleTemplate.clone(src=cms.InputTag(diphoColl),
                                                                    cut=cms.string("leadingPhoton.genMatchType != 1 || subLeadingPhoton.genMatchType != 1 "
                                                                                   " || leadingPhoton.%(genIsoVar)s >= %(genIsoCut)f"
                                                                                   " || subLeadingPhoton.%(genIsoVar)s >= %(genIsoCut)f"
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
    def addGenDiphoSelection(self,process,label,selectorTemplate,sorterTemplate,dumperTemplate,dumpTrees=False,dumpWorkspace=False,dumpHistos=True,splitByIso=False,
                             selectN=False):
        """
        Add diphoton producer and dumper to the process
        """
        
        modules = []
        
        diphoColl  = "%sDiPhotons" % label
        postSelect = None
        if self.keepFFOnly:
            postSelect = "leadingExtra.type != 1 && subLeadingExtra.type != 1"
        elif self.keepPFOnly:
            postSelect = "(leadingExtra.type != 1) != (subLeadingExtra.type != 1 )"
            
        sortDiPhoColl = "sorted"+diphoColl if postSelect else diphoColl
        
        
        dumperName = label
        ## register diphoton selector and associated dumper
        setattr(process,"all"+diphoColl,selectorTemplate.clone())
        if selectN:
            setattr(process,sortDiPhoColl,sorterTemplate.clone(src=cms.InputTag("all"+diphoColl),
                                                                  maxNumber=cms.uint32(selectN),
                                                                  ))
        else:
            setattr(process,sortDiPhoColl,sorterTemplate.clone(src=cms.InputTag("all"+diphoColl)))
            
        if postSelect:
            setattr(process,diphoColl,selectorTemplate.clone(src=cms.InputTag(sortDiPhoColl),
                                                           cut=cms.string(postSelect))
                                                           )
        
        print diphoColl, getattr(process,diphoColl).dumpPython()
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
            setattr(process,diphoColl+"GenIso",selectorTemplate.clone(src=cms.InputTag(diphoColl),
                                                                 cut=cms.string("leadingExtra.type == 1 && subLeadingExtra.type == 1 "
                                                                                " && leadingExtra.%(genGenIsoVar)s < %(genIsoCut)f"
                                                                                " && subLeadingExtra.%(genGenIsoVar)s < %(genIsoCut)f" 
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
            
            setattr(process,diphoColl+"NonGenIso",selectorTemplate.clone(src=cms.InputTag(diphoColl),
                                                                    cut=cms.string("leadingExtra.type != 1 || subLeadingExtra.type != 1 "
                                                                                   " || leadingExtra.%(genGenIsoVar)s >= %(genIsoCut)f"
                                                                                   " || subLeadingExtra.%(genGenIsoVar)s >= %(genIsoCut)f"
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
                if hasattr(selectorTemplate,"ignore"):
                    ignore += selectorTemplate.ignore;
                nMinus1Template = selectorTemplate.clone(ignore=cms.vint32(ignore),
                                                         invert=cms.vint32(invert[0]),
                                                         invertNtimes=cms.int32(invert[1]),
                                                         )
            else:
                ignore,ilabel,dumpTrees,dumpWorkspace,dumpHistos = settings[:5]
                if hasattr(selectorTemplate,"ignore"):
                    ignore += selectorTemplate.ignore;
                nMinus1Template = selectorTemplate.clone(ignore=cms.vint32(ignore))
                if len(settings) == 0:
                    doSplitByIso = settings[5]
                else:
                    doSplitByIso = splitByIso
                
            modules += self.addDiphoSelection(process,"%s%s" % (label,ilabel),nMinus1Template,
                                              dumpTrees=dumpTrees,
                                              dumpWorkspace=dumpWorkspace,
                                              dumpHistos=dumpHistos,
                                              splitByIso=doSplitByIso,
                                              dumperTemplate=dumperTemplate
                                              )
            
        return modules

    # ----------------------------------------------------------------------------------------------------------------------
    def addPhotonKinematicSelection(self,process,dumperTemplate=None,
                                    dumpTrees=True,dumpWorkspace=False,dumpHistos=True,splitByIso=True):
        """
        Add signle photon kinematic selection to the process    
        """
        if not dumperTemplate:
            dumperTemplate = self.dumperTemplate
        
        template = singlePhoSimpleTemplate.clone(src=cms.InputTag("flashggPhotons"),
                                        cut = cms.string(
                " pt > %(ptSublead)f"
                " && abs(superCluster.eta)<2.5 && ( abs(superCluster.eta)<1.4442 || abs(superCluster.eta)>1.566)"
                % { "ptSublead" : self.ptSublead,
                    }
                )
                                        )
        
        if self.computeMVA:
            ## sys.exit("MVA computation not supported for single photon selection. Please do something about it.",-1)
            print "WARNING: MVA computation not supported for single photon selection."
            
        self.photonSelections += self.addPhoSelection(process,"kin",template,dumperTemplate,
                                                      dumpTrees=dumpTrees,dumpWorkspace=dumpWorkspace,dumpHistos=dumpHistos,splitByIso=splitByIso,selectN=False)
        
        
    # ----------------------------------------------------------------------------------------------------------------------
    def addPhotonAnalysisSelection(self,process,label,selectorTemplate,
                     nMinusOne=None,dumperTemplate=None,
                     dumpTrees=False,dumpWorkspace=False,dumpHistos=True,splitByIso=True):
        """
        Add analysis selection to the process.
        Also adds the kinematic selection in case it hasn't been, but no tree is asked for it.
        """
    
        if not hasattr(process,"kinPhotons"):
            self.addPhotonKinematicSelection(process,dumpTrees=False,dumperTemplate=dumperTemplate)
            
        template = selectorTemplate.clone(src=cms.InputTag("kinPhotons"))
        self.photonSelections += self.addPhoSelection(process,label,template,dumperTemplate=dumperTemplate,splitByIso=splitByIso,
                                                          dumpTrees=dumpTrees,dumpHistos=dumpHistos)
        if nMinusOne:
            self.photonSelections += self.addNMinusOnePhoSelections(process,label,template,nMinusOne,
                                                                    dumperTemplate=dumperTemplate,splitByIso=splitByIso)
    

    # ----------------------------------------------------------------------------------------------------------------------
    def addPhoSelection(self,process,label,selectorTemplate,dumperTemplate=None,dumpTrees=False,dumpWorkspace=False,dumpHistos=True,splitByIso=False,
                        selectN=False):
        """
        Add single photon producer and dumpoer to the process
        """
        
        modules = []
        if not dumperTemplate:
            dumperTemplate = self.singlePhoDumperTemplate
            
        phoColl  = "%sPhotons" % label
        dumperName = "%sSinglePho" % label
        postSelect = None
        if self.keepFOnly:
            postSelect = "genMatchType != 1"
        elif self.keepPOnly:
            postSelect = "genMatchType == 1"
            
        sortPhoColl = "sorted"+phoColl if postSelect else phoColl
        
        ## register photon selector and associated dumper
        if selectN:
            sys.exit("selectN not supported for single photon selection. Please do something about it.",-1)
        else:
            setattr(process,sortPhoColl,selectorTemplate.clone())

        if postSelect:
            setattr(process,phoColl,singlePhoSimpleTemplate.clone(src=cms.InputTag(sortPhoColl),
                                                                  cut=cms.string(postSelect))
                    )
        

        setattr(process,dumperName,dumperTemplate.clone(src=cms.InputTag(phoColl), 
                                                        dumpTrees=cms.untracked.bool(dumpTrees),
                                                        dumpWorkspace=cms.untracked.bool(dumpWorkspace),
                                                        dumpHistos=cms.untracked.bool(dumpHistos),
                                                        )
                )
        
        dumper = getattr(process,dumperName)
        modules.append( (phoColl,dumperName) )
        
        ## split into isolated and non isolated parts
        if splitByIso:
            self.splitByIso.append(dumperName)
            setattr(process,phoColl+"GenIso",singlePhoSimpleTemplate.clone(src=cms.InputTag(phoColl),
                                                                    cut=cms.string("genMatchType == 1 "
                                                                                   " && %(genIsoVar)s < %(genIsoCut)f"
                                                                                   % self.isoCut
                                                                                   )
                                                                    )
                    )
            setattr(process,dumperName+"GenIso",dumperTemplate.clone(src=cms.InputTag(phoColl+"GenIso"), 
                                                                     dumpTrees=cms.untracked.bool(dumpTrees),
                                                                     dumpWorkspace=cms.untracked.bool(dumpWorkspace),
                                                                     dumpHistos=cms.untracked.bool(dumpHistos),
                                                                     )
                    )
            
            setattr(process,phoColl+"NonGenIso",singlePhoSimpleTemplate.clone(src=cms.InputTag(phoColl),
                                                                       cut=cms.string("genMatchType != 1 "
                                                                                      " || %(genIsoVar)s >= %(genIsoCut)f"
                                                                                      % self.isoCut
                                                                                      )
                                                                       )
                    )
            setattr(process,dumperName+"NonGenIso",dumperTemplate.clone(src=cms.InputTag(phoColl+"NonGenIso"), 
                                                                        dumpTrees=cms.untracked.bool(dumpTrees),
                                                                        dumpWorkspace=cms.untracked.bool(dumpWorkspace),
                                                                        dumpHistos=cms.untracked.bool(dumpHistos),
                                                                        )
                    )
            
            modules.append( (phoColl+"GenIso",dumperName+"GenIso"))
            modules.append( (phoColl+"NonGenIso",dumperName+"NonGenIso"))
            self.splitDumpers.extend( [dumperName+"GenIso",dumperName+"NonGenIso"] )
            
        return modules


    # ----------------------------------------------------------------------------------------------------------------------
    def addNMinusOnePhoSelections(self,process,label,selectorTemplate,nMinusOne,dumperTemplate=None,splitByIso=False):
        """
        Add photon producer n-1 selection and corresponding dumper to the process
        """
        modules = []
        if not dumperTemplate:
            dumperTemplate = self.singlePhoDumperTemplate

        for settings in nMinusOne:
            if len(settings) == 7:
                ignore,invert,ilabel,dumpTrees,dumpWorkspace,dumpHistos,doSplitByIso = settings
                if hasattr(selectorTemplate,"ignore"):
                    ignore += selectorTemplate.ignore;
                nMinus1Template = selectorTemplate.clone(ignore=cms.vint32(ignore),
                                                         invert=cms.vint32(invert[0]),
                                                         invertNtimes=cms.int32(invert[1]),
                                                         )
            else:
                ignore,ilabel,dumpTrees,dumpWorkspace,dumpHistos = settings[:5]
                if hasattr(selectorTemplate,"ignore"):
                    ignore += selectorTemplate.ignore;
                nMinus1Template = selectorTemplate.clone(ignore=cms.vint32(ignore))
                if len(settings) == 0:
                    doSplitByIso = settings[5]
                else:
                    doSplitByIso = splitByIso
                
            modules += self.addPhoSelection(process,"%s%s" % (label,ilabel),nMinus1Template,
                                            dumpTrees=dumpTrees,
                                            dumpWorkspace=dumpWorkspace,
                                            dumpHistos=dumpHistos,
                                            splitByIso=doSplitByIso,
                                            dumperTemplate=dumperTemplate
                                              )
            
        return modules

    # ----------------------------------------------------------------------------------------------------------------------
    def addTriggeredDumpers(self,process,splitByIso=False):
        
        for coll,dumper in self.analysisSelections:
            if not self.useDumper(process,dumper,splitByIso):
                continue
            self.addTriggeredDumper(process,coll,getattr(process,dumper))

        for coll,dumper in self.photonSelections:
            if not self.useDumper(process,dumper,splitByIso):
                continue
            self.addTriggeredDumper(process,coll,isSingle=True)
            
        ### if len(self.photonSelections) > 0:
        ###     sys.exit("Triggered dumpers not supported for single photon selection. Please do something about it.",-1)
            
    # ----------------------------------------------------------------------------------------------------------------------
    def addTriggeredDumper(self,process,label,dumperTemplate=None,isSingle=False):
        if not dumperTemplate:
            dumperTemplate = self.dumperTemplate if not isSingle else self.singlePhoDumperTemplate
        
        lab = "DiPhotons" if not isSingle else "Photons"
        if not lab in label:
            coll  = "%s%s" % (label,lab)
        else:
            coll  = label
        dumperName = "%sTrg" % label
        
        trg = self.getHltFilter(process)
        setattr(process,dumperName,dumperTemplate.clone(src=cms.InputTag(coll), 
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
