from WMCore.Configuration import Configuration

config = Configuration()

config.section_('General')
config.General.requestName     = 'EXOSummer15_diphotons_746p2_DoubleEG_Run2015B'
config.General.transferLogs    = True
config.General.transferOutputs = True

config.section_('JobType')
config.JobType.pluginName      = 'Analysis'

# Name of the CMSSW configuration file
config.JobType.psetName        = 'tnp_MC.py'
config.JobType.priority        = 20

config.section_('Data')
# This string determines the primary dataset of the newly-produced outputs.
config.Data.inputDataset       = '/DoubleEG/musella-EXOSpring15_v2-Spring15BetaV2-2-gfceadad-v0-Run2015B-PromptReco-v1-ef29126b817954697c7731700c502b95/USER'

#config.Data.useParent = True
config.Data.inputDBS           = 'phys03'
config.Data.splitting          = 'FileBased'
config.Data.unitsPerJob        = 10
config.Data.totalUnits         = -1
config.Data.publication        = False

# This string is used to construct the output dataset name
#config.Data.publishDataName = 'CRAB3-tutorial'
config.Data.outLFNDirBase      =  '/store/user/spigazzi/'

config.section_('Site')
# Where the output files will be transmitted to
config.Site.storageSite        = 'T3_IT_MIB'
