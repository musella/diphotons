import FWCore.ParameterSet.Config as cms
import FWCore.Utilities.FileUtils as FileUtils

isMC = True;
is25ns = True;   
is2015B = False;    
is2015C = False;  
is2015D = False;   

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
elif (isMC==False and is2015B):
    process.GlobalTag = GlobalTag(process.GlobalTag, '74X_dataRun2_Prompt_v1', '')
    print "74X_dataRun2_Prompt_v1"
elif (isMC==False and is2015C):
    process.GlobalTag = GlobalTag(process.GlobalTag, '74X_dataRun2_Prompt_v1', '')
    print "74X_dataRun2_Prompt_v1"  
elif ((isMC==False and is2015D)):
    process.GlobalTag = GlobalTag(process.GlobalTag, '74X_dataRun2_Prompt_v2', '')
    print "74X_dataRun2_Prompt_v2"

process.MessageLogger.cerr.FwkReport.reportEvery = cms.untracked.int32( 1000 )

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32( -1 ) )

process.source = cms.Source("PoolSource",
                            fileNames = cms.untracked.vstring('/store/group/phys_higgs/cmshgg/musella/flashgg/ExoPhys14_v2/diphotonsPhys14V2/RSGravToGG_kMpl001_M_5000_Tune4C_13TeV_pythia8/ExoPhys14_v2-diphotonsPhys14V2-v0-Phys14DR-PU20bx25_PHYS14_25_V1-v1/150128_133931/0000/myOutputFile_1.root'
                            )

# to apply the json file offline
if (isMC==False and is2015D):
    print "applying 2015D json"                                
    process.source.lumisToProcess = cms.untracked.VLuminosityBlockRange("256630:5-256630:26","256673:55-256673:56","256674:1-256674:2","256675:1-256675:106","256675:111-256675:164","256677:1-256677:291","256677:293-256677:390","256677:392-256677:397","256677:400-256677:455","256677:457-256677:482","256729:11-256729:207","256729:209-256729:209","256729:213-256729:213","256729:220-256729:336","256729:346-256729:598","256729:600-256729:755","256729:758-256729:760","256729:765-256729:1165","256729:1167-256729:1292","256729:1295-256729:1327","256729:1329-256729:1732","256734:1-256734:57","256734:60-256734:178","256801:73-256801:263","256842:131-256842:132","256843:1-256843:204","256843:207-256843:284","256843:286-256843:378","256843:380-256843:430")

if (isMC==False and is2015C and is25ns):
    print "applying 2015C json for 25ns"                                
    process.source.lumisToProcess = cms.untracked.VLuminosityBlockRange("254231:1-254231:24", "254232:1-254232:81", "254790:90", "254790:93-254790:630", "254790:633-254790:697", "254790:701-254790:715", "254790:719-254790:784", "254852:47-254852:94", "254879:52", "254879:54-254879:140", "254906:1-254906:75", "254907:1-254907:52")

if (isMC==False and is2015C and is25ns==False):
    print "applying 2015C json for 50ns - only run 254833"                                
    process.source.lumisToProcess = cms.untracked.VLuminosityBlockRange("254833:43-254833:43", "254833:46-254833:861", "254833:863-254833:1505", "254833:1507-254833:1582", "254833:1584-254833:1632")

if (isMC==False and is2015B):
    print "applying 2015B json (50ns)"                                
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
                                reducedBarrelRecHitCollection = cms.InputTag('reducedEgamma','reducedEBRecHits'),
                                reducedEndcapRecHitCollection = cms.InputTag('reducedEgamma','reducedEERecHits'),
                                generatorInfo = cms.InputTag("generator"),
                                dopureweight = PU,
                                sampleIndex  = SI,
                                puWFileName  = weights,
                                xsec         = XS,
                                kfac         = KF,
                                sumDataset   = SDS
                                )

process.p = cms.Path(process.tnpAna)

