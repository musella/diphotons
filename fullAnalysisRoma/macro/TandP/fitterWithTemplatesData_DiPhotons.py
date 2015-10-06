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
                                         binsForFit = cms.uint32(40),
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


            # barrel
            "RooGaussian::signalResPass(mass, meanP[.0,-5.000,5.000],sigmaP[0.956,0.00,5.000])",    #20-110
            "RooGaussian::signalResFail(mass, meanF[.0,-5.000,5.000],sigmaF[0.956,0.00,5.000])",    #20-110 
            #"RooGaussian::signalResPass(mass, meanP[.0,-5.000,2.000],sigmaP[0.956,0.00,5.000])",    #>=110
            #"RooGaussian::signalResFail(mass, meanF[.0,-5.000,5.000],sigmaF[0.956,0.00,5.000])",    #>=110

            # endcap
            #"RooGaussian::signalResPass(mass, meanP[.0,-5.000,5.000],sigmaP[0.956,0.00,5.000])",    # 20-80
            #"RooGaussian::signalResPass(mass, meanP[.0,-2.000,2.000],sigmaP[0.956,0.00,5.000])",    # 80-110
            #"RooGaussian::signalResPass(mass, meanP[.0,-2.000,5.000],sigmaP[0.956,0.00,5.000])",    # >=110
            #"RooGaussian::signalResFail(mass, meanF[.0,-5.000,5.000],sigmaF[0.956,0.00,5.000])",    # all
            
            # both EB and EE            
            "ZGeneratorLineShape::signalPhyPass(mass,\"MCtemplates.root\", \"hMass_20.000000To30.000000_0.000000To1.500000_Pass\")",
            "ZGeneratorLineShape::signalPhyFail(mass,\"MCtemplates.root\", \"hMass_20.000000To30.000000_0.000000To1.500000_Fail\")",

            "FCONV::signalPass(mass, signalPhyPass, signalResPass)",
            "FCONV::signalFail(mass, signalPhyFail, signalResFail)",

            #"RooExponential::backgroundPass(mass, aPass[-0.1, -1., 0.2])",  # >60
            #"RooExponential::backgroundPass(mass, aPass[-0.1, -1., 0.1])",  # 40-60 
            "RooExponential::backgroundPass(mass, aPass[-0.1, -1., 0.])",    # 20-40
            "RooExponential::backgroundFail(mass, aFail[-0.1, -1., 0.1])",   # 20-110
            #"RooExponential::backgroundFail(mass, aFail[-0.1, -1., 0.2])",  # >110

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
