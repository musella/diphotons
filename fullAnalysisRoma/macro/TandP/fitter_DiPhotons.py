import FWCore.ParameterSet.Config as cms

process = cms.Process("TagProbe")
process.source = cms.Source("EmptySource")
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(1) )

process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.destinations = ['cout', 'cerr']
process.MessageLogger.cerr.FwkReport.reportEvery = 1000

################################################
InputFileName = "/afs/cern.ch/work/c/crovelli/public/TaP_spring15_7415v2/topup/formattedZ/Formatted_DYLL_all__1pb.root"
OutputFilePrefix = "efficiency-mc-fullSel"
PDFName = "pdfSignalPlusBackground"

################################################
#specifies the binning of parameters
EfficiencyBins = cms.PSet(probe_pt = cms.vdouble( 20, 30 ),
                          probe_absEta = cms.vdouble( 0.0, 1.5 ),
                          )

EfficiencyBinningSpecificationMC = cms.PSet(
    UnbinnedVariables = cms.vstring("mass", "pu_weight"),                 # NB: using pu_weight only 1) not to have too small weights 2) to use the mc statistical error
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
                                         binsForFit = cms.uint32(40),            # 20-200 EB  
                                         #binsForFit = cms.uint32(30),            # 200-350 EB
                                         #binsForFit = cms.uint32(20),            # >=350 EB
                                         #binsForFit = cms.uint32(40),            # 20-110 EE  
                                         #binsForFit = cms.uint32(25),            # >=110 EE  
                                         WeightVariable = cms.string("pu_weight"),
                                         
                                         # defines all the real variables of the probes available in the input tree and intended for use in the efficiencies
                                         Variables = cms.PSet(mass = cms.vstring("Tag-Probe Mass", "70.0", "110.0", "GeV/c^{2}"),
                                                              probe_pt = cms.vstring("Probe E_{T}", "0", "500", "GeV/c"),
                                                              probe_absEta = cms.vstring("Probe #eta", "0", "2.5", ""), 
                                                              pu_weight = cms.vstring("PU weight only", "-1000", "1000", ""),
                                                              ),

                                         # defines all the discrete variables of the probes available in the input tree and intended for use in the efficiency calculations
                                         Categories = cms.PSet(
                                                               probe_fullsel = cms.vstring("probe_fullsel", "dummy[pass=1,fail=0]"),
                                                               ),

                                         # defines all the PDFs that will be available for the efficiency calculations; 
                                         PDFs = cms.PSet(pdfSignalPlusBackground = cms.vstring(

            # Free fit to fix N in EB
            "RooCBExGaussShape::signalResPass(mass,meanP[0.0,-1.,1.],sigmaP[1.,0.01,3.1],alphaP[1.,0.01,5.0],nP[2.,0.1,50.000],sigmaP_2[1.000,0.1,15.00])",    # 20-30
            #"RooCBExGaussShape::signalResPass(mass,meanP[0.0,-1.,1.],sigmaP[2.,0.01,3.1],alphaP[1.,0.01,5.0],nP[2.,0.1,50.000],sigmaP_2[1.000,0.1,15.00])",    # 30-350
            #"RooCBExGaussShape::signalResPass(mass,meanP[0.0,-2.,2.],sigmaP[1.,0.01,3.1],alphaP[1.,0.01,5.0],nP[2.,0.1,50.000],sigmaP_2[1.000,0.1,15.00])",    # >350
            "RooCBExGaussShape::signalResFail(mass,meanF[0.0,-1.,1.],sigmaF[3.,0.01,2.],alphaF[1.,0.,5.0],nF[3,0.01,25.0],sigmaF_2[1.,0.001,15.000])",         # 20-40
            #"RooCBExGaussShape::signalResFail(mass,meanF[0.0,-1.,3.],sigmaF[3.,0.01,2.],alphaF[1.,0.,5.0],nF[3,0.01,25.0],sigmaF_2[1.,0.001,10.000])",         # 40-50
            #"RooCBExGaussShape::signalResFail(mass,meanF[1.2,-0.5,3.],sigmaF[1.,0.01,5],alphaF[2.,0.01,8.0],nF[3,0.01,25.0],sigmaF_2[1.,0.001,5.000])",        # 50-60
            #"RooCBExGaussShape::signalResFail(mass,meanF[1.2,-2.,3.],sigmaF[1.,0.01,5],alphaF[2.,0.01,8.0],nF[3,0.01,25.0],sigmaF_2[1.,0.001,5.000])",         # 60-150
            #"RooCBExGaussShape::signalResFail(mass,meanF[-1.5,-3.,-0.5],sigmaF[1.,0.01,5],alphaF[2.,0.01,8.0],nF[3,0.01,25.0],sigmaF_2[1.,0.001,3.000])",      # 150-350
            #"RooCBExGaussShape::signalResFail(mass,meanF[-0.5,-3.,0.5],sigmaF[1.,0.01,5],alphaF[2.,0.01,8.0],nF[3,0.01,10.0],sigmaF_2[1.,0.001,3.000])",       # >350

            # Free fit to fix N in EE
            #"RooCBExGaussShape::signalResPass(mass,meanP[0.0,-5.,5.],sigmaP[1.,0.01,5.0],alphaP[1.,0.01,50.0],nP[2.,0.1,50.000],sigmaP_2[1.000,0.1,15.00])",     # 20-30 
            #"RooCBExGaussShape::signalResPass(mass,meanP[0.0,-5.,5.],sigmaP[1.,0.01,5.0],alphaP[1.,0.01,50.0],nP[2.,0.1,50.000],sigmaP_2[1.000,0.01,15.00])",    # 30-50 
            #"RooCBExGaussShape::signalResPass(mass,meanP[-0.5,-3.,1.],sigmaP[1.,0.01,5.0],alphaP[1.,0.01,50.0],nP[2.,0.1,50.000],sigmaP_2[1.000,0.1,15.00])",    # 50-60
            #"RooCBExGaussShape::signalResPass(mass,meanP[-0.5,-3.,0.5],sigmaP[1.,0.01,5.0],alphaP[1.,0.01,50.0],nP[2.,0.1,50.000],sigmaP_2[1.000,0.1,15.00])",   # 60-
            #"RooCBExGaussShape::signalResFail(mass,meanF[0.0,-5.,5.],sigmaF[3.,0.01,5.0],alphaF[1.,0.,5.0],nF[3,0.1,10.0],sigmaF_2[1.,0.001,15.000])",          # 20-30
            #"RooCBExGaussShape::signalResFail(mass,meanF[0.0,-5.,5.],sigmaF[3.,0.01,5.0],alphaF[1.,0.,5.0],nF[3,0.1,50.0],sigmaF_2[1.,0.001,15.000])",          # 30-50 
            #"RooCBExGaussShape::signalResFail(mass,meanF[1.,-1.,2.],sigmaF[3.,0.01,5.0],alphaF[1.,0.,5.0],nF[3,0.1,50.0],sigmaF_2[1.,0.001,15.000])",           # 50-60 
            #"RooCBExGaussShape::signalResFail(mass,meanF[-0.5,-2.,0.],sigmaF[3.,0.01,5.0],alphaF[1.,0.,5.0],nF[3,0.1,50.0],sigmaF_2[1.,0.001,15.000])",         # 60-150
            #"RooCBExGaussShape::signalResFail(mass,meanF[-1.5,-2.,-1.],sigmaF[3.,0.01,5.0],alphaF[1.,0.,5.0],nF[3,0.1,50.0],sigmaF_2[1.,0.001,15.000])",         # 150-500 
            
            "ZGeneratorLineShape::signalPhy(mass)", ### NLO line shape

            "RooExponential::backgroundPass(mass, aPass[-0.1, -3., 0.])",    
            "RooExponential::backgroundFail(mass, aFail[-0.1, -3., 0.1])",   

            "FCONV::signalPass(mass, signalPhy, signalResPass)",
            "FCONV::signalFail(mass, signalPhy, signalResFail)",     
            "efficiency[0.5,0,1]",
            "signalFractionInPassing[0.9,0,1]"     
            ),
                                                         ),

                                         Efficiencies = cms.PSet(mcTruthModules
                                                                 )

                                         )



process.fit = cms.Path(
    process.GsfElectronToId  
    )
