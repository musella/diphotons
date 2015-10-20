import FWCore.ParameterSet.Config as cms

process = cms.Process("TagProbe")
process.source = cms.Source("EmptySource")
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(1) )

process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.destinations = ['cout', 'cerr']
process.MessageLogger.cerr.FwkReport.reportEvery = 1000

################################################
##                      _              _       
##   ___ ___  _ __  ___| |_ __ _ _ __ | |_ ___ 
##  / __/ _ \| '_ \/ __| __/ _` | '_ \| __/ __|
## | (_| (_) | | | \__ \ || (_| | | | | |_\__ \
##  \___\___/|_| |_|___/\__\__,_|_| |_|\__|___/
##                                              
################################################

InputFileName = "/afs/cern.ch/work/c/crovelli/public/TaP_spring15_7412v2/formattedZ/Formatted_DYLL_all__553pb.root"
OutputFilePrefix = "efficiency-mc-fullSel"
PDFName = "pdfSignalPlusBackground"

################################################
#specifies the binning of parameters
EfficiencyBins = cms.PSet(probe_pt = cms.vdouble( 20, 30 ),
                          probe_absEta = cms.vdouble( 0.0, 1.5 ),
                          )

EfficiencyBinningSpecificationMC = cms.PSet(
    UnbinnedVariables = cms.vstring("mass", "weight"),                 # NB: weight includes everything
    BinnedVariables = cms.PSet(EfficiencyBins,
                               ),
    BinToPDFmap = cms.vstring(PDFName)  
)

############################################################################################

mcTruthModules = cms.PSet(
    MCtruth_Tight = cms.PSet(EfficiencyBinningSpecificationMC,
                              EfficiencyCategoryAndState = cms.vstring("probe_fullsel", "pass"),
                             ),
    )

############################################################################################
############################################################################################
####### GsfElectron->Id / selection efficiency 
############################################################################################
############################################################################################

process.GsfElectronToId = cms.EDAnalyzer("TagProbeFitTreeAnalyzer",
                                         InputFileNames = cms.vstring(InputFileName),
                                         InputDirectoryName = cms.string("tnpAna"),
                                         InputTreeName = cms.string("TaPTree"), 
                                         OutputFileName = cms.string(OutputFilePrefix+"FullSel.root"),
                                         NumCPU = cms.uint32(1),
                                         SaveWorkspace = cms.bool(False), 
                                         doCutAndCount = cms.bool(True),
                                         floatShapeParameters = cms.bool(True),
                                         binnedFit = cms.bool(True),
                                         binsForFit = cms.uint32(40),
                                         WeightVariable = cms.string("weight"),
                                         
                                         # defines all the real variables of the probes available in the input tree and intended for use in the efficiencies
                                         Variables = cms.PSet(mass = cms.vstring("Tag-Probe Mass", "70.0", "110.0", "GeV/c^{2}"),
                                                              probe_pt = cms.vstring("Probe E_{T}", "0", "500", "GeV/c"),
                                                              probe_absEta = cms.vstring("Probe #eta", "0", "2.5", ""), 
                                                              weight = cms.vstring("Total weight", "0", "100", ""),
                                                              ),

                                         # defines all the discrete variables of the probes available in the input tree and intended for use in the efficiency calculations
                                         Categories = cms.PSet(
                                                               probe_fullsel = cms.vstring("probe_fullsel", "dummy[pass=1,fail=0]"),
                                                               ),

                                         # defines all the PDFs that will be available for the efficiency calculations; 
                                         # uses RooFit's "factory" syntax;
                                         # each pdf needs to define "signal", "backgroundPass", "backgroundFail" pdfs, "efficiency[0.9,0,1]" 
                                         # and "signalFractionInPassing[0.9]" are used for initial values  
                                         PDFs = cms.PSet(pdfSignalPlusBackground = cms.vstring(

            # Free fit to fix N in EB
            "RooCBExGaussShape::signalResPass(mass,meanP[0.0,-1.,1.],sigmaP[1.,0.01,3.1],alphaP[1.,0.01,5.0],nP[2.,0.1,5.000],sigmaP_2[1.000,0.1,15.00])",   
            "RooCBExGaussShape::signalResFail(mass,meanF[0.0,-1.,1.],sigmaF[3.,0.01,4.],alphaF[1.,0.,5.0],nF[3,0.01,25.0],sigmaF_2[1.,0.001,15.000])",     # 20-50
            #"RooCBExGaussShape::signalResFail(mass,meanF[1.2,-1.,1.5],sigmaF[1.,0.01,3.1],alphaF[1.,0.,5.0],nF[10,0.01,50.0],sigmaF_2[1.,0.001,3.000])",   # >=50

            # Free fit to fix N in EE
            #"RooCBExGaussShape::signalResPass(mass,meanP[0.0,-5.,5.],sigmaP[1.,0.01,5.0],alphaP[1.,0.01,50.0],nP[2.,0.1,50.000],sigmaP_2[1.000,0.1,15.00])",   # 20-50 
            #"RooCBExGaussShape::signalResPass(mass,meanP[-0.5,-3.,0.],sigmaP[1.,0.01,5.0],alphaP[1.,0.01,50.0],nP[2.,0.1,50.000],sigmaP_2[1.000,0.1,15.00])",   # >=50
            #"RooCBExGaussShape::signalResFail(mass,meanF[0.0,-5.,5.],sigmaF[3.,0.01,5.0],alphaF[1.,0.,5.0],nF[3,0.1,10.0],sigmaF_2[1.,0.001,15.000])",         # 20-30
            #"RooCBExGaussShape::signalResFail(mass,meanF[0.0,-5.,5.],sigmaF[3.,0.01,5.0],alphaF[1.,0.,5.0],nF[3,0.1,50.0],sigmaF_2[1.,0.001,15.000])",         # 30-50 
            #"RooCBExGaussShape::signalResFail(mass,meanF[-0.5,-3.,0.],sigmaF[3.,0.01,5.0],alphaF[1.,0.,5.0],nF[3,0.1,50.0],sigmaF_2[1.,0.001,15.000])",        # 50-60 
            #"RooCBExGaussShape::signalResFail(mass,meanF[-0.5,-2.,0.],sigmaF[3.,0.01,5.0],alphaF[1.,0.,5.0],nF[3,0.1,50.0],sigmaF_2[1.,0.001,15.000])",         # >=60

            "ZGeneratorLineShape::signalPhy(mass)", ### NLO line shape

            "RooExponential::backgroundPass(mass, aPass[-0.1, -1., 0.])",    
            "RooExponential::backgroundFail(mass, aFail[-0.1, -1., 0.1])",   

            "FCONV::signalPass(mass, signalPhy, signalResPass)",
            "FCONV::signalFail(mass, signalPhy, signalResFail)",     
            "efficiency[0.5,0,1]",
            "signalFractionInPassing[1.0]"     
            ),
                                                         ),

                                         Efficiencies = cms.PSet(mcTruthModules
                                                                 )

                                         )



process.fit = cms.Path(
    process.GsfElectronToId  
    )
