import FWCore.ParameterSet.Config as cms
import FWCore.Utilities.FileUtils as FileUtils
import FWCore.PythonUtilities.LumiList as LumiList  
import FWCore.ParameterSet.Types as CfgTypes   

isMC = True;

process = cms.Process("tnpAna")

process.load("FWCore.MessageService.MessageLogger_cfi")

process.load("Configuration.StandardSequences.GeometryDB_cff")
process.load("Configuration.StandardSequences.MagneticField_cff")
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff")

from Configuration.AlCa.GlobalTag import GlobalTag

if (isMC):
    process.GlobalTag = GlobalTag(process.GlobalTag, '76X_mcRun2_asymptotic_v12', '')
    print "76X_mcRun2_asymptotic_v12"
elif (isMC==False):
    process.GlobalTag = GlobalTag(process.GlobalTag, '76X_dataRun2_v15', '')
    print "76X_dataRun2_v15"

process.MessageLogger.cerr.FwkReport.reportEvery = cms.untracked.int32( 1000 )

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32( -1 ) )

process.source = cms.Source("PoolSource",
                            fileNames = cms.untracked.vstring('/store/group/phys_higgs/cmshgg/musella/flashgg/ExoPhys14_v2/diphotonsPhys14V2/RSGravToGG_kMpl001_M_5000_Tune4C_13TeV_pythia8/ExoPhys14_v2-diphotonsPhys14V2-v0-Phys14DR-PU20bx25_PHYS14_25_V1-v1/150128_133931/0000/myOutputFile_1.root'
                            )

# to apply the json file offline
if (isMC==False):
    print "applying 2015D rereco json"                                
    process.source.lumisToProcess = CfgTypes.untracked(CfgTypes.VLuminosityBlockRange())  
    JSONfile = '/afs/cern.ch/user/c/crovelli/public/json2015/rereco76x/singleEle/processedAndSilver_2015D_final.json'
    myLumis = LumiList.LumiList(filename = JSONfile).getCMSSWString().split(',')  
    process.source.lumisToProcess.extend(myLumis)                              

process.load("flashgg/MicroAOD/flashggPhotons_cfi")
process.load("flashgg/MicroAOD/flashggDiPhotons_cfi")
process.load("flashgg/MicroAOD/flashggElectrons_cfi")

process.TFileService = cms.Service("TFileService",fileName = cms.string("OUTPUT"))

process.tnpAna = cms.EDAnalyzer('TaPAnalyzer',
                                VertexTag = cms.untracked.InputTag('offlineSlimmedPrimaryVertices'),
                                ElectronTag=cms.InputTag('flashggSelectedElectrons'),
                                genPhotonExtraTag = cms.InputTag("flashggGenPhotonsExtra"),   
                                DiPhotonTag = cms.untracked.InputTag('flashggDiPhotons'),
                                PileUpTag = cms.untracked.InputTag('slimmedAddPileupInfo'),
                                rhoTag = cms.InputTag('fixedGridRhoAll'), 
                                rhoEleTag = cms.InputTag('fixedGridRhoFastjetAll'),
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

