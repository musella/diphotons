#include "RooRealVar.h"
#include "RooAbsPdf.h"
#include "RooBinning.h"
#include "RooDataSet.h"
#include "RooDataHist.h"
#include "RooWorkspace.h"
#include "RooPlot.h"
#include "RooGaussian.h"
#include "RooHistPdf.h"
#include "RooMomentMorph.h"
#include "RooNumConvPdf.h"   
#include "RooFFTConvPdf.h"
#include "RooFitResult.h"     
#include "RooCustomizer.h"     
#include "TFile.h"
#include "TH1D.h"
#include "TTree.h"
#include "TChain.h"
#include "TMath.h"
#include "TROOT.h"
#include "TVectorD.h"
#include <iostream>
#include "TCanvas.h"

using namespace RooFit;
using namespace std;

// to be modified:
static const Int_t NCAT = 2;  
static const Int_t checkMass = 0;        // 0 not to check; otherwise mass to be checked
static const Int_t genOnly = 1;
static const Int_t inZero = 0;

// Definition of the variables in the input ntuple
RooArgSet* defineVariables() {

  RooRealVar* mgg        = new RooRealVar("mgg",        "M(gg)",      300, 5050, "GeV");   
  RooRealVar* mggGen     = new RooRealVar("mggGen",     "M(gg) gen",  300, 5050, "GeV");   
  RooRealVar* eventClass = new RooRealVar("eventClass", "eventClass", -10,   10, "");
  RooRealVar* weight     = new RooRealVar("weight",     "weightings",   0, 1000, "");   

  RooArgSet* ntplVars = new RooArgSet(*mgg, *mggGen, *eventClass, *weight);                  
  
  return ntplVars;
}

// Load the roodatahists and make a convolution 
void ConvolutionFromRDH(RooWorkspace* w, Int_t mass, TString coupling) {     

  TString myMass = TString::Format("%d",mass);

  // Dataset - only for closure test   
  RooDataSet* sigToFit[NCAT];   
  
  // RooRealVars
  RooRealVar* mgg = w->var("mgg");  
  RooRealVar* zeroVar = w->var("zeroVar");
  TString deltaMname = TString::Format("deltaM_mass%d",mass);
  RooRealVar* deltaM = w->var(deltaMname);    
  deltaM->SetTitle("deltaM");     
  deltaM->SetName("deltaM");
  TString deltaMgenName = TString::Format("deltaMgen_mass%d",mass);
  RooRealVar* deltaMgen = w->var(deltaMgenName);    
  deltaMgen->SetTitle("deltaMgen");  
  deltaMgen->SetName("deltaMgen");   
  TString mHname = TString::Format("mH_mass%d",mass);
  RooRealVar* mH = w->var(mHname);
  mH->setConstant();   

  // Resolution centred in zero
  RooFormulaVar *deltaM_formula = new RooFormulaVar("deltaM_formula","","@0",RooArgList(*w->var("mgg")));
  if (inZero) deltaM_formula = new RooFormulaVar("deltaM_formula","","@0",RooArgList(*w->var("zeroVar")));

  // Intrinsic width centred in mH or in zero
  RooFormulaVar *deltaMgen_formula = new RooFormulaVar("deltaMgen_formula","","@0-@1",RooArgList(*w->var("mgg"),*mH)); 
  if (inZero) deltaMgen_formula = new RooFormulaVar("deltaMgen_formula","","@0",RooArgList(*w->var("zeroVar"))); 

  // To move from deltaM to mgg
  RooArgList histObsRes;
  histObsRes.add(*deltaM);

  // To move from deltaMgen to mgg
  RooArgList histObsInw;
  histObsInw.add(*deltaMgen);

  // Files with resolution and intrinsic width RDHs
  TFile *fileRes;
  if (!genOnly) fileRes = new TFile("ResolutionHistos.root");       
  if (genOnly)  fileRes = new TFile("ResHistosGenOnlyScan.root");
  TFile *fileInw;
  if (!genOnly) fileInw = new TFile("IntrinsicWidthHistos.root"); 
  if (genOnly)  fileInw = new TFile("WidthHistosGenOnlyScan.root");

  // Plots to check
  TCanvas* c1 = new TCanvas("c1","PhotonsMass",0,0,800,800);  
  c1->cd(1);    

  for (int c=0; c<NCAT; ++c) {  

    cout << "---------- Category = " << c << endl;    
    TString myCutW = "EBEB";         // category for the width
    if (NCAT==2) {
      if (c==0) myCutW = "EBEB";  
      if (c==1) myCutW = "EBEE";  
    } else if (NCAT==3) {
      if (c==0) myCutW = "EBEB";  
      if (c==1) myCutW = "EBEB";  
      if (c==2) myCutW = "EBEE";  
    } else if (NCAT==4) {
      if (c==0) myCutW = "EBEB";  
      if (c==1) myCutW = "EBEB";  
      if (c==2) myCutW = "EBEE";  
      if (c==3) myCutW = "EBEE";  
    }
    TString myCutR = "EBEB";        // category for the resolution
    if (NCAT==2) {  
      if (c==0) myCutR = "EBEB";  
      if (c==1) myCutR = "EBEE";  
    } else if (NCAT==3) { 
      if (c==0) myCutR = "EBHighR9";  
      if (c==1) myCutR = "EBLowR9";  
      if (c==2) myCutR = "EBEE";  
    } else if (NCAT==4) { 
      if (c==0) myCutR = "EBHighR9";  
      if (c==1) myCutR = "EBLowR9";  
      if (c==2) myCutR = "EEHighR9";  
      if (c==3) myCutR = "EELowR9";  
    }

    // for resolution
    TString myDeltaM_formulaA = TString(Form("deltaM_formula_cat"+myCutR+"_mass%d",mass));
    TString myDeltaM_formula  = TString(Form(myDeltaM_formulaA+"_kpl"+coupling));   
    deltaM_formula->SetTitle(myDeltaM_formula);
    deltaM_formula->SetName(myDeltaM_formula);
    RooArgList pdfObsRes;
    pdfObsRes.add(*deltaM_formula);  
    
    // for intrinsic width
    TString myDeltaMgen_formulaA = TString(Form("deltaMgen_formula_cat"+myCutW+"_mass%d",mass));
    TString myDeltaMgen_formula  = TString(Form(myDeltaMgen_formulaA+"_kpl"+coupling));   
    deltaMgen_formula->SetTitle(myDeltaMgen_formula);
    deltaMgen_formula->SetName(myDeltaMgen_formula);
    RooArgList pdfObsInw;
    pdfObsInw.add(*deltaMgen_formula);

    // reading the roodatahists 
    TString myRDHA = TString(Form("resolRDH_mass%d_cat",mass)+myCutR);     
    RooDataHist *resRDH = (RooDataHist*)fileRes->Get(myRDHA);    
    resRDH->Print(); 
    //TString myRDHBa = TString(Form("intWidthRDH_mass%d_cat",mass)+myCutW);   
    TString myRDHBa = TString(Form("widthRDH_mass%d_cat",mass)+myCutW);   
    TString myRDHB = TString(Form(myRDHBa))+TString(Form("_kpl"))+TString(Form(coupling));
    RooDataHist *inwRDH = (RooDataHist*)fileInw->Get(myRDHB);   
    cout << myRDHB << endl;
    inwRDH->Print();        
    cout << "RooDataHists taken" << endl;       

    // creating the roohistpdfs    
    TString myHistPdfResNameA = TString(Form("myHistPdfRes_cat"+myCutR+"_mass%d",mass));
    TString myHistPdfResName  = TString(Form(myHistPdfResNameA+"_kpl"+coupling));   
    RooHistPdf *myHistPdfRes = new RooHistPdf("myHistPdfRes","myHistPdfRes",pdfObsRes, histObsRes,*resRDH,0) ;  
    myHistPdfRes->SetTitle(myHistPdfResName);
    myHistPdfRes->SetName(myHistPdfResName);
    myHistPdfRes->Print(); 
    //
    TString myHistPdfInwNameA = TString(Form("myHistPdfInw_cat"+myCutW+"_mass%d",mass));
    TString myHistPdfInwName  = TString(Form(myHistPdfInwNameA+"_kpl"+coupling));   
    RooHistPdf *myHistPdfInw = new RooHistPdf("myHistPdfInw","myHistPdfInw",pdfObsInw, histObsInw,*inwRDH,0) ;  
    myHistPdfInw->SetTitle(myHistPdfInwName);
    myHistPdfInw->SetName(myHistPdfInwName);
    myHistPdfInw->Print(); 

    cout << "RooHistPdfs done" << endl;      

    // Now make the convolution    

    // numerical
    /*
    mgg->setRange(-500,500);
    mgg->setBins(1000);
    RooNumConvPdf convol("convol","convol",*mgg,*myHistPdfInw,*myHistPdfRes);          
    RooRealVar* W = new RooRealVar("W","W",200);    
    W->setConstant();    
    RooRealVar* C = new RooRealVar("C","C",0);  
    C->setConstant();      
    convol.setConvolutionWindow(*C,*W);  
    */

    // analytical
    if (!inZero) mgg->setBins(10000, "cache");
    if (inZero)  zeroVar->setBins(10000, "cache");
    TString myConvNameA = TString(Form("Convolution_cat"+myCutR+"_mass%d",mass));
    TString myConvName  = TString(Form(myConvNameA+"_kpl"+coupling));   
    RooFFTConvPdf *convol = new RooFFTConvPdf("convol","convol",*mgg,*myHistPdfInw,*myHistPdfRes);          
    if (inZero) convol = new RooFFTConvPdf("convol","convol",*zeroVar,*myHistPdfInw,*myHistPdfRes);          
    // ad hoc corrections - start
    if (coupling=="001" && mass==518)  convol->setBufferFraction(0.04);    
    if (coupling=="001" && mass==524)  convol->setBufferFraction(0.04);    
    if (coupling=="001" && mass==568)  convol->setBufferFraction(0.04);    
    if (coupling=="001" && mass==584)  convol->setBufferFraction(0.04);    
    if (coupling=="001" && mass==520)  convol->setBufferFraction(0.09);    
    if (coupling=="001" && mass==536)  convol->setBufferFraction(0.09);    
    if (coupling=="001" && mass==572)  convol->setBufferFraction(0.09);    
    if (coupling=="001" && mass==578)  convol->setBufferFraction(0.09);    
    if (coupling=="001" && mass==586)  convol->setBufferFraction(0.09);    
    if (coupling=="001" && mass==624)  convol->setBufferFraction(0.09);    
    if (coupling=="001" && mass==750)  convol->setBufferFraction(0.08);    
    if (coupling=="001" && mass==786)  convol->setBufferFraction(0.09);    
    if (coupling=="001" && mass==3200) convol->setBufferFraction(0.09);    
    /*
    if (coupling=="001" && mass==528 && c==1)  convol->setBufferFraction(0.11);    
    if (coupling=="001" && mass==640 && c==1)  convol->setBufferFraction(0.11);
    if (coupling=="001" && mass==678 && c==1)  convol->setBufferFraction(0.11);
    if (coupling=="001" && mass==732 && c==1)  convol->setBufferFraction(0.11);
    if (coupling=="001" && mass==852 && c==1)  convol->setBufferFraction(0.11);
    if (coupling=="001" && mass==896 && c==1)  convol->setBufferFraction(0.11);
    if (coupling=="001" && mass==1054 && c==1) convol->setBufferFraction(0.11);
    if (coupling=="001" && mass==1112 && c==1) convol->setBufferFraction(0.11);
    if (coupling=="001" && mass==1000 && c==1) convol->setBufferFraction(0.11);
    if (coupling=="001" && mass==1050 && c==1) convol->setBufferFraction(0.11);
    if (coupling=="001" && mass==3200 && c==1) convol->setBufferFraction(0.12);
    if (coupling=="001" && mass==4650 && c==1) convol->setBufferFraction(0.11);
    if (coupling=="001" && mass==1540 && c==1) convol->setBufferFraction(0.11);
    if (coupling=="001" && mass==1580 && c==1) convol->setBufferFraction(0.11);
    if (coupling=="001" && mass==1592 && c==1) convol->setBufferFraction(0.11);
    */
    // ad hoc corrections - end
    convol->SetTitle(myConvName);
    convol->SetName(myConvName);

    // cout << "cambio nome start" << endl;
    // RooFormulaVar *shiftFormula = new RooFormulaVar("shiftFormula","","@0-@1",RooArgList(*w->var("mgg"),*mH)); 
    // RooCustomizer* cust = new RooCustomizer(convol, "shiftedMass");
    // cust->replaceArg(*zeroVar,*shiftFormula);
    // RooAbsPdf *newconvol = (RooAbsPdf*) cust->build();
    // newconvol->Print();
    // cout << "cambio nome end" << endl;
    cout << "Convolution done" << endl;   

    // Both
    convol->Print();     
    cout << "Convolution done and printed. now plot" << endl;   

    // Fit and Plot - only for full sim samples
    if (!genOnly) {
      int fitMin = mass-250;        
      int fitMax = mass+250;        
      if (mass>=2000) {
	fitMin = mass-750;        
	fitMax = mass+750;        
	if (inZero) {
	  fitMin = -300;
	  fitMax = -300;
	}      
      } 

      TString myUnbDS = TString(Form("SigWeight_cat"+myCutR+"_mass%d",mass));   
      sigToFit[c] = (RooDataSet*) w->data(myUnbDS); 
      sigToFit[c]->Print();      
      
      RooPlot* myPlot = mgg->frame(Range(fitMin,fitMax),Bins(50)); 
      if (!inZero && coupling=="001") myPlot = mgg->frame(Range(fitMin,fitMax),Bins(40));
      if (inZero) myPlot = zeroVar->frame(Range(fitMin,fitMax),Bins(50)); 
      myPlot->SetTitle("Convolution, cat"+myCutR);    
      if (!inZero) sigToFit[c]->plotOn(myPlot, LineColor(kRed), LineStyle(kDashed));         
      convol->plotOn(myPlot, LineColor(kBlue));  
      myHistPdfRes->plotOn(myPlot, LineColor(kRed));
      myHistPdfInw->plotOn(myPlot, LineColor(kYellow));
      double max = myPlot->GetMaximum();   
      myPlot->GetYaxis()->SetRangeUser(0.0001, max*1.2);      
      myPlot->Draw();           
      TString canvasName = TString(Form("closure_cat"+myCutR+"_mass"+myMass+"_kpl"+coupling+".png"));
      c1->SetLogy(0);
      c1->SaveAs(canvasName); 
      c1->SetLogy(1);
      canvasName = TString(Form("closure_cat"+myCutR+"_mass"+myMass+"_kpl"+coupling+"_log.png"));
      c1->SaveAs(canvasName); 
    }

    // Importing the convolution in the workspace
    w->import(*convol);   

    // moving to the corresponding roohistpdf
    if (!inZero) mgg->setBins(10000);
    if (inZero)  zeroVar->setBins(10000);
    RooDataHist *convRDH;
    if (!inZero) convRDH = convol->generateBinned(*mgg,10000,kTRUE);
    if (inZero)  convRDH = convol->generateBinned(*zeroVar,10000,kTRUE);
    TString myConvRdhNameA = TString(Form("ConvolutionRDH_cat"+myCutR+"_mass%d",mass));
    TString myConvRdhName  = TString(Form(myConvRdhNameA+"_kpl"+coupling));   
    convRDH->SetTitle(myConvRdhName);
    convRDH->SetName(myConvRdhName);
    cout << "done with RooDataHist from conv" << endl;
    convRDH->Print();
    cout << endl;
    
    RooHistPdf *convRhPdf;
    if (!inZero) convRhPdf = new RooHistPdf("convRhPdf","convRHhPdf",*mgg,*convRDH,0);
    if (inZero)  convRhPdf = new RooHistPdf("convRhPdf","convRHhPdf",*zeroVar,*convRDH,0);
    TString myConvRhPdfNameA = TString(Form("ConvolutionRhPdf_cat"+myCutR+"_mass%d",mass));
    TString myConvRhPdfName  = TString(Form(myConvRhPdfNameA+"_kpl"+coupling));              // chiara
    //TString myConvRhPdfName  = TString(Form(myConvRhPdfNameA+"_kpl_smearDown"+coupling));        // chiara
    convRhPdf->SetTitle(myConvRhPdfName);   
    convRhPdf->SetName(myConvRhPdfName);    
    cout << "done with RooHistPdf from conv"<< endl;
    convRhPdf->Print();
    w->import(*convRhPdf);
    cout << endl;
    
    delete convol;
    delete convRhPdf;
    delete myHistPdfRes;    
    delete myHistPdfInw;     
  }

  // deleting
  delete c1;
  delete fileInw;   
  delete fileRes;      
  delete deltaM_formula;
  delete deltaMgen_formula;

  // Saving the WS
  cout<< endl; 
  TString filename("/tmp/crovelli/myWSwithMorphing.root"); 
  TFile fileWs(filename,"RECREATE");
  fileWs.cd(); 
  w->writeToFile(filename);       
  cout << "Write signal workspace in: " << filename << " file" << endl;  
  cout << endl;  

  // checking the workspaces
  w->Print();
}

//-------------------------------------------------------

// Loading signal data and making roodatasets
void AddSigData(RooWorkspace* w, int mass, TString coupling) {
  
  TString myMass = TString::Format("%d",mass);

  Int_t ncat = NCAT;
  
  // Variables
  RooArgSet* ntplVars = defineVariables();

  // Files
  TString inDir = "../macro/allFilesWithResolAtZ_rereco76x_2classes_0T_v6/";          // chiara
  TChain* sigTree = new TChain();
  cout << "reading file " 
       << inDir+TString(Form("FormSigMod_kpl"))+coupling+TString(Form("_M%d.root/DiPhotonTree", mass)) << endl;
  sigTree->Add(inDir+TString(Form("FormSigMod_kpl"))+coupling+TString(Form("_M%d.root/DiPhotonTree", mass)));
  sigTree->SetTitle("sigTree");
  sigTree->SetName("sigTree");
  
  // Minimal common preselection cut on mgg and mggGen
  TString mainCut = TString::Format("mgg>=300 && mgg<=5050 && mggGen>=300 && mggGen<=5050");  
  RooDataSet sigWeighted("sigWeighted","dataset",sigTree,*ntplVars,mainCut,"weight");   
  sigWeighted.Print();

  // defining the reco - gen mass difference
  RooFormulaVar *deltaM_formula = new RooFormulaVar("deltaM_formula","","@0-@1",RooArgList(*w->var("mgg"),*w->var("mggGen"))); 
  RooRealVar* deltaM = (RooRealVar*) sigWeighted.addColumn(*deltaM_formula);       
  TString deltaMname = TString::Format("deltaM_mass%d",mass);
  deltaM->SetName(deltaMname);
  deltaM->SetTitle(deltaMname);
  w->import(*deltaM);

  // nominal graviton mass
  RooRealVar* mH = new RooRealVar("mH", "M(Graviton)", 0, 10000, "GeV");    
  TString mHname = TString::Format("mH_mass%d",mass);
  mH->SetName(mHname);
  mH->SetTitle(mHname);
  mH->setVal(mass);
  mH->setConstant();
  w->import(*mH);

  // defining the gen - mH difference 
  RooFormulaVar *deltaMgen_formula = new RooFormulaVar("deltaMgen_formula","","@0-@1",RooArgList(*w->var("mggGen"),*w->var(mHname))); 
  RooRealVar* deltaMgen = (RooRealVar*) sigWeighted.addColumn(*deltaMgen_formula);       
  TString deltaMgenName = TString::Format("deltaMgen_mass%d",mass);
  deltaMgen->SetName(deltaMgenName);
  deltaMgen->SetTitle(deltaMgenName);
  w->import(*deltaMgen);

  // Split in categories - mgg as observable
  cout << endl;
  cout << "preparing dataset with observable mgg" << endl;
  RooDataSet* signal[NCAT];
  for (int c=0; c<ncat; ++c) {
    if (c==0) signal[c] = (RooDataSet*) sigWeighted.reduce(*w->var("mgg"),mainCut+TString::Format("&& eventClass==0"));
    if (c==1) signal[c] = (RooDataSet*) sigWeighted.reduce(*w->var("mgg"),mainCut+TString::Format("&& eventClass==1"));
    if (NCAT==4) {
      if (c==2) signal[c] = (RooDataSet*) sigWeighted.reduce(*w->var("mgg"),mainCut+TString::Format("&& eventClass==2"));
      if (c==3) signal[c] = (RooDataSet*) sigWeighted.reduce(*w->var("mgg"),mainCut+TString::Format("&& eventClass==3"));
    }

    TString myCut;
    if (NCAT==2) {
      if (c==0) myCut = "EBEB";  
      if (c==1) myCut = "EBEE";
    } else if (NCAT==3) {
      if (c==0) myCut = "EBHighR9";  
      if (c==1) myCut = "EBLowR9";
      if (c==2) myCut = "EBEE";  
    } else if (NCAT==4) {
      if (c==0) myCut = "EBHighR9";  
      if (c==1) myCut = "EBLowR9";
      if (c==2) myCut = "EEHighR9";  
      if (c==3) myCut = "EELowR9";
    }
    w->import(*signal[c],Rename("SigWeight_cat"+myCut+"_mass"+myMass));
    
    cout << "cat " << c << ", signal[c]: " << endl;
    signal[c]->Print("v");
    cout << "---- for category " << c << ", nX for signal[c]:  " << signal[c]->sumEntries() << endl; 
    cout << endl;
  }

  cout << endl;
  cout << endl;
  cout << "---------------------------------------------" << endl;
  cout << endl;
  cout << endl;

  // Split in categories - deltaM as observable
  cout << endl;
  cout << "preparing dataset with observable deltaM" << endl;
  RooDataSet* signalDm[NCAT];
  for (int c=0; c<ncat; ++c) {
    TString deltaMname = TString::Format("deltaM_mass%d",mass);
    if (c==0) signalDm[c] = (RooDataSet*) sigWeighted.reduce(*w->var(deltaMname),mainCut+TString::Format("&& eventClass==0"));
    if (c==1) signalDm[c] = (RooDataSet*) sigWeighted.reduce(*w->var(deltaMname),mainCut+TString::Format("&& eventClass==1"));
    if (NCAT==3) {
      if (c==2) signalDm[c] = (RooDataSet*) sigWeighted.reduce(*w->var(deltaMname),mainCut+TString::Format("&& eventClass==2"));
    }
    if (NCAT==4) {
      if (c==2) signalDm[c] = (RooDataSet*) sigWeighted.reduce(*w->var(deltaMname),mainCut+TString::Format("&& eventClass==2"));
      if (c==3) signalDm[c] = (RooDataSet*) sigWeighted.reduce(*w->var(deltaMname),mainCut+TString::Format("&& eventClass==3"));
    }

    TString myCut;
    if (NCAT==2) {
      if (c==0) myCut = "EBEB";  
      if (c==1) myCut = "EBEE";
    } else if (NCAT==3) {
      if (c==0) myCut = "EBHighR9";  
      if (c==1) myCut = "EBLowR9";
      if (c==2) myCut = "EBEE";  
    } else if (NCAT==4) {
      if (c==0) myCut = "EBHighR9";  
      if (c==1) myCut = "EBLowR9";
      if (c==2) myCut = "EEHighR9";  
      if (c==3) myCut = "EELowR9";
    }
    w->import(*signalDm[c],Rename("SigWeightDeltaM_cat"+myCut+"_mass"+myMass));
    
    cout << "cat " << c << ", signal[c]: " << endl;
    signalDm[c]->Print("v");
    cout << "---- for category " << c << ", nX for signal[c]:  " << signalDm[c]->sumEntries() << endl; 
    cout << endl;
  }

  cout << endl;
  cout << endl;
  cout << "---------------------------------------------" << endl;
  cout << endl;
  cout << endl;

  // Split in categories - deltaMgen as observable
  cout << endl;
  cout << "preparing dataset with observable deltaMgen" << endl;
  RooDataSet* signalDmgen[NCAT];
  for (int c=0; c<ncat; ++c) {
    TString deltaMgenName = TString::Format("deltaMgen_mass%d",mass);
    if (c==0) signalDmgen[c] = (RooDataSet*) sigWeighted.reduce(*w->var(deltaMgenName),mainCut+TString::Format("&& eventClass==0"));
    if (c==1) signalDmgen[c] = (RooDataSet*) sigWeighted.reduce(*w->var(deltaMgenName),mainCut+TString::Format("&& eventClass==1"));
    if (NCAT==3) {
      if (c==2) signalDmgen[c] = (RooDataSet*) sigWeighted.reduce(*w->var(deltaMgenName),mainCut+TString::Format("&& eventClass==2"));
    }
    if (NCAT==4) {
      if (c==2) signalDmgen[c] = (RooDataSet*) sigWeighted.reduce(*w->var(deltaMgenName),mainCut+TString::Format("&& eventClass==2"));
      if (c==3) signalDmgen[c] = (RooDataSet*) sigWeighted.reduce(*w->var(deltaMgenName),mainCut+TString::Format("&& eventClass==3"));
    }

    TString myCut;
    if (NCAT==2) {
      if (c==0) myCut = "EBEB";  
      if (c==1) myCut = "EBEE";
    } else if (NCAT==3) {
      if (c==0) myCut = "EBHighR9";  
      if (c==1) myCut = "EBLowR9";
      if (c==2) myCut = "EBEE";  
    } else if (NCAT==4) {
      if (c==0) myCut = "EBHighR9";  
      if (c==1) myCut = "EBLowR9";
      if (c==2) myCut = "EEHighR9";  
      if (c==3) myCut = "EELowR9";
    }

    w->import(*signalDm[c],Rename("SigWeightDeltaMgen_cat"+myCut+"_mass"+myMass));
    
    cout << "cat " << c << ", signal[c]: " << endl;
    signalDmgen[c]->Print("v");
    cout << "---- for category " << c << ", nX for signal[c]:  " << signalDm[c]->sumEntries() << endl; 
    cout << endl;
  }

  cout << endl;
  cout << endl;
  cout << "---------------------------------------------" << endl;
  cout << endl;
  cout << endl;

  cout << "workspace summary" << endl;
  w->Print();
  cout << endl;
  cout << "---------------------------------------------" << endl;
  cout << endl;
}

// Interpolation of the convolution functions
void Interpolation(RooWorkspace* w, vector<int> masses, string coupling) {

  // Variables
  RooRealVar* mgg = w->var("mgg");    
  RooRealVar* zeroVar = w->var("zeroVar");    
  RooArgList varlist;
  if (!inZero) varlist.add(*mgg);
  if (inZero)  varlist.add(*zeroVar);

  // Morphing variable
  RooRealVar* mu;
  w->factory("mu[0,7000]");      
  mu = w->var("mu");

  // PDFs
  RooArgList pdfsCat0, pdfsCat1;
  RooArgList pdfsCat2, pdfsCat3;

  // Reference points
  int numMass = (int)masses.size();
  if (checkMass>0) numMass = (int)masses.size() -1;
  TVectorD paramVec(numMass); 

  // To plot
  RooPlot *frameCat0 = mgg->frame(Range(300,5500));   
  RooPlot *frameCat1 = mgg->frame(Range(300,5500));   
  RooPlot *frameCat2 = mgg->frame(Range(300,5500));   
  RooPlot *frameCat3 = mgg->frame(Range(300,5500));   
  if (checkMass!=0) {
    frameCat0 = mgg->frame(Range(checkMass-300,checkMass+300));   
    frameCat1 = mgg->frame(Range(checkMass-300,checkMass+300));   
    frameCat2 = mgg->frame(Range(checkMass-300,checkMass+300));   
    frameCat3 = mgg->frame(Range(checkMass-300,checkMass+300));   
  }
  if (inZero) {
    frameCat0 = zeroVar->frame(Range(-300,300));   
    frameCat1 = zeroVar->frame(Range(-300,300));   
    frameCat2 = zeroVar->frame(Range(-300,300));   
    frameCat3 = zeroVar->frame(Range(-300,300));   
  }

  // To fill the pdfs
  RooHistPdf *convRhPdf;

  // Loop over masses
  for (int ii=0; ii<(int)masses.size(); ii++) {  // loop over masses
    int theMass = masses[ii];
    TString myMass = TString::Format("%d",theMass);
    
    if (checkMass==0) 
      paramVec[ii] = (double)theMass;     
    else {
      if (theMass<checkMass)      paramVec[ii] = (double)theMass; 
      else if (theMass>checkMass) paramVec[ii-1] = (double)theMass;
    }

    for (int c=0; c<NCAT; ++c) {   

      cout << "---------- Category = " << c << ", mass = " << myMass << endl; 
      TString myCut = "EBEB"; 
      if (NCAT==2) {
	if (c==0) myCut = "EBEB";        
	if (c==1) myCut = "EBEE";  
      } else if (NCAT==3) {
	if (c==0) myCut = "EBHighR9";        
	if (c==1) myCut = "EBLowR9";        
	if (c==2) myCut = "EBEE";        
      } else if (NCAT==4) {
	if (c==0) myCut = "EBHighR9";        
	if (c==1) myCut = "EBLowR9";        
	if (c==2) myCut = "EEHighR9";        
	if (c==3) myCut = "EELowR9";        
      }

      // reading the convolution function from the workspace
      TString myConvNameA = TString(Form("Convolution_cat"+myCut+"_mass%d",theMass));
      TString myConvName  = TString(Form(myConvNameA+"_kpl"+coupling));   
      RooAbsPdf* conv = w->pdf(myConvName);    
      conv->SetTitle(myConvName);
      conv->SetName(myConvName);
      cout << "Just got the convolution pdf " << myConvName << endl;
      conv->Print();
      cout << endl;

      // moving to the corresponding roohistpdf
      if (!inZero) mgg->setBins(10000);
      if (inZero)  zeroVar->setBins(10000);
      RooDataHist *convRDH;
      if (!inZero) convRDH = conv->generateBinned(*mgg,10000,kTRUE);
      if (inZero)  convRDH = conv->generateBinned(*zeroVar,10000,kTRUE);
      TString myConvRdhNameA = TString(Form("ConvolutionRDH_cat"+myCut+"_mass%d",theMass));
      TString myConvRdhName  = TString(Form(myConvRdhNameA+"_kpl"+coupling));   
      convRDH->SetTitle(myConvRdhName);
      convRDH->SetName(myConvRdhName);
      cout << "done with RooDataHist from conv" << endl;
      convRDH->Print();
      cout << endl;

      if (!inZero) convRhPdf = new RooHistPdf("convRhPdf","convRHhPdf",*mgg,*convRDH,0);
      if (inZero)  convRhPdf = new RooHistPdf("convRhPdf","convRHhPdf",*zeroVar,*convRDH,0);
      TString myConvRhPdfNameA = TString(Form("ConvolutionRhPdf_cat"+myCut+"_mass%d",theMass));
      TString myConvRhPdfName  = TString(Form(myConvRhPdfNameA+"_kpl"+coupling));           // chiara 
      //TString myConvRhPdfName  = TString(Form(myConvRhPdfNameA+"_kpl_smearDown"+coupling));   // chiara
      convRhPdf->SetTitle(myConvRhPdfName);   
      convRhPdf->SetName(myConvRhPdfName);    
      cout << "done with RooHistPdf from conv"<< endl;
      convRhPdf->Print();
      cout << endl;

      // adding to the list of pdfs
      if (theMass!=checkMass) {   
	if (c==0) pdfsCat0.add(*convRhPdf);         
	if (c==1) pdfsCat1.add(*convRhPdf);
	if (NCAT==3) {
	  if (c==2) pdfsCat2.add(*convRhPdf);
	}
	if (NCAT==4) {
	  if (c==2) pdfsCat2.add(*convRhPdf);
	  if (c==3) pdfsCat3.add(*convRhPdf);
	}
      } 
      if (c==0) convRhPdf->plotOn(frameCat0,LineColor(kBlue), LineStyle(kSolid));
      if (c==1) convRhPdf->plotOn(frameCat1,LineColor(kBlue), LineStyle(kSolid));
      if (NCAT==3) {
	if (c==2) convRhPdf->plotOn(frameCat2,LineColor(kBlue), LineStyle(kSolid));
      }
      if (NCAT==4) {
	if (c==2) convRhPdf->plotOn(frameCat2,LineColor(kBlue), LineStyle(kSolid));
	if (c==3) convRhPdf->plotOn(frameCat3,LineColor(kBlue), LineStyle(kSolid));
      }
      cout << "convolutions added to the pdf list" << endl;

    } // loop over cat
  }   // loop over masses


  // Now morphing for the two categories
  cout << endl;
  cout << "morphing for the convolution functions" << endl;
  pdfsCat0.Print();
  pdfsCat1.Print();
  if (NCAT==3) {
    pdfsCat2.Print();
  }
  if (NCAT==4) {
    pdfsCat2.Print();
    pdfsCat3.Print();
  }

  RooMomentMorph *morphCat0 = new RooMomentMorph("morphCat0","morphCat0",*mu,varlist,pdfsCat0,paramVec,RooMomentMorph::Linear);
  TString morphCat0name = TString(Form("MorphCatEBEB_kpl"))+coupling;
  if (NCAT==3) morphCat0name = TString(Form("MorphCatEBHighR9_kpl"))+coupling;
  if (NCAT==4) morphCat0name = TString(Form("MorphCatEBHighR9_kpl"))+coupling;
  morphCat0->SetTitle(morphCat0name);
  morphCat0->SetName(morphCat0name);
  morphCat0->Print();

  RooMomentMorph *morphCat1 = new RooMomentMorph("morphCat1","morphCat1",*mu,varlist,pdfsCat1,paramVec,RooMomentMorph::Linear);
  TString morphCat1name = TString(Form("MorphCatEBEE_kpl"))+coupling;
  if (NCAT==3) morphCat1name = TString(Form("MorphCatEBLowR9_kpl"))+coupling;
  if (NCAT==4) morphCat1name = TString(Form("MorphCatEBLowR9_kpl"))+coupling;
  morphCat1->SetTitle(morphCat1name);
  morphCat1->SetName(morphCat1name);
  morphCat1->Print();

  RooMomentMorph *morphCat2;
  if (NCAT==3) {
    morphCat2 = new RooMomentMorph("morphCat2","morphCat2",*mu,varlist,pdfsCat2,paramVec,RooMomentMorph::Linear);
    TString morphCat2name = TString(Form("MorphCatEBEE_kpl"))+coupling;
    morphCat2->SetTitle(morphCat2name);
    morphCat2->SetName(morphCat2name);
    morphCat2->Print();
  }
  if (NCAT==4) {
    morphCat2 = new RooMomentMorph("morphCat2","morphCat2",*mu,varlist,pdfsCat2,paramVec,RooMomentMorph::Linear);
    TString morphCat2name = TString(Form("MorphCatEEHighR9_kpl"))+coupling;
    morphCat2->SetTitle(morphCat2name);
    morphCat2->SetName(morphCat2name);
    morphCat2->Print();
  }

  RooMomentMorph *morphCat3;
  if (NCAT==4) {
    morphCat3 = new RooMomentMorph("morphCat3","morphCat3",*mu,varlist,pdfsCat3,paramVec,RooMomentMorph::Linear);
    TString morphCat3name = TString(Form("MorphCatEELowR9_kpl"))+coupling;
    morphCat3->SetTitle(morphCat3name);
    morphCat3->SetName(morphCat3name);
    morphCat3->Print();
  }

  cout << endl;
  cout << endl;

  // Importing in the workspace
  w->import(*morphCat0);
  w->import(*morphCat1);
  if (NCAT==3) {
    w->import(*morphCat2);
  }
  if (NCAT==4) {
    w->import(*morphCat2);
    w->import(*morphCat3);
  }
  w->Print();


  // Saving the WS
  cout<< endl; 
  TString filename("/tmp/crovelli/myWSwithMorphing.root"); 
  TFile fileWs(filename,"RECREATE");
  fileWs.cd(); 
  w->writeToFile(filename);       
  cout << "Write signal workspace in: " << filename << " file" << endl;  
  cout << endl;  


  // Evaluating
  cout << endl;
  cout << endl;
  cout << "----------------------------" << endl;
  cout << endl;
  cout << "now evaluating the morphing" << endl;
  if (checkMass==0) mu->setVal(3000);   
  else mu->setVal(checkMass);   
  cout << endl;
  morphCat0->Print();
  cout << endl;
  morphCat1->Print();
  if (NCAT==3) {
    cout << endl;
    morphCat2->Print();
  }
  if (NCAT==4) {
    cout << endl;
    morphCat2->Print();
    cout << endl;
    morphCat3->Print();
  }

  // Making control plots
  TCanvas *c1 = new TCanvas("c1","c1",1);
  morphCat0->plotOn(frameCat0, LineColor(kRed), LineStyle(kDashed));
  frameCat0->Draw();
  c1->SaveAs("testCat0.png");
  c1->SetLogy(1);
  c1->SaveAs("testCat0Log.png");

  TCanvas *c2 = new TCanvas("c2","c2",1);
  morphCat1->plotOn(frameCat1, LineColor(kGreen), LineStyle(kDashed));
  frameCat1->Draw();
  c2->SaveAs("testCat1.png");
  c2->SetLogy(1);
  c2->SaveAs("testCat1Log.png");

  if (NCAT==3 || NCAT==4) {
    TCanvas *c3 = new TCanvas("c3","c3",1);
    morphCat2->plotOn(frameCat2, LineColor(kGreen), LineStyle(kDashed));
    frameCat2->Draw();
    c3->SaveAs("testCat2.png");
    c3->SetLogy(1);
    c3->SaveAs("testCat2Log.png");
  }

  if (NCAT==4) {
    TCanvas *c4 = new TCanvas("c4","c4",1);
    morphCat3->plotOn(frameCat3, LineColor(kGreen), LineStyle(kDashed));
    frameCat3->Draw();
    c4->SaveAs("testCat3.png");
    c4->SetLogy(1);
    c4->SaveAs("testCat3Log.png");
  }
}

// To run the analysis
void runfits(string coupling="01") {

  cout << endl;
  cout << endl;
  cout << "  ====================== this is 0T ================" << endl;
  cout << endl;
  cout << endl;

  RooWorkspace *w = new RooWorkspace("w");
 
  // range for the variables
  w->factory("mgg[300,5050]");      
  w->factory("mggGen[300,5050]");
  w->factory("zeroVar[-300,300]");
  w->Print("v");

  // range of masses
  vector<int> masses;
  if (!genOnly) {                     // full sim samples
    if (coupling=="01") {
      /*
      masses.push_back(500);
      masses.push_back(750);
      */
      masses.push_back(1000);
      /*
      masses.push_back(1250);
      masses.push_back(1500);
      masses.push_back(1750);
      masses.push_back(2000);
      masses.push_back(2250);
      //masses.push_back(2500);
      masses.push_back(2750);
      masses.push_back(3000);
      masses.push_back(3500);
      masses.push_back(4000);
      masses.push_back(4500);
      */
      //masses.push_back(5000);
    } else if (coupling=="001") {
      ////////////////////masses.push_back(500);
      masses.push_back(750);
      masses.push_back(1000);
      //masses.push_back(1500);
      masses.push_back(2000);
      //masses.push_back(3000);
      masses.push_back(4000);
      masses.push_back(5000);
    } else if (coupling=="02") {  
      masses.push_back(500);
      masses.push_back(750);
      masses.push_back(1000);
      masses.push_back(1500);
      masses.push_back(2000);
      masses.push_back(3000);
      masses.push_back(4000);
      masses.push_back(5000);
    }
  } else {   // fast sim samples

    /*
    for (int iGenMass=0; iGenMass<1; iGenMass++) {
      int thisMass = 3200 + iGenMass*2;
      masses.push_back(thisMass); 
    }
    */

    for (int iGenMass=0; iGenMass<250; iGenMass++) {
      int thisMass = 500 + iGenMass*2;
      masses.push_back(thisMass); 
    }

    /*
    for (int iGenMass=0; iGenMass<150; iGenMass++) {
      int thisMass = 1000 + iGenMass*4;
      masses.push_back(thisMass); 
    }
    for (int iGenMass=0; iGenMass<34; iGenMass++) {
      int thisMass = 1600 + iGenMass*100;
      if (thisMass==4900 && coupling=="005") continue;
      if (thisMass==4900 && coupling=="007") continue;
      masses.push_back(thisMass); 
    }
    */
  }

  // loading data for the wanted coupling for control plots and make the roodatasets with minimal selection - full sim samples only
  if (!genOnly) {
    cout << endl; 
    cout << "------------------------------------------" << endl; 
    cout << endl; 
    cout << "Now add signal data" << endl;
    for (int ii=0; ii<(int)masses.size(); ii++) {
      int theMass = masses[ii];
      cout << "adding mass " << theMass << endl;
      AddSigData(w, theMass, coupling);   
    }
  }


  // preparing roorealvars for the gen only samples
  if (genOnly) {
    cout << endl; 
    cout << "------------------------------------------" << endl; 
    cout << endl; 
    cout << "Now add mass dependent roorealvar - for gen level scan analysis" << endl;
    for (int ii=0; ii<(int)masses.size(); ii++) {
      int theMass = masses[ii];
      
      RooRealVar* deltaM = new RooRealVar("deltaM", "deltaM", -10000, 10000, "GeV");   
      TString deltaMname = TString::Format("deltaM_mass%d",theMass);
      deltaM->SetName(deltaMname);
      deltaM->SetTitle(deltaMname);
      w->import(*deltaM);
      
      RooRealVar* mH = new RooRealVar("mH", "mH", 0, 10000, "GeV");   
      TString mHname = TString::Format("mH_mass%d",theMass);
      mH->SetName(mHname);
      mH->SetTitle(mHname);
      mH->setVal(theMass);
      mH->setConstant();
      w->import(*mH);
      
      RooRealVar* deltaMgen = new RooRealVar("deltaMgen", "deltaMgen", -10000, 10000, "GeV");   
      TString deltaMgenName = TString::Format("deltaMgen_mass%d",theMass);
      deltaMgen->SetName(deltaMgenName);
      deltaMgen->SetTitle(deltaMgenName);
      w->import(*deltaMgen);
    }
    w->Print();
  }

  // Now make the convolution 
  cout << endl;    
  cout << endl;    
  cout << "--------------------------------------------------------------------------" << endl;     
  cout << endl;    
  cout << "Now make the convolution of the two histograms" << endl;  
  for (int ii=0; ii<(int)masses.size(); ii++) {  
    int theMass = masses[ii];    
    cout << "convolution for mass " << theMass << " and coupling " << coupling << endl;      
    ConvolutionFromRDH(w, theMass,coupling);        
  }

  // interpolation
  cout << endl; 
  cout << endl; 
  cout << "--------------------------------------------------------------------------" << endl; 
  cout << endl;    
  cout << "Now make the interpolation" << endl; 
  // Interpolation(w, masses, coupling);

  return;
}

