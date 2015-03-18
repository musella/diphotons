#!/usr/bin/env cmsRun

import FWCore.ParameterSet.Config as cms
import FWCore.Utilities.FileUtils as FileUtils

from flashgg.MicroAOD.flashggPreselectedDiPhotons_cfi import flashggPreselectedDiPhotons

def addVariable(hana,nbins,min,max,quantity,name=None,desc=None,itemsToPlot=1):

    if not name:
        name = "dipho_%d_"+quantity.replace(".","_").replace("(","").replace(")","")

    if not desc:
        desc = name
        
    hana.histograms.append(
        cms.PSet( itemsToPlot = cms.untracked.int32(itemsToPlot),
                  min = cms.untracked.double(min),
                  max = cms.untracked.double(max),
                  nbins = cms.untracked.int32(nbins),
                  name = cms.untracked.string(name),
                  description = cms.untracked.string(desc),
                  plotquantity = cms.untracked.string(quantity),
                  )
        )
    
def mkHistogrammer(process,name,collection):

    hana = cms.EDAnalyzer('DiPhotonsHistoAnalyzer',
                          src = cms.InputTag(collection),
                          weights = cms.untracked.InputTag("weightProducer"),
                          histograms = cms.VPSet()
        )
    
    addVariable(hana,1500,0,15000,"mass")
    
    addVariable(hana,145,100,3000,"leadingPhoton.pt")
    addVariable(hana,251,-2.505,2.505,"leadingPhoton.superCluster().eta")
    addVariable(hana,321,3.205,3.205,"leadingPhoton.phi")
    addVariable(hana,30,0,0.3,"leadingPhoton.hadronicOverEm")

    addVariable(hana,145,100,3000,"subLeadingPhoton.pt")
    addVariable(hana,251,-2.505,2.505,"subLeadingPhoton.superCluster().eta")
    addVariable(hana,321,3.205,3.205,"subLeadingPhoton.phi")
    addVariable(hana,30,0,0.3,"subLeadingPhoton.hadronicOverEm")

    setattr(process,name,hana)

    return getattr(process,name)
    

def mkCategoryHistos(process,name,sel):
    setattr(process,name,flashggPreselectedDiPhotons.clone(cut = cms.string(sel)))
    photons = getattr(process,name)
        
    num = mkHistogrammer(process,"num%s" % name, name)
    den = mkHistogrammer(process,"den%s" % name, name)

    return (photons,num,den)


process = cms.Process("Analysis")

process.load("FWCore.MessageService.MessageLogger_cfi")

process.load("Configuration.StandardSequences.GeometryDB_cff")
process.load("Configuration.StandardSequences.MagneticField_cff")
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
process.GlobalTag.globaltag = 'POSTLS170_V5::All'
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )
process.MessageLogger.cerr.FwkReport.reportEvery = cms.untracked.int32( 1000 )

process.source = cms.Source("PoolSource",
                            fileNames=cms.untracked.vstring(
        )
)


process.TFileService = cms.Service("TFileService",
                                   ## fileName = cms.string("histograms_15TeV_OrNoHE.root")
                                   fileName = cms.string("histograms_5TeV_OrNoHE.root")
)

photonColls = cms.Sequence()
nums = cms.Sequence()
dens = cms.Sequence()

common = "leadingPhoton.pt>85 && subLeadingPhoton.pt>85 && (abs(leadingPhoton.superCluster().eta)<1.4442 || (abs(leadingPhoton.superCluster().eta)>1.566 && abs(leadingPhoton.superCluster().eta)<2.5 ) ) && (abs(subLeadingPhoton.superCluster().eta)<1.4442 || (abs(subLeadingPhoton.superCluster().eta)>1.566 && abs(subLeadingPhoton.superCluster().eta)<2.5 ) )"


for name,sel in [ ("EBEB",   "(%s) &&   leadingPhoton.isEB && subLeadingPhoton.isEB" % common), 
                  ("notEBEB","(%s) && !(leadingPhoton.isEB && subLeadingPhoton.isEB)"% common), ]:
    photons,num,den = mkCategoryHistos(process,name,sel)
    photonColls += photons
    nums += num
    dens += den

process.load("flashgg.MetaData.WeightProducer_cfi")
### process.weightProducer = weightProducer = cms.EDProducer('WeightProducer',
###                                 lumiWeight = cms.double(1.),
### )
photonColls += process.weightProducer

process.load('HLTrigger.HLTfilters.hltHighLevel_cfi')
process.hltHighLevel.HLTPaths = ["HLT_DoublePhoton85*","HLT_Photon250_NoHE*"]


process.p1 = cms.Path(
    photonColls*dens
    )

process.p2 = cms.Path(process.hltHighLevel+
                      (photonColls*nums)
                      )

from diphotons.MetaData.JobConfig import customize
## customize.campaign = "isolation_studies"
customize.setDefault("maxEvents",100)
customize(process)
