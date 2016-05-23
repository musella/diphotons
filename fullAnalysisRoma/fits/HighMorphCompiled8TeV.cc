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

  // RooRealVars
  RooRealVar* mgg = w->var("mgg");  
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

  // Intrinsic width centred in zero
  RooFormulaVar *deltaMgen_formula = new RooFormulaVar("deltaMgen_formula","","@0-@1",RooArgList(*w->var("mgg"),*mH)); 

  // To move from deltaM to mgg
  RooArgList histObsRes;
  histObsRes.add(*deltaM);

  // To move from deltaMgen to mgg
  RooArgList histObsInw;
  histObsInw.add(*deltaMgen);

  // Files with resolution and intrinsic width RDHs
  TFile *fileRes = new TFile("ResHistosGenOnlyScan.root");
  TFile *fileInw = new TFile("WidthHistosGenOnlyScan.root");

  // Plots to check
  TCanvas* c1 = new TCanvas("c1","PhotonsMass",0,0,800,800);  
  c1->cd(1);    

  // for resolution
  TString myDeltaM_formulaA = TString(Form("deltaM_formula_mass%d",mass));
  TString myDeltaM_formula  = TString(Form(myDeltaM_formulaA+"_kpl"+coupling));   
  deltaM_formula->SetTitle(myDeltaM_formula);
  deltaM_formula->SetName(myDeltaM_formula);
  RooArgList pdfObsRes;
  pdfObsRes.add(*deltaM_formula);  

  // for intrinsic width
  TString myDeltaMgen_formulaA = TString(Form("deltaMgen_formula_mass%d",mass));
  TString myDeltaMgen_formula  = TString(Form(myDeltaMgen_formulaA+"_kpl"+coupling));   
  deltaMgen_formula->SetTitle(myDeltaMgen_formula);
  deltaMgen_formula->SetName(myDeltaMgen_formula);
  RooArgList pdfObsInw;
  pdfObsInw.add(*deltaMgen_formula);

  // reading the roodatahists 
  TString myRDHA = TString(Form("resolRDH_mass%d",mass));     
  RooDataHist *resRDH = (RooDataHist*)fileRes->Get(myRDHA);    
  resRDH->Print(); 
  TString myRDHBa = TString(Form("widthRDH_mass%d",mass));   
  //TString myRDHB = TString(Form(myRDHBa))+TString(Form("_kpl"))+TString(Form(coupling));             // chiara: when reading width for 8TeV samples
  TString myRDHB = TString(Form(myRDHBa))+TString(Form("_catEBEB_kpl"))+TString(Form(coupling));       // chiara: when reading width for 13TeV samples
  RooDataHist *inwRDH = (RooDataHist*)fileInw->Get(myRDHB);   
  cout << myRDHB << endl;
  inwRDH->Print();        
  cout << "RooDataHists taken" << endl;       

  // creating the roohistpdfs    
  TString myHistPdfResNameA = TString(Form("myHistPdfRes_mass%d",mass));
  TString myHistPdfResName  = TString(Form(myHistPdfResNameA+"_kpl"+coupling));   
  RooHistPdf *myHistPdfRes = new RooHistPdf("myHistPdfRes","myHistPdfRes",pdfObsRes, histObsRes,*resRDH,0) ;  
  myHistPdfRes->SetTitle(myHistPdfResName);
  myHistPdfRes->SetName(myHistPdfResName);
  myHistPdfRes->Print(); 

  TString myHistPdfInwNameA = TString(Form("myHistPdfInw_mass%d",mass));
  TString myHistPdfInwName  = TString(Form(myHistPdfInwNameA+"_kpl"+coupling));   
  RooHistPdf *myHistPdfInw = new RooHistPdf("myHistPdfInw","myHistPdfInw",pdfObsInw, histObsInw,*inwRDH,0) ;  
  myHistPdfInw->SetTitle(myHistPdfInwName);
  myHistPdfInw->SetName(myHistPdfInwName);
  myHistPdfInw->Print(); 
  
  cout << "RooHistPdfs done" << endl;      
    
  // Now make the convolution    
  mgg->setBins(10000, "cache");
  TString myConvNameA = TString(Form("Convolution_mass%d",mass));
  TString myConvName  = TString(Form(myConvNameA+"_kpl"+coupling));   
  RooFFTConvPdf *convol = new RooFFTConvPdf("convol","convol",*mgg,*myHistPdfInw,*myHistPdfRes);          
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
  //if (coupling=="001" && mass==3200) convol->setBufferFraction(0.09);
  //
  if (coupling=="001" && mass==528)  convol->setBufferFraction(0.11);    
  if (coupling=="001" && mass==530)  convol->setBufferFraction(0.11);    
  if (coupling=="001" && mass==582)  convol->setBufferFraction(0.11);    
  if (coupling=="001" && mass==634)  convol->setBufferFraction(0.11);
  if (coupling=="001" && mass==640)  convol->setBufferFraction(0.11);
  if (coupling=="001" && mass==678)  convol->setBufferFraction(0.11);
  if (coupling=="001" && mass==686)  convol->setBufferFraction(0.11);
  if (coupling=="001" && mass==732)  convol->setBufferFraction(0.11);
  if (coupling=="001" && mass==738)  convol->setBufferFraction(0.11);
  if (coupling=="001" && mass==844)  convol->setBufferFraction(0.11);
  if (coupling=="001" && mass==852)  convol->setBufferFraction(0.11);
  if (coupling=="001" && mass==896)  convol->setBufferFraction(0.11);
  if (coupling=="001" && mass==948)  convol->setBufferFraction(0.11);
  if (coupling=="001" && mass==1054) convol->setBufferFraction(0.11);
  if (coupling=="001" && mass==1112) convol->setBufferFraction(0.11);
  if (coupling=="001" && mass==1000) convol->setBufferFraction(0.11);
  if (coupling=="001" && mass==1050) convol->setBufferFraction(0.11);
  if (coupling=="001" && mass==3200) convol->setBufferFraction(0.12);
  if (coupling=="001" && mass==4650) convol->setBufferFraction(0.11);
  if (coupling=="001" && mass==1540) convol->setBufferFraction(0.11);
  if (coupling=="001" && mass==1580) convol->setBufferFraction(0.11);
  if (coupling=="001" && mass==1592) convol->setBufferFraction(0.11);
  // ad hoc corrections - end
  convol->SetTitle(myConvName);
  convol->SetName(myConvName);
  cout << "Convolution done" << endl;   

  // Both
  convol->Print();     
  cout << "Convolution done and printed. now plot" << endl;   

  // Importing the convolution in the workspace
  w->import(*convol);   
  
  // moving to the corresponding roohistpdf
  mgg->setBins(10000);
  RooDataHist *convRDH;
  convRDH = convol->generateBinned(*mgg,10000,kTRUE);
  TString myConvRdhNameA = TString(Form("ConvolutionRDH_mass%d",mass));
  TString myConvRdhName  = TString(Form(myConvRdhNameA+"_kpl"+coupling));   
  convRDH->SetTitle(myConvRdhName);
  convRDH->SetName(myConvRdhName);
  cout << "done with RooDataHist from conv" << endl;
  convRDH->Print();
  cout << endl;
    
  RooHistPdf *convRhPdf;
  convRhPdf = new RooHistPdf("convRhPdf","convRHhPdf",*mgg,*convRDH,0);
  TString myConvRhPdfNameA = TString(Form("ConvolutionRhPdf_mass%d",mass));
  TString myConvRhPdfName  = TString(Form(myConvRhPdfNameA+"_kpl"+coupling));   
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

// To run the analysis
void runfits(string coupling="01") {

  RooWorkspace *w = new RooWorkspace("w");
 
  // range for the variables
  w->factory("mgg[300,5050]");      
  w->factory("mggGen[300,5050]");
  w->Print("v");

  // range of masses
  vector<int> masses;

  for (int iGenMass=0; iGenMass<250; iGenMass++) {
    int thisMass = 500 + iGenMass*2;
    masses.push_back(thisMass); 
  }
  /*
  for (int iGenMass=0; iGenMass<150; iGenMass++) {
    int thisMass = 1000 + iGenMass*4;
    masses.push_back(thisMass); 
  }
  for (int iGenMass=0; iGenMass<25; iGenMass++) { // extended range
    int thisMass = 1600 + iGenMass*100;
    masses.push_back(thisMass); 
  }
  */

  // preparing roorealvars
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

  // Now make the convolution 
  cout << endl;    
  cout << endl;    
  cout << "-----------------------------------------------------" << endl;     
  cout << endl;    
  cout << "Now make the convolution of the two histograms" << endl;  
  for (int ii=0; ii<(int)masses.size(); ii++) {  
    int theMass = masses[ii];    
    cout << "convolution for mass " << theMass << " and coupling " << coupling << endl; 
    ConvolutionFromRDH(w,theMass,coupling);        
  }

  return;
}

