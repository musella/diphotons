import FWCore.ParameterSet.Config as cms

process = cms.Process("TagProbe")
process.source = cms.Source("EmptySource")
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(1) )

process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.destinations = ['cout', 'cerr']
process.MessageLogger.cerr.FwkReport.reportEvery = 1000

###############################################
InputFileName = "/afs/cern.ch/work/c/crovelli/public/TaP_spring15_7415v2/topup/formattedZ/Formatted_singleEle2015D_all.root"
OutputFilePrefix = "efficiency-data-"
PDFName = "pdfSignalPlusBackground"

################################################
EfficiencyBins = cms.PSet(probe_pt = cms.vdouble( 20, 30 ), 
                          probe_absEta = cms.vdouble( 0.0, 1.5 ), 
                          ) 

EfficiencyBinningSpecification = cms.PSet(
    UnbinnedVariables = cms.vstring("mass"),
    BinnedVariables = cms.PSet(EfficiencyBins),
    BinToPDFmap = cms.vstring(PDFName)
)

mcTruthModules = cms.PSet()

############################################################################################
process.GsfElectronToId = cms.EDAnalyzer("TagProbeFitTreeAnalyzer",
                                         InputFileNames = cms.vstring(InputFileName),
                                         InputDirectoryName = cms.string("tnpAna"),
                                         InputTreeName = cms.string("TaPTree"), 
                                         OutputFileName = cms.string(OutputFilePrefix+"FullSel.root"),
                                         NumCPU = cms.uint32(1),
                                         SaveWorkspace = cms.bool(False),
                                         doCutAndCount = cms.bool(False),
                                         floatShapeParameters = cms.bool(True),
                                         binnedFit = cms.bool(True),
                                         binsForFit = cms.uint32(40),            # 20-200 EB
                                         #binsForFit = cms.uint32(30),            # 200-350 EB
                                         #binsForFit = cms.uint32(20),            # >=350 EB
                                         #binsForFit = cms.uint32(40),             # EE
                                         # defines all the real variables of the probes available in the input tree and intended for use in the efficiencies
                                         Variables = cms.PSet(mass = cms.vstring("Tag-Probe Mass", "70.0", "110.0", "GeV/c^{2}"),
                                                              probe_pt = cms.vstring("Probe E_{T}", "0", "500", "GeV/c"),
                                                              probe_absEta = cms.vstring("Probe #eta", "0", "2.5", ""),
                                                              ),

                                         # defines all the discrete variables of the probes available in the input tree and intended for use in the efficiency calculations
                                         Categories = cms.PSet(
        probe_fullsel = cms.vstring("probe_fullsel", "dummy[pass=1,fail=0]"),
                                                               ),

                                         # defines all the PDFs that will be available for the efficiency calculations; 
                                         PDFs = cms.PSet(pdfSignalPlusBackground = cms.vstring(


            # barrel signal
            "RooGaussian::signalResPass(mass, meanP[-0.5,-5.000,1.000],sigmaP[0.956,0.00,5.000])",      #20-40 x fondo nominale 
            #"RooGaussian::signalResPass(mass, meanP[0.,-2.000,0.500],sigmaP[1.5,0.00,5.000])",          #20-40 x sistematica fondo
            #"RooGaussian::signalResPass(mass, meanP[0.5,-1.000,3.000],sigmaP[2.,0.10,5.000])",          #40-50
            #"RooGaussian::signalResPass(mass, meanP[0.5,-1.000,5.000],sigmaP[2.,0.50,5.000])",          #50-80
            #"RooGaussian::signalResPass(mass, meanP[0.0,-1.000,3.000],sigmaP[0.956,0.00,5.000])",       #80-150
            #"RooGaussian::signalResPass(mass, meanP[0.0,-2.000,5.000],sigmaP[0.956,0.50,1.300])",       #150-200
            #"RooGaussian::signalResPass(mass, meanP[0.6,0.,1.2],sigmaP[2.,1.00,5.000])",                #200-350
            #"RooGaussian::signalResPass(mass, meanP[2.1,1.,3.5],sigmaP[1.,0.5,3.50])",                  #>350 x fondo nominale 
            #"RooGaussian::signalResPass(mass, meanP[1.3,-0.5,2.5],sigmaP[2.7,2.,3.])",                  #>350 x sistematica fondo 
            "RooGaussian::signalResFail(mass, meanF[0.0,-1.000,5.000],sigmaF[0.956,0.00,5.000])",       #20-80 
            #"RooGaussian::signalResFail(mass, meanF[-1.0,-3.000,2.000],sigmaF[0.956,0.00,5.000])",      #80-110 
            #"RooGaussian::signalResFail(mass, meanF[0.0,-1.000,5.000],sigmaF[0.956,0.00,2.000])",       #110-150
            #"RooGaussian::signalResFail(mass, meanF[0.0,-1.000,5.000],sigmaF[2.,0.50,4.000])",          #150-200
            #"RooGaussian::signalResFail(mass, meanF[-0.5,-1.,1.],sigmaF[0.5.,0.4,2.000])",              #200-270
            #"RooGaussian::signalResFail(mass, meanF[-0.5,-1.000,0.0],sigmaF[0.956,0.00,5.000])",        #270-350
            #"RooGaussian::signalResFail(mass, meanF[1.,0.5,1.5],sigmaF[0.4,0.,0.5])",                   #>350 x fondo nominale  
            #"RooGaussian::signalResFail(mass, meanF[0.9,0.,2.],sigmaF[0.06,0.00,0.5])",                 #>350 x sistematica fondo    
            

            # endcap signal
            #"RooGaussian::signalResPass(mass, meanP[.0,-4.000,2.000],sigmaP[0.956,0.00,5.000])",       # 20-50 x fondo nominale 
            #"RooGaussian::signalResPass(mass, meanP[.0,-4.000,2.000],sigmaP[0.956,0.00,5.000])",       # 20-30 x sistematica fondo
            #"RooGaussian::signalResPass(mass, meanP[0,-2.000,2.000],sigmaP[0.956,0.00,5.000])",        # 30-40 x sistematica fondo 
            #"RooGaussian::signalResPass(mass, meanP[.0,-4.000,2.000],sigmaP[0.956,0.00,5.000])",       # 40-50 x sistematica fondo
            #"RooGaussian::signalResPass(mass, meanP[.0,-4.000,2.000],sigmaP[1.5,1.00,5.000])",         # 50-80
            #"RooGaussian::signalResPass(mass, meanP[.0,-2.000,2.000],sigmaP[0.956,0.00,2.000])",       # 80-110 x fondo nominale   
            #"RooGaussian::signalResPass(mass, meanP[.0,-2.000,2.000],sigmaP[0.956,0.00,1.100])",       # 80-110 x sistematica fondo  
            #"RooGaussian::signalResPass(mass, meanP[-0.5,-3.000,1.00],sigmaP[0.956,0.00,5.000])",      # 110-150
            #"RooGaussian::signalResPass(mass, meanP[0.,-2.000,2.000],sigmaP[0.956,1.00,5.000])",       # 150-200
            #"RooGaussian::signalResPass(mass, meanP[-1.,-1.5,1.5],sigmaP[0.956,0.20,2.000])",          # >=200
            #"RooGaussian::signalResFail(mass, meanF[.0,-3.000,3.000],sigmaF[0.956,0.00,5.000])",       # 20-80 
            #"RooGaussian::signalResFail(mass, meanF[.0,-5.000,5.000],sigmaF[0.956,0.00,5.000])",       # 80-110
            #"RooGaussian::signalResFail(mass, meanF[.0,-4.000,3.000],sigmaF[0.956,0.00,5.000])",       # 110-150
            #"RooGaussian::signalResFail(mass, meanF[1,-1.000,3.000],sigmaF[0.956,0.00,5.000])",        # 150-200
            #"RooGaussian::signalResFail(mass, meanF[0,-0.5,1.5],sigmaF[1,0.5,2.])",                    # >=200
            
            
            # both EB and EE signal            
            "ZGeneratorLineShape::signalPhyPass(mass,\"MCtemplates.root\", \"hMass_20.000000To30.000000_0.000000To1.500000_Pass\")",
            "ZGeneratorLineShape::signalPhyFail(mass,\"MCtemplates.root\", \"hMass_20.000000To30.000000_0.000000To1.500000_Fail\")",

            "FCONV::signalPass(mass, signalPhyPass, signalResPass)",
            "FCONV::signalFail(mass, signalPhyFail, signalResFail)",


            # nominal background fit
            "RooExponential::backgroundPass(mass, aPass[-0.1, -1., 0.])",      # 20-40
            #"RooExponential::backgroundPass(mass, aPass[-0.1, -1., 0.1])",    # 40-60 
            #"RooExponential::backgroundPass(mass, aPass[-0.1, -1., 0.2])",    # >60
            "RooExponential::backgroundFail(mass, aFail[-0.1, -1., 0.1])",     # 20-110 EB, all EE
            #"RooExponential::backgroundFail(mass, aFail[-0.1, -1., 0.2])",    # >110, EB


            # to study the background fit systematics
            # EB 
            #"RooCMSShape::backgroundPass(mass, alphaPass[60.,50.,120.], betaPass[0.0001, 0.,1.], gammaPass[0.01, 0.,.5], peakPass[90.0])",         # 20-30  
            #"RooCMSShape::backgroundPass(mass, alphaPass[60.,50.,120.], betaPass[0.0001, 0.,1.], gammaPass[0.001, 0.,.5], peakPass[90.0])",        # 30-110
            #"RooCMSShape::backgroundPass(mass, alphaPass[80.,60.,140.], betaPass[0.001, 0.,0.1], gammaPass[0.01, -2.,3], peakPass[90.0])",         # 110-150
            #"RooCMSShape::backgroundPass(mass, alphaPass[80.,60.,140.], betaPass[0.001, 0.,0.3], gammaPass[0.001, 0.,1], peakPass[90.0])",         # 150-350
            #"RooCMSShape::backgroundPass(mass, alphaPass[80.,60.,140.], betaPass[0.001, 0.,0.3], gammaPass[0.01, -2.,3], peakPass[90.0])",         # 350-500
            #"RooCMSShape::backgroundFail(mass, alphaFail[60.,50.,80.], betaFail[0.001, 0.,0.1], gammaFail[0.0001, -1.,1.0], peakFail[90.0])",      # 20-50
            #"RooCMSShape::backgroundFail(mass, alphaFail[60.,50.,100.], betaFail[0.001, 0.,0.1], gammaFail[0.0001, -1.,1.0], peakFail[90.0])",     # 50-110
            #"RooCMSShape::backgroundFail(mass, alphaFail[60.,50.,80.], betaFail[0.001, 0.,1.], gammaFail[0.0001, -1.,1.0], peakFail[90.0])",       # 110-350
            #"RooCMSShape::backgroundFail(mass, alphaFail[60.,50.,81.], betaFail[0.001, 0.,1.], gammaFail[0.0001, -1.,1.0], peakFail[90.0])",        # 350-500

            # EE
            #"RooCMSShape::backgroundPass(mass, alphaPass[100.,50.,130.], betaPass[0.001, 0.,5.], gammaPass[0.0001, 0.,1.0], peakPass[90.0])",       # 20-40      
            #"RooCMSShape::backgroundFail(mass, alphaFail[60.,50.,70.], betaFail[0.001, 0.,0.1], gammaFail[0.001, -1.,1.0], peakFail[90.0])",        # 20-40
            #"RooCMSShape::backgroundPass(mass, alphaPass[100.,50.,130.], betaPass[0.001, 0.,5.], gammaPass[0.001, 0.,2.0], peakPass[90.0])",        # 40-50
            #"RooCMSShape::backgroundFail(mass, alphaFail[60.,50.,70.], betaFail[0.001, 0.,0.1], gammaFail[0.0001, -1.,1.0], peakFail[90.0])",       # 40-50
            #"RooCMSShape::backgroundPass(mass, alphaPass[100.,50.,130.], betaPass[0.001, 0.,5.], gammaPass[0.001, -1.,2.0], peakPass[90.0])",       # 50-80
            #"RooCMSShape::backgroundFail(mass, alphaFail[60.,50.,70.], betaFail[0.001, 0.,0.1], gammaFail[0.0001, -1.,1.0], peakFail[90.0])",       # 50-80
            #"RooCMSShape::backgroundPass(mass, alphaPass[100.,50.,130.], betaPass[0.001, 0.,12.], gammaPass[0.001, -1.,6.0], peakPass[90.0])",      # 80-110
            #"RooCMSShape::backgroundFail(mass, alphaFail[60.,50.,70.], betaFail[0.001, 0.,0.1], gammaFail[0.0001, -1.,1.0], peakFail[90.0])",       # 80-110
            #"RooCMSShape::backgroundPass(mass, alphaPass[100.,50.,130.], betaPass[0.001, 0.,15.], gammaPass[0.001, -1.,6.], peakPass[90.0])",       # 110-150
            #"RooCMSShape::backgroundFail(mass, alphaFail[90.,60.,120.], betaFail[0.001, 0.,0.2], gammaFail[0.0001, -1.,1.0], peakFail[90.0])",      # 110-150
            #"RooCMSShape::backgroundPass(mass, alphaPass[110.,100.,120.], betaPass[0.01, 0.,6.], gammaPass[0.0001, -1.,1.5], peakPass[90.0])",      # 150-500
            #"RooCMSShape::backgroundFail(mass, alphaFail[90.,60.,120.], betaFail[0.01, 0.,0.15], gammaFail[0.0001, -1.,1.0], peakFail[90.0])",      # 150-500

            "efficiency[0.5,0,1]",
            "signalFractionInPassing[0.9,0.,1]"
            #"signalFractionInPassing[0.99,0.8,1]"
            ),
                                                         ),

                                         # defines a set of efficiency calculations, what PDF to use for fitting and how to bin the data;
                                         # there will be a separate output directory for each calculation that includes a simultaneous fit, side band subtraction and counting. 
                                         Efficiencies = cms.PSet(
        MCtruth_Tight = cms.PSet(EfficiencyBinningSpecification,
                                 EfficiencyCategoryAndState = cms.vstring("probe_fullsel", "pass"),
                                                                                   ),
                                                                 )
                                         )

process.fit = cms.Path(
    process.GsfElectronToId  
    )
