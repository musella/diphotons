import FWCore.ParameterSet.Config as cms

process = cms.Process("TagProbe")
process.source = cms.Source("EmptySource")
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(1) )

process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.destinations = ['cout', 'cerr']
process.MessageLogger.cerr.FwkReport.reportEvery = 1000

###############################################
InputFileName = "/afs/cern.ch/work/c/crovelli/public/TaP_spring15_7415v2/topup/formattedZfake/Formatted_singleEle2015D_all__forFakeRate.root"
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
                                         OutputFileName = cms.string(OutputFilePrefix+"EleVeto.root"),
                                         NumCPU = cms.uint32(1),
                                         SaveWorkspace = cms.bool(False),
                                         doCutAndCount = cms.bool(False),
                                         floatShapeParameters = cms.bool(True),
                                         binnedFit = cms.bool(True),
                                         binsForFit = cms.uint32(40),            # 20-200 EB
                                         #binsForFit = cms.uint32(30),            # 200-350 EB
                                         #binsForFit = cms.uint32(20),            # >=350 EB
                                         #binsForFit = cms.uint32(40),            # 20-180 EE
                                         #binsForFit = cms.uint32(30),            # >=180 EE
                                         # defines all the real variables of the probes available in the input tree and intended for use in the efficiencies
                                         Variables = cms.PSet(mass = cms.vstring("Tag-Probe Mass", "70.0", "110.0", "GeV/c^{2}"),
                                                              probe_pt = cms.vstring("Probe E_{T}", "0", "500", "GeV/c"),
                                                              probe_absEta = cms.vstring("Probe #eta", "0", "2.5", ""),
                                                              ),

                                         # defines all the discrete variables of the probes available in the input tree and intended for use in the efficiency calculations
                                         Categories = cms.PSet(
        probe_eleveto = cms.vstring("probe_eleveto", "dummy[pass=1,fail=0]"),
                                                               ),

                                         # defines all the PDFs that will be available for the efficiency calculations; 
                                         PDFs = cms.PSet(pdfSignalPlusBackground = cms.vstring(


            # barrel signal
            "RooGaussian::signalResPass(mass, meanP[0.,-2.000,2.000],sigmaP[0.956,0.50,5.000])",        #20-40 
            #"RooGaussian::signalResPass(mass, meanP[0.5,-1.000,3.000],sigmaP[2.,0.10,5.000])",          #40-50
            #"RooGaussian::signalResPass(mass, meanP[0.5,-1.000,5.000],sigmaP[2.,0.50,5.000])",          #50-60
            #"RooGaussian::signalResPass(mass, meanP[1.,0.000,5.000],sigmaP[1.,0.1,5.000])",             #60-80
            #"RooGaussian::signalResPass(mass, meanP[0.0,-1.000,3.000],sigmaP[0.956,0.00,5.000])",       #80-150
            #"RooGaussian::signalResPass(mass, meanP[1.5,-1.000,5.000],sigmaP[0.956,0.50,5.00])",        #150-200
            #"RooGaussian::signalResPass(mass, meanP[0.6,-2.,2.],sigmaP[2.,1.00,5.000])",                #200-350
            #"RooGaussian::signalResPass(mass, meanP[2.1,1.,3.5],sigmaP[1.,0.5,3.50])",                  #>350
            "RooGaussian::signalResFail(mass, meanF[0.0,-1.000,5.000],sigmaF[0.956,0.00,5.000])",       #20-80 
            #"RooGaussian::signalResFail(mass, meanF[-1.0,-3.000,2.000],sigmaF[0.956,0.00,5.000])",      #80-110 
            #"RooGaussian::signalResFail(mass, meanF[0.0,-1.000,5.000],sigmaF[0.956,0.00,2.000])",       #110-150
            #"RooGaussian::signalResFail(mass, meanF[0.0,-3.000,2.000],sigmaF[2.,0.50,4.000])",          #150-200
            #"RooGaussian::signalResFail(mass, meanF[-0.5,-1.,1.],sigmaF[0.5.,0.4,2.000])",              #200-270
            #"RooGaussian::signalResFail(mass, meanF[-0.5,-1.000,1.0],sigmaF[0.956,0.00,5.000])",        #270-350
            #"RooGaussian::signalResFail(mass, meanF[1.,0.5,1.5],sigmaF[0.4,0.,0.5])",                   #>350

            # endcap signal
            #"RooGaussian::signalResPass(mass, meanP[.0,-4.000,2.000],sigmaP[0.956,0.00,5.000])",       # 20-50 
            #"RooGaussian::signalResPass(mass, meanP[.0,-4.000,2.000],sigmaP[1.5,1.00,5.000])",         # 50-80
            #"RooGaussian::signalResPass(mass, meanP[0.1,-1.000,1.000],sigmaP[0.956,0.00,3.000])",      # 80-105 
            #"RooGaussian::signalResPass(mass, meanP[-0.2,-2.00,0.00],sigmaP[0.956,0.00,5.000])",       # 105-140
            #"RooGaussian::signalResPass(mass, meanP[-1.,-2.000,0.],sigmaP[3.,1.00,5.000])",            # 140-180
            #"RooGaussian::signalResPass(mass, meanP[-1.5,-2.,0.],sigmaP[0.1,0.,1.5])",                 # >=180
            #"RooGaussian::signalResFail(mass, meanF[.0,-3.000,3.000],sigmaF[0.956,0.00,5.000])",       # 20-80 
            #"RooGaussian::signalResFail(mass, meanF[0.1,-1.000,1.000],sigmaF[2,0.00,5.000])",          # 80-105
            #"RooGaussian::signalResFail(mass, meanF[.0,-4.000,3.000],sigmaF[0.956,0.00,5.000])",       # 105-140
            #"RooGaussian::signalResFail(mass, meanF[0,-2.000,2.000],sigmaF[0.956,0.00,5.000])",        # 140-180
            #"RooGaussian::signalResFail(mass, meanF[0.7,0.3,1.5],sigmaF[0.5,0.,1.])",                  # 180-250   
            #"RooGaussian::signalResFail(mass, meanF[0.1,0.,1.5],sigmaF[0.5,0.,1.])",                   # >=250
            
            # both EB and EE signal            
            "ZGeneratorLineShape::signalPhyPass(mass,\"MCtemplatesFakes.root\", \"hMass_20.000000To30.000000_0.000000To1.500000_Pass\")",
            "ZGeneratorLineShape::signalPhyFail(mass,\"MCtemplatesFakes.root\", \"hMass_20.000000To30.000000_0.000000To1.500000_Fail\")",

            "FCONV::signalPass(mass, signalPhyPass, signalResPass)",
            "FCONV::signalFail(mass, signalPhyFail, signalResFail)",


            # nominal background fit
            "RooExponential::backgroundPass(mass, aPass[-0.1, -1., 0.])",      # 20-40
            #"RooExponential::backgroundPass(mass, aPass[-0.1, -1., 0.1])",    # 40-60 
            #"RooExponential::backgroundPass(mass, aPass[-0.1, -1., 0.2])",    # >60
            "RooExponential::backgroundFail(mass, aFail[-0.1, -1., 0.1])",     # 20-110 EB, all EE
            #"RooExponential::backgroundFail(mass, aFail[-0.1, -1., 0.2])",    # >110, EB

            "efficiency[0.5,0,1]",
            "signalFractionInPassing[0.9,0.,1]"
            ),
                                                         ),

                                         # defines a set of efficiency calculations, what PDF to use for fitting and how to bin the data;
                                         # there will be a separate output directory for each calculation that includes a simultaneous fit, side band subtraction and counting. 
                                         Efficiencies = cms.PSet(
        MCtruth_Tight = cms.PSet(EfficiencyBinningSpecification,
                                 EfficiencyCategoryAndState = cms.vstring("probe_eleveto", "pass"),
                                                                                   ),
                                                                 )
                                         )

process.fit = cms.Path(
    process.GsfElectronToId  
    )
