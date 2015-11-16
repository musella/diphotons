import FWCore.ParameterSet.Config as cms

process = cms.Process("TagProbe")
process.source = cms.Source("EmptySource")
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(1) )

process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.destinations = ['cout', 'cerr']
process.MessageLogger.cerr.FwkReport.reportEvery = 1000

################################################
InputFileName = "/afs/cern.ch/work/c/crovelli/public/TaP_spring15_7412v2/formattedZ/Formatted_singleEle2015D_all.root"
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
                                         binsForFit = cms.uint32(40),          # 20-150 EB
                                         #binsForFit = cms.uint32(30),          # >=150 EB
                                         #binsForFit = cms.uint32(40),          # 20-80 EE
                                         #binsForFit = cms.uint32(25),           # >=80 EE
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
            "RooGaussian::signalResPass(mass, meanP[.0,-5.000,5.000],sigmaP[0.956,0.00,5.000])",     #20-80
            #"RooGaussian::signalResPass(mass, meanP[.0,-1.000,5.000],sigmaP[0.956,0.00,5.000])",     #80-110
            #"RooGaussian::signalResPass(mass, meanP[.0,-5.000,2.000],sigmaP[0.956,0.00,5.000])",      #>=110
            "RooGaussian::signalResFail(mass, meanF[.0,-5.000,5.000],sigmaF[0.956,0.00,5.000])",     #20-110 
            #"RooGaussian::signalResFail(mass, meanF[.0,-5.000,5.000],sigmaF[0.956,0.00,5.000])",      #>=110

            # endcap signal
            #"RooGaussian::signalResPass(mass, meanP[.0,-4.000,4.000],sigmaP[0.956,0.00,5.000])",      # 20-80
            #"RooGaussian::signalResPass(mass, meanP[.0,-4.000,2.000],sigmaP[0.956,0.00,4.000])",      # 80-110
            #"RooGaussian::signalResPass(mass, meanP[.0,-3.000,4.000],sigmaP[0.956,0.00,5.000])",       # >=110
            #"RooGaussian::signalResFail(mass, meanF[.0,-3.000,3.000],sigmaF[0.956,0.00,5.000])",      # 20-80 
            #"RooGaussian::signalResFail(mass, meanF[.0,-5.000,5.000],sigmaF[0.956,0.00,5.000])",      # 80-110
            #"RooGaussian::signalResFail(mass, meanF[.0,-4.000,3.000],sigmaF[0.956,0.00,5.000])",       # >=110
            
            # both EB and EE signal            
            "ZGeneratorLineShape::signalPhyPass(mass,\"MCtemplates.root\", \"hMass_20.000000To30.000000_0.000000To1.500000_Pass\")",
            "ZGeneratorLineShape::signalPhyFail(mass,\"MCtemplates.root\", \"hMass_20.000000To30.000000_0.000000To1.500000_Fail\")",

            "FCONV::signalPass(mass, signalPhyPass, signalResPass)",
            "FCONV::signalFail(mass, signalPhyFail, signalResFail)",


            # nominal background fit
            "RooExponential::backgroundPass(mass, aPass[-0.1, -1., 0.])",    # 20-40
            #"RooExponential::backgroundPass(mass, aPass[-0.1, -1., 0.1])",   # 40-60 
            #"RooExponential::backgroundPass(mass, aPass[-0.1, -1., 0.2])",    # >60
            "RooExponential::backgroundFail(mass, aFail[-0.1, -1., 0.1])",    # 20-110 EB, all EE
            #"RooExponential::backgroundFail(mass, aFail[-0.1, -1., 0.2])",    # >110, EB


            # to study the background fit systematics
            # EB 
            #"RooCMSShape::backgroundPass(mass, alphaPass[60.,50.,120.], betaPass[0.001, 0.,1.], gammaPass[0.01, 0.,.5], peakPass[90.0])",         # 20-40                       
            #"RooCMSShape::backgroundPass(mass, alphaPass[60.,50.,120.], betaPass[10., 5.,15.], gammaPass[0.001, 0.,0.08], peakPass[90.0])",       # 40-50          
            #"RooCMSShape::backgroundPass(mass, alphaPass[80.,60.,120.], betaPass[10., 5.,40.], gammaPass[0.001, -0.5,0.5], peakPass[90.0])",      # 50-60          
            #"RooCMSShape::backgroundPass(mass, alphaPass[80.,60.,120.], betaPass[50., 25.,70.], gammaPass[0.001, -0.5,5.], peakPass[90.0])",      # 60-80                    
            #"RooCMSShape::backgroundPass(mass, alphaPass[80.,60.,120.], betaPass[50., 25.,70.], gammaPass[0., -2.,1.], peakPass[90.0])",          # 80-110
            #"RooCMSShape::backgroundPass(mass, alphaPass[80.,60.,120.], betaPass[50., 25.,71.], gammaPass[0., -2.,1.2], peakPass[90.0])",         # 110-150
            #"RooCMSShape::backgroundPass(mass, alphaPass[80.,60.,120.], betaPass[0.001, 0.,0.1], gammaPass[0.001, -0.01,1.], peakPass[90.0])",    # 150-250
            #"RooCMSShape::backgroundPass(mass, alphaPass[80.,60.,120.], betaPass[0.001, 0.,0.2], gammaPass[0.001, -0.01,1.], peakPass[90.0])",    # 250-500 
            #"RooCMSShape::backgroundFail(mass, alphaFail[60.,50.,70.], betaFail[0.001, 0.,0.1], gammaFail[0.0001, -1.,1.0], peakFail[90.0])",     # 20-110
            #"RooCMSShape::backgroundFail(mass, alphaFail[60.,50.,72.], betaFail[0.001, 0.,0.1], gammaFail[0.0001, -1.,1.0], peakFail[90.0])",     # 110-250
            #"RooCMSShape::backgroundFail(mass, alphaFail[60.,50.,72.], betaFail[0.001, 0.,0.1], gammaFail[0.0001, 0.,1.0], peakFail[90.0])",      # 250-500

            # EE
            #"RooCMSShape::backgroundPass(mass, alphaPass[100.,50.,120.], betaPass[0.001, 0.,5.], gammaPass[0.001, 0.,1.0], peakPass[90.0])",        # 20-40                
            #"RooCMSShape::backgroundFail(mass, alphaFail[60.,50.,70.], betaFail[0.001, 0.,0.1], gammaFail[0.0001, -1.,1.0], peakFail[90.0])",       # 20-40
            #"RooCMSShape::backgroundPass(mass, alphaPass[100.,50.,130.], betaPass[0.01, 0.,10.], gammaPass[0.001, 0.,6.], peakPass[90.0])",         # 40-50
            #"RooCMSShape::backgroundFail(mass, alphaFail[60.,50.,80.], betaFail[0.1, 0.02,0.2], gammaFail[0.05, 0.,1.0], peakFail[90.0])",          # 40-50
            #"RooCMSShape::backgroundPass(mass, alphaPass[100.,50.,130.], betaPass[0.01, 0.,10.], gammaPass[0.001, 0.,5.], peakPass[90.0])",         # 50-60
            #"RooCMSShape::backgroundFail(mass, alphaFail[60.,50.,80.], betaFail[0.1, 0.02,0.2], gammaFail[0.05, 0.,1.0], peakFail[90.0])",          # 50-60
            #"RooCMSShape::backgroundPass(mass, alphaPass[100.,50.,130.], betaPass[0.01, 0.,10.], gammaPass[0.001, 0.,1.], peakPass[90.0])",        # 60-80
            #"RooCMSShape::backgroundFail(mass, alphaFail[90.,60.,120.], betaFail[0.001, 0.,0.1], gammaFail[0.0001, -1.,1.0], peakFail[90.0])",      # 60-80
            #"RooCMSShape::backgroundPass(mass, alphaPass[110.,90.,120.], betaPass[0.01, 0.,10.], gammaPass[0.001, 0.,1.], peakPass[90.0])",          # 80-110
            #"RooCMSShape::backgroundFail(mass, alphaFail[90.,60.,120.], betaFail[0.001, 0.,0.15], gammaFail[0.0001, -1.,1.0], peakFail[90.0])",      # 80-110
            #"RooCMSShape::backgroundPass(mass, alphaPass[110.,100.,120.], betaPass[0.001, 0.,5.], gammaPass[0.001, 0.,1.], peakPass[90.0])",           # >110
            #"RooCMSShape::backgroundFail(mass, alphaFail[90.,60.,120.], betaFail[0.001, 0.,0.1], gammaFail[0.0001, -1.,1.0], peakFail[90.0])",       # >110

            "efficiency[0.5,0,1]",
            "signalFractionInPassing[0.9]"
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
