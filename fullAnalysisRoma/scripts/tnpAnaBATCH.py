import FWCore.ParameterSet.Config as cms
import FWCore.Utilities.FileUtils as FileUtils

isMC = False;
is25ns = False;   
is2012B = False;    
is2012C = True;  
is2012D = False;   

process = cms.Process("tnpAna")

process.load("FWCore.MessageService.MessageLogger_cfi")

process.load("Configuration.StandardSequences.GeometryDB_cff")
process.load("Configuration.StandardSequences.MagneticField_cff")
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff")

from Configuration.AlCa.GlobalTag import GlobalTag

if (isMC and is25ns==False):
    process.GlobalTag = GlobalTag(process.GlobalTag, 'MCRUN2_74_V9A', '')
    print "MCRUN2_74_V9A"
elif (isMC and is25ns):
    process.GlobalTag = GlobalTag(process.GlobalTag, 'MCRUN2_74_V9', '')
    print "MCRUN2_74_V9"
elif (isMC==False and is2012B):
    process.GlobalTag = GlobalTag(process.GlobalTag, '74X_dataRun2_Prompt_v1', '')
    print "74X_dataRun2_Prompt_v1"
elif (isMC==False and is2012C):
    process.GlobalTag = GlobalTag(process.GlobalTag, '74X_dataRun2_Prompt_v1', '')
    print "74X_dataRun2_Prompt_v1"  
elif ((isMC==False and is2012D)):
    process.GlobalTag = GlobalTag(process.GlobalTag, '74X_dataRun2_Prompt_v2', '')
    print "74X_dataRun2_Prompt_v2"

process.MessageLogger.cerr.FwkReport.reportEvery = cms.untracked.int32( 1000 )

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32( -1 ) )

process.source = cms.Source("PoolSource",
                            fileNames = cms.untracked.vstring('/store/group/phys_higgs/cmshgg/musella/flashgg/ExoPhys14_v2/diphotonsPhys14V2/RSGravToGG_kMpl001_M_5000_Tune4C_13TeV_pythia8/ExoPhys14_v2-diphotonsPhys14V2-v0-Phys14DR-PU20bx25_PHYS14_25_V1-v1/150128_133931/0000/myOutputFile_1.root'
                            )

# to apply the json file offline
if (isMC==False and is2012C and is25ns):
    print "applying 2012C json for 25ns"                                
    process.source.lumisToProcess = cms.untracked.VLuminosityBlockRange("254231:1-254231:24", "254232:1-254232:81", "254790:90", "254790:93-254790:630", "254790:633-254790:697", "254790:701-254790:715", "254790:719-254790:784", "254852:47-254852:94", "254879:52", "254879:54-254879:140", "254906:1-254906:75", "254907:1-254907:52")

if (isMC==False and is2012C and is25ns==False):
    print "applying 2012C json for 50ns - only run 254833"                                
    process.source.lumisToProcess = cms.untracked.VLuminosityBlockRange("254833:43-254833:43", "254833:46-254833:861", "254833:863-254833:1505", "254833:1507-254833:1582", "254833:1584-254833:1632")

if (isMC==False and is2012B):
    print "applying 2012B json (50ns)"                                
    lumisToProcess = cms.untracked.VLuminosityBlockRange("251244:85-251244:86", "251244:88-251244:93", "251244:96-251244:121", "251244:123-251244:156", "251244:158-251244:428", "251244:430-251244:442", "251251:1-251251:31", "251251:33-251251:97", "251251:99-251251:167", "251252:1-251252:283", "251252:285-251252:505", "251252:507-251252:554", "251521:39-251521:42", "251561:1-251561:94", "251562:1-251562:439", "251562:443-251562:691", "251643:1-251643:216", "251643:222-251643:606", "251721:1-251721:36", "251883:56", "251883:58-251883:60", "251883:62-251883:437") 

process.load("flashgg/MicroAOD/flashggPhotons_cfi")
process.load("flashgg/MicroAOD/flashggDiPhotons_cfi")
process.load("flashgg/MicroAOD/flashggElectrons_cfi")

process.TFileService = cms.Service("TFileService",fileName = cms.string("OUTPUT"))

process.tnpAna = cms.EDAnalyzer('TaPAnalyzer',
                                VertexTag = cms.untracked.InputTag('offlineSlimmedPrimaryVertices'),
                                ElectronTag=cms.InputTag('flashggElectrons'),
                                genPhotonExtraTag = cms.InputTag("flashggGenPhotonsExtra"),   
                                DiPhotonTag = cms.untracked.InputTag('flashggDiPhotons'),
                                PileupTag = cms.untracked.InputTag('addPileupInfo'),
                                bits = cms.InputTag("TriggerResults","","HLT"),
                                objects = cms.InputTag("selectedPatTrigger"),
                                MetTag=cms.InputTag('slimmedMETs'),
                                generatorInfo = cms.InputTag("generator"),
                                dopureweight = PU,
                                sampleIndex  = SI,
                                puWFileName  = weights,
                                xsec         = XS,
                                kfac         = KF,
                                sumDataset   = SDS
                                )

process.p = cms.Path(process.tnpAna)

