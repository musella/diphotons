import FWCore.ParameterSet.Config as cms
import FWCore.Utilities.FileUtils as FileUtils

from flashgg.MicroAODProducers.flashggPreselectedDiPhotons_cfi import flashggPreselectedDiPhotons

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
                               histograms = cms.VPSet()
        )
    
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
        
    num = mkHistogrammer(process,"num_%s" % name, name)
    den = mkHistogrammer(process,"den_%s" % name, name)

    return (photons,num,den)


process = cms.Process("Analysis")

process.load("FWCore.MessageService.MessageLogger_cfi")

process.load("Configuration.StandardSequences.GeometryDB_cff")
process.load("Configuration.StandardSequences.MagneticField_cff")
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
process.GlobalTag.globaltag = 'POSTLS170_V5::All'
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )
process.MessageLogger.cerr.FwkReport.reportEvery = cms.untracked.int32( 1000 )

# Uncomment the following if you notice you have a memory leak
# This is a lightweight tool to digg further
#process.SimpleMemoryCheck = cms.Service("SimpleMemoryCheck",
#                                        ignoreTotal = cms.untracked.int32(1),
#                                        monitorPssAndPrivate = cms.untracked.bool(True)
#                                       )

process.source = cms.Source("PoolSource",
                            fileNames=cms.untracked.vstring(
        ## "/store/group/phys_higgs/cmshgg/flashgg/ExoPhys14/alphaV1-96-g5e4dc54/RSGravToGG_kMpl001_M_5000_Tune4C_13TeV_pythia8/ExoPhys14-alphaV1-96-g5e4dc54-v0-Phys14DR-PU20bx25_PHYS14_25_V1-v1/141111_110141/0000/myOutputFile_2.root"
        ## "/store/mc/Phys14DR/RSGravToGG_kMpl-02_M-5000_Tune4C_13TeV-pythia8/MINIAODSIM/PU20bx25_PHYS14_25_V1-v1/00000/5035D51F-2D68-E411-BD0A-00259073E42E.root"

        "/store/group/phys_higgs/cmshgg/flashgg/ExoPhys14/alphaV1-96-g5e4dc54/RSGravToGG_kMpl-02_M-5000_Tune4C_13TeV-pythia8/ExoPhys14-alphaV1-96-g5e4dc54-v1-Phys14DR-PU20bx25_PHYS14_25_V1-v1/141112_020912/0000/myOutputFile_1.root",
        "/store/group/phys_higgs/cmshgg/flashgg/ExoPhys14/alphaV1-96-g5e4dc54/RSGravToGG_kMpl-02_M-5000_Tune4C_13TeV-pythia8/ExoPhys14-alphaV1-96-g5e4dc54-v1-Phys14DR-PU20bx25_PHYS14_25_V1-v1/141112_020912/0000/myOutputFile_2.root",
        "/store/group/phys_higgs/cmshgg/flashgg/ExoPhys14/alphaV1-96-g5e4dc54/RSGravToGG_kMpl-02_M-5000_Tune4C_13TeV-pythia8/ExoPhys14-alphaV1-96-g5e4dc54-v1-Phys14DR-PU20bx25_PHYS14_25_V1-v1/141112_020912/0000/myOutputFile_3.root",

        ## "/store/group/phys_higgs/cmshgg/flashgg/ExoPhys14/alphaV1-96-g5e4dc54/RSGravToGG_kMpl-001_M-1500_Tune4C_13TeV-pythia8/ExoPhys14-alphaV1-96-g5e4dc54-v1-Phys14DR-PU20bx25_PHYS14_25_V1-v1/141112_020908/0000/myOutputFile_2.root", 
        ## "/store/group/phys_higgs/cmshgg/flashgg/ExoPhys14/alphaV1-96-g5e4dc54/RSGravToGG_kMpl-001_M-1500_Tune4C_13TeV-pythia8/ExoPhys14-alphaV1-96-g5e4dc54-v1-Phys14DR-PU20bx25_PHYS14_25_V1-v1/141112_020908/0000/myOutputFile_3.root", 
        )
)


## process.load("flashgg/MicroAODProducers/flashggVertexMaps_cfi")
## process.load("flashgg/MicroAODProducers/flashggPhotons_cfi")
## process.load("flashgg/MicroAODProducers/flashggDiPhotons_cfi")

process.out = cms.OutputModule("PoolOutputModule", fileName = cms.untracked.string('myOutputFile.root'),
                               outputCommands = cms.untracked.vstring('drop *')
                               )

process.TFileService = cms.Service("TFileService",
                                   ## fileName = cms.string("histograms_15TeV_OrNoHE.root")
                                   fileName = cms.string("histograms_5TeV_OrNoHE.root")
)

### process.den = cms.EDAnalyzer('DiPhotonsHistoAnalyzer',
###                                src = cms.InputTag('flashggDiPhotons'),
###                                histograms = cms.VPSet(
###         cms.PSet( itemsToPlot = cms.untracked.int32(1),
###                   min = cms.untracked.double(500),
###                   max = cms.untracked.double(3000),
###                   nbins = cms.untracked.int32(250),
###                   name = cms.untracked.string("leadPt"),
###                   description = cms.untracked.string("leadPt"),
###                   plotquantity = cms.untracked.string("leadingPhoton.pt"),
###                   )
###         
###         )
### )
### 
### process.num = process.den.clone()

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

    print str(photons)
    print str(num)
    print str(den)

print photonColls
print nums
print dens
    
process.load('HLTrigger.HLTfilters.hltHighLevel_cfi')
process.hltHighLevel.HLTPaths = ["HLT_DoublePhoton85*","HLT_Photon250_NoHE*"]

## process.microAOD = cms.Sequence(((process.flashggVertexMapUnique+process.flashggVertexMapNonUnique)*
##                       process.flashggPhotons*
##                       process.flashggDiPhotons))
## 
process.p1 = cms.Path(## process.microAOD*
                      photonColls*dens
                    )

process.p2 = cms.Path(process.hltHighLevel*
                      ## process.microAOD*
                      photonColls*nums
                    )

process.e = cms.EndPath(process.out)
