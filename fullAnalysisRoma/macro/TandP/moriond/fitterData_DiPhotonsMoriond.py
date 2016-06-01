import FWCore.ParameterSet.Config as cms
process = cms.Process("TagProbe")
process.source = cms.Source("EmptySource")
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(1) )

process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.destinations = ['cout', 'cerr']
process.MessageLogger.cerr.FwkReport.reportEvery = 1000

################################################
InputFileName = "/afs/cern.ch/work/c/crovelli/public/TaP_fall15_Moriond16_v1/formattedZ/Formatted_singleEle2015Dv1.root"
OutputFilePrefix = "efficiency-data-"
PDFName = "pdfSignalPlusBackground"

################################################
EfficiencyBins = cms.PSet(probe_pt = cms.vdouble(20, 30 ),
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

            ## ------------ signal --------------------
            # EB, 20-30
            "RooCBExGaussShape::signalResPass(mass,meanP[0.0,-1.,1.],sigmaP[1.,0.01,5.0],alphaP[1.,0.01,5.0],nP[3.],sigmaP_2[1.000,0.1,15.00])", 
            "RooCBExGaussShape::signalResFail(mass,meanF[0.0,-1.,1.],sigmaF[5.,0.01,7.0],alphaF[1.,0.,5.0],nF[0.8],sigmaF_2[1.,0.001,15.000])",

            # EB, 30-40
            #"RooCBExGaussShape::signalResPass(mass,meanP[0.0,-1.,1.],sigmaP[1.,0.01,3.1],alphaP[1.,0.01,5.0],nP[6],sigmaP_2[1.000,0.1,15.00])", 
            #"RooCBExGaussShape::signalResFail(mass,meanF[0.0,-1.,1.],sigmaF[3.,0.01,4.0],alphaF[1.,0.,5.0],nF[2.5],sigmaF_2[1.,0.001,15.000])",
                              
            # EB, 40-50
            #"RooCBExGaussShape::signalResPass(mass,meanP[0.0,-1.5,1.5],sigmaP[1.,0.01,3.1],alphaP[1.,0.01,5.0],nP[50.],sigmaP_2[1.000,0.1,15.00])", 
            #"RooCBExGaussShape::signalResFail(mass,meanF[0.0,-1.,1.],sigmaF[3.,0.01,4.0],alphaF[1.,0.,5.0],nF[25.],sigmaF_2[1.,0.001,15.000])",
            
            # EB, 50-60
            #"RooCBExGaussShape::signalResPass(mass,meanP[0.0,-1.5,1.5],sigmaP[1.,0.01,3.1],alphaP[1.,0.01,10.0],nP[50.],sigmaP_2[1.000,0.1,15.00])", 
            #"RooCBExGaussShape::signalResFail(mass,meanF[1.2,-1.,1.5],sigmaF[1.,0.01,3.1],alphaF[1.,0.,5.0],nF[25.],sigmaF_2[1.,0.001,3.000])", 

            # EB, 60-80
            #"RooCBExGaussShape::signalResPass(mass,meanP[0.0,-1.5,1.5],sigmaP[1.,0.01,3.1],alphaP[1.,0.01,5.0],nP[50.],sigmaP_2[1.000,0.1,15.00])", 
            #"RooCBExGaussShape::signalResFail(mass,meanF[1.2,-1.1,1.5],sigmaF[1.,0.01,3.1],alphaF[1.,0.,5.0],nF[25.],sigmaF_2[1.,0.001,5.000])", 
            
            # EB, 80-110
            #"RooCBExGaussShape::signalResPass(mass,meanP[0.0,-1.5,1.5],sigmaP[1.,0.01,3.1],alphaP[1.,0.01,5.0],nP[50.],sigmaP_2[1.000,0.1,15.00])", 
            #"RooCBExGaussShape::signalResFail(mass,meanF[1.2,-1.5,1.5],sigmaF[1.,0.01,3.1],alphaF[1.,0.,5.0],nF[2.5],sigmaF_2[1.,0.001,5.000])", 
   
            # EB, 110-150
            #"RooCBExGaussShape::signalResPass(mass,meanP[1.0,-1.05,1.5],sigmaP[1.,0.01,3.1],alphaP[1.,0.01,5.0],nP[50.],sigmaP_2[1.000,0.1,15.00])", 
            #"RooCBExGaussShape::signalResFail(mass,meanF[0.,-1.5,1.],sigmaF[1.,0.01,3.1],alphaF[1.,0.,5.0],nF[3.6],sigmaF_2[1.,0.001,15.000])", 

            # EB, 150-200
            #"RooCBExGaussShape::signalResPass(mass,meanP[1.0,-0.5,1.5],sigmaP[1.,0.01,3.1],alphaP[1.,0.01,5.0],nP[6.],sigmaP_2[1.000,0.1,15.00])", 
            #"RooCBExGaussShape::signalResFail(mass,meanF[-0.5,-1.5,0.],sigmaF[1.,0.01,3.1],alphaF[1.,0.,5.0],nF[19.],sigmaF_2[1.,0.001,15.000])", 

            # EB, 200-270
            #"RooCBExGaussShape::signalResPass(mass,meanP[0.5,0.,1.5],sigmaP[1.,0.01,3.1],alphaP[1.,0.01,5.0],nP[50.],sigmaP_2[1.000,0.1,15.00])", 
            #"RooCBExGaussShape::signalResFail(mass,meanF[0.5,0.,1.],sigmaF[1.55,1.,3.1],alphaF[1.,0.,5.0],nF[25.],sigmaF_2[1.,0.001,15.000])", 
  
            # EB, 270-350
            #"RooCBExGaussShape::signalResPass(mass,meanP[-1.,-2.,0.],sigmaP[1.,0.01,2.],alphaP[1.,0.01,5.0],nP[10.],sigmaP_2[1.000,0.1,15.00])", 
            #"RooCBExGaussShape::signalResFail(mass,meanF[-1.,-2.,0.],sigmaF[1.,0.01,3.1],alphaF[1.,0.,5.0],nF[1.1],sigmaF_2[1.,0.001,15.000])", 

            # EB, 350-500
            #"RooCBExGaussShape::signalResPass(mass,meanP[-1.,-1.5,0.],sigmaP[1.,0.01,2.],alphaP[1.,0.01,5.0],nP[12.],sigmaP_2[1.000,0.1,15.00])", 
            #"RooCBExGaussShape::signalResFail(mass,meanF[0.,-3.,0.5],sigmaF[1.,0.01,2.1],alphaF[1.,0.,5.0],nF[0.12],sigmaF_2[1.,0.001,15.000])", 




            # EE, 20-30
            #"RooCBExGaussShape::signalResPass(mass,meanP[0.0,-5.,5.],sigmaP[1.,0.01,5.0],alphaP[1.,0.01,50.0],nP[3.9],sigmaP_2[1.000,0.1,15.00])",    
            #"RooCBExGaussShape::signalResFail(mass,meanF[0.0,-5.,5.],sigmaF[3.,0.01,5.0],alphaF[1.,0.,5.0],nF[3.9],sigmaF_2[1.,0.001,15.000])",       

            # EE, 30-40
            #"RooCBExGaussShape::signalResPass(mass,meanP[0.0,-5.,5.],sigmaP[1.,0.01,5.0],alphaP[1.,0.01,50.0],nP[7.5.],sigmaP_2[1.000,0.1,15.00])",       
            #"RooCBExGaussShape::signalResFail(mass,meanF[0.0,-5.,5.],sigmaF[3.,0.01,5.0],alphaF[1.,0.,5.0],nF[5.],sigmaF_2[1.,0.001,15.000])",          
            
            # EE, 40-50
            #"RooCBExGaussShape::signalResPass(mass,meanP[-0.5,-1.,0.],sigmaP[1.,0.01,5.0],alphaP[1.,0.01,50.0],nP[24.],sigmaP_2[1.000,0.1,15.00])",    
            #"RooCBExGaussShape::signalResFail(mass,meanF[-0.5,-1.,0.],sigmaF[3.,0.01,5.0],alphaF[1.,0.,5.0],nF[50.],sigmaF_2[1.,0.001,15.000])",        

            # EE, 50-60
            #"RooCBExGaussShape::signalResPass(mass,meanP[-1.,-2.,0.],sigmaP[1.,0.01,5.0],alphaP[1.,0.01,50.0],nP[12.],sigmaP_2[1.000,0.1,15.00])",  
            #"RooCBExGaussShape::signalResFail(mass,meanF[-0.5,-3.,0.],sigmaF[3.,0.01,5.0],alphaF[1.,0.,5.0],nF[50.],sigmaF_2[1.,0.001,15.000])",     

            # EE, 60-80
            #"RooCBExGaussShape::signalResPass(mass,meanP[-0.5,-1.5,0.0],sigmaP[0.5,0.001,4.0],alphaP[1.,0.01,50.0],nP[50.],sigmaP_2[1.000,0.1,5.00])",  
            #"RooCBExGaussShape::signalResFail(mass,meanF[-1.,-1.5,0.],sigmaF[3.,0.01,5.0],alphaF[1.,0.,5.0],nF[50.],sigmaF_2[1.,0.001,5.000])",  

            # EE, 80-110
            #"RooCBExGaussShape::signalResPass(mass,meanP[-1.,-3.,0.],sigmaP[1.,0.01,5.0],alphaP[1.,0.01,50.0],nP[0.2],sigmaP_2[1.000,0.1,15.00])",  
            #"RooCBExGaussShape::signalResFail(mass,meanF[-1.,-3.,0.],sigmaF[3.,0.01,5.0],alphaF[1.,0.,5.0],nF[4.4],sigmaF_2[1.,0.001,15.000])",  

            # EE, 110-150
            #"RooCBExGaussShape::signalResPass(mass,meanP[0.,-0.5,0.5],sigmaP[1.,0.01,5.0],alphaP[1.,0.01,50.0],nP[47.],sigmaP_2[1.000,0.1,15.00])",  
            #"RooCBExGaussShape::signalResFail(mass,meanF[0.,-0.5,0.5],sigmaF[3.,0.01,5.0],alphaF[1.,0.,5.0],nF[50.],sigmaF_2[1.,0.001,15.000])",  

            # EE, 150-200
            #"RooCBExGaussShape::signalResPass(mass,meanP[-0.1,-2.,2.],sigmaP[1.,0.01,5.0],alphaP[1.,0.01,50.0],nP[15.],sigmaP_2[1.000,0.1,15.00])",  
            #"RooCBExGaussShape::signalResFail(mass,meanF[-0.5,-2.,0.],sigmaF[3.,0.01,5.0],alphaF[1.,0.,5.0],nF[44.],sigmaF_2[1.,0.001,15.000])",  

            # EE, 200-500
            #"RooCBExGaussShape::signalResPass(mass,meanP[-0.1,-3.,3.],sigmaP[1.,0.01,5.0],alphaP[1.,0.01,50.0],nP[17.],sigmaP_2[1.000,0.1,15.00])",  
            #"RooCBExGaussShape::signalResFail(mass,meanF[0.,-0.5,0.5],sigmaF[3.,0.01,5.0],alphaF[1.,0.,5.0],nF[1.9],sigmaF_2[1.,0.001,15.000])",    

            "ZGeneratorLineShape::signalPhy(mass)", ### NLO line shape 
            "FCONV::signalPass(mass, signalPhy, signalResPass)", 
            "FCONV::signalFail(mass, signalPhy, signalResFail)", 
            

            ## ------------ background --------------------
            # EB
            "RooExponential::backgroundPass(mass, aPass[-0.1, -1., 0.])",     # 20-40
            #"RooExponential::backgroundPass(mass, aPass[-0.1, -1., 0.1])",    # 40-60
            #"RooExponential::backgroundPass(mass, aPass[-0.1, -1., 0.2])",    # >60 
            "RooExponential::backgroundFail(mass, aFail[-0.1, -1., 0.1])",    # 20-110 EB
            #"RooExponential::backgroundFail(mass, aFail[-0.1, -1., 0.2])",    # >110

            # EE
            #"RooExponential::backgroundPass(mass, aPass[-0.1, -1., 0.])",     # 20-40
            #"RooExponential::backgroundPass(mass, aPass[-0.1, -1., 0.1])",    # 40-50
            #"RooExponential::backgroundPass(mass, aPass[-0.1, -1., 0.2])",    # 50-60
            #"RooExponential::backgroundPass(mass, aPass[-0.1, -1., 0.2])",     # >60 
            #"RooExponential::backgroundFail(mass, aFail[-0.1, -1., 0.1])",     # all

            "efficiency[0.5,0,1]",  
            "signalFractionInPassing[0.9,0.,1]"     
            ),
                                                         ),

                                         Efficiencies = cms.PSet(
        MCtruth_Tight = cms.PSet(EfficiencyBinningSpecification,
                           EfficiencyCategoryAndState = cms.vstring("probe_fullsel", "pass"),
                           ),
        )
                                         )

process.fit = cms.Path(
    process.GsfElectronToId  
    )
