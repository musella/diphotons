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

// Definition of the variables in the input ntuple
RooArgSet* defineVariables() {

  RooRealVar* mgg        = new RooRealVar("mgg",        "M(gg)",        0, 7000, "GeV");   
  RooRealVar* mggGen     = new RooRealVar("mggGen",     "M(gg) gen",    0, 7000, "GeV");   
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

  // Resolution centred in zero in mgg
  RooFormulaVar *deltaM_formula = new RooFormulaVar("deltaM_formula","","@0",RooArgList(*w->var("mgg")));

  // Intrinsic width centred in mH in mgg   
  RooFormulaVar *deltaMgen_formula = new RooFormulaVar("deltaMgen_formula","","@0-@1",RooArgList(*w->var("mgg"),*mH)); 

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
  if (genOnly)  fileInw = new TFile("IntrinsicWidthHistosGenOnlyK02.root");

  // Plots to check
  TCanvas* c1 = new TCanvas("c1","PhotonsMass",0,0,800,800);  
  c1->cd(1);    

  for (int c=0; c<NCAT; ++c) {  

    cout << "---------- Category = " << c << endl;    
    TString myCut = "EBEB";   
    if (c==1) myCut = "EBEE";  

    // for resolution
    TString myDeltaM_formulaA = TString(Form("deltaM_formula_cat"+myCut+"_mass%d",mass));
    TString myDeltaM_formula  = TString(Form(myDeltaM_formulaA+"_kpl"+coupling));   
    deltaM_formula->SetTitle(myDeltaM_formula);
    deltaM_formula->SetName(myDeltaM_formula);
    RooArgList pdfObsRes;
    pdfObsRes.add(*deltaM_formula);  

    // for intrinsic width
    TString myDeltaMgen_formulaA = TString(Form("deltaMgen_formula_cat"+myCut+"_mass%d",mass));
    TString myDeltaMgen_formula  = TString(Form(myDeltaMgen_formulaA+"_kpl"+coupling));   
    deltaMgen_formula->SetTitle(myDeltaMgen_formula);
    deltaMgen_formula->SetName(myDeltaMgen_formula);
    RooArgList pdfObsInw;
    pdfObsInw.add(*deltaMgen_formula);

    // reading the roodatahists 
    TString myRDHA = TString(Form("resolRDH_mass%d_cat",mass)+myCut);     
    RooDataHist *resRDH = (RooDataHist*)fileRes->Get(myRDHA);    
    resRDH->Print();   
    TString myRDHBa = TString(Form("intWidthRDH_mass%d_cat",mass)+myCut);   
    TString myRDHB = TString(Form(myRDHBa))+TString(Form("_kpl"))+TString(Form(coupling));
    RooDataHist *inwRDH = (RooDataHist*)fileInw->Get(myRDHB);   
    inwRDH->Print();        
    cout << "RooDataHists taken" << endl;       

    // creating the roohistpdfs    
    TString myHistPdfResNameA = TString(Form("myHistPdfRes_cat"+myCut+"_mass%d",mass));
    TString myHistPdfResName  = TString(Form(myHistPdfResNameA+"_kpl"+coupling));   
    RooHistPdf *myHistPdfRes = new RooHistPdf("myHistPdfRes","myHistPdfRes",pdfObsRes, histObsRes,*resRDH,0) ;  
    myHistPdfRes->SetTitle(myHistPdfResName);
    myHistPdfRes->SetName(myHistPdfResName);
    myHistPdfRes->Print(); 
    //
    TString myHistPdfInwNameA = TString(Form("myHistPdfInw_cat"+myCut+"_mass%d",mass));
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
    mgg->setBins(10000, "cache");
    TString myConvNameA = TString(Form("Convolution_cat"+myCut+"_mass%d",mass));
    TString myConvName  = TString(Form(myConvNameA+"_kpl"+coupling));   
    RooFFTConvPdf convol("convol","convol",*mgg,*myHistPdfInw,*myHistPdfRes);          
    convol.SetTitle(myConvName);
    convol.SetName(myConvName);

    // Both
    convol.Print();     
    cout << "done!!" << endl;   

    // Fit and Plot - only for full sim samples
    if (!genOnly) {

      int fitMin = mass-250;        
      int fitMax = mass+250;        
      if (mass>=2000) {
	fitMin = mass-750;        
	fitMax = mass+750;        
      } 
      
      TString myUnbDS = TString(Form("SigWeight_cat"+myCut+"_mass%d",mass));   
      sigToFit[c] = (RooDataSet*) w->data(myUnbDS); 
      sigToFit[c]->Print();      
      
      RooPlot* myPlot1 = mgg->frame(Range(fitMin,fitMax),Bins(100));   
      sigToFit[c]->plotOn(myPlot1, LineColor(kRed), LineStyle(kDashed));           
      double max = myPlot1->GetMaximum(); 
      
      RooPlot* myPlot = mgg->frame(Range(fitMin,fitMax),Bins(50)); 
      if (coupling=="001") myPlot = mgg->frame(Range(fitMin,fitMax),Bins(40));
      myPlot->SetTitle("Convolution, cat"+myCut);    
      sigToFit[c]->plotOn(myPlot, LineColor(kRed), LineStyle(kDashed));         
      convol.plotOn(myPlot, LineColor(kBlue));  
      myHistPdfRes->plotOn(myPlot, LineColor(kRed));
      myHistPdfInw->plotOn(myPlot, LineColor(kYellow));
      
      myPlot->GetYaxis()->SetRangeUser(0.0001, max*3.);      
      myPlot->Draw();           
      TString canvasName = TString(Form("closure_cat"+myCut+"_mass"+myMass+"_kpl"+coupling+".png"));
      c1->SetLogy(0);
      c1->SaveAs(canvasName); 
      c1->SetLogy(1);
      canvasName = TString(Form("closure_cat"+myCut+"_mass"+myMass+"_kpl"+coupling+"_log.png"));
      c1->SaveAs(canvasName); 
    }

    // Importing the convolution in the workspace
    w->import(convol);   
    
    delete myHistPdfRes;    
    delete myHistPdfInw;     
  }

  // deleting
  delete fileInw;   
  delete fileRes;      

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
  TString inDir = "../macro/allFiles/";  
  TChain* sigTree = new TChain();
  cout << "reading file " 
       << inDir+TString(Form("FormSigMod_kpl"))+coupling+TString(Form("_M%d.root/DiPhotonTree", mass)) << endl;
  sigTree->Add(inDir+TString(Form("FormSigMod_kpl"))+coupling+TString(Form("_M%d.root/DiPhotonTree", mass)));
  sigTree->SetTitle("sigTree");
  sigTree->SetName("sigTree");
  
  // Minimal common preselection cut on mgg and mggGen
  TString mainCut = TString::Format("mgg>=0 && mgg<=7000 && mggGen>=0 && mggGen<=7000");  
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

    TString myCut;
    if (c==0)      myCut = "EBEB";  
    else if (c==1) myCut = "EBEE";
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

    TString myCut;
    if (c==0)      myCut = "EBEB";  
    else if (c==1) myCut = "EBEE";
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

    TString myCut;
    if (c==0)      myCut = "EBEB";  
    else if (c==1) myCut = "EBEE";
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
  RooArgList varlist;
  varlist.add(*mgg);

  // Morphing variable
  RooRealVar* mu;
  w->factory("mu[0,7000]");      
  mu = w->var("mu");

  // PDFs
  RooArgList pdfsCat0, pdfsCat1;

  // Reference points
  int numMass = (int)masses.size();
  if (checkMass>0) numMass = (int)masses.size() -1;
  TVectorD paramVec(numMass); 

  // To plot
  RooPlot *frameCat0 = mgg->frame(Range(300,5500));   
  RooPlot *frameCat1 = mgg->frame(Range(300,5500));   
  if (checkMass!=0) {
    frameCat0 = mgg->frame(Range(checkMass-300,checkMass+300));   
    frameCat1 = mgg->frame(Range(checkMass-300,checkMass+300));   
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
      if (c==1) myCut = "EBEE";  

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
      mgg->setBins(10000);
      RooDataHist *convRDH = conv->generateBinned(*mgg,10000,kTRUE);
      TString myConvRdhNameA = TString(Form("ConvolutionRDH_cat"+myCut+"_mass%d",theMass));
      TString myConvRdhName  = TString(Form(myConvRdhNameA+"_kpl"+coupling));   
      convRDH->SetTitle(myConvRdhName);
      convRDH->SetName(myConvRdhName);
      cout << "done with RooDataHist from conv" << endl;
      convRDH->Print();
      cout << endl;

      convRhPdf = new RooHistPdf("convRhPdf","convRHhPdf",*mgg,*convRDH,0);
      TString myConvRhPdfNameA = TString(Form("ConvolutionRhPdf_cat"+myCut+"_mass%d",theMass));
      TString myConvRhPdfName  = TString(Form(myConvRhPdfNameA+"_kpl"+coupling));   
      convRhPdf->SetTitle(myConvRhPdfName);   
      convRhPdf->SetName(myConvRhPdfName);    
      cout << "done with RooHistPdf from conv"<< endl;
      convRhPdf->Print();
      cout << endl;

      // adding to the list of pdfs
      if (theMass!=checkMass) {   
	if (c==0) pdfsCat0.add(*convRhPdf);         
	if (c==1) pdfsCat1.add(*convRhPdf);
      } 
      if (c==0) convRhPdf->plotOn(frameCat0,LineColor(kBlue), LineStyle(kSolid));
      if (c==1) convRhPdf->plotOn(frameCat1,LineColor(kBlue), LineStyle(kSolid));
      cout << "convolutions added to the pdf list" << endl;

    } // loop over cat
  }   // loop over masses


  // Now morphing for the two categories
  cout << endl;
  cout << "morphing for the convolution functions" << endl;
  pdfsCat0.Print();
  pdfsCat1.Print();
  RooMomentMorph *morphCat0 = new RooMomentMorph("morphCat0","morphCat0",*mu,varlist,pdfsCat0,paramVec,RooMomentMorph::Linear);
  TString morphCat0name = TString(Form("MorphCatEBEB_kpl"))+coupling;
  morphCat0->SetTitle(morphCat0name);
  morphCat0->SetName(morphCat0name);
  morphCat0->Print();
  RooMomentMorph *morphCat1 = new RooMomentMorph("morphCat1","morphCat1",*mu,varlist,pdfsCat1,paramVec,RooMomentMorph::Linear);
  TString morphCat1name = TString(Form("MorphCatEBEE_kpl"))+coupling;
  morphCat1->SetTitle(morphCat1name);
  morphCat1->SetName(morphCat1name);
  morphCat1->Print();
  cout << endl;
  cout << endl;

  // Importing in the workspace
  w->import(*morphCat0);
  w->import(*morphCat1);
  w->Print();

  // Saving the WS
  cout<< endl; 
  TString filename("myWSwithMorphing.root"); 
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
}

// To run the analysis
void runfits(string coupling="01") {

  RooWorkspace *w = new RooWorkspace("w");
 
  // range for the variables
  w->factory("mgg[0,7000]");      
  w->factory("mggGen[0,7000]");
  w->Print("v");

  // range of masses
  vector<int> masses;
  if (!genOnly) {                     // full sim samples
    if (coupling=="01") {
      masses.push_back(500);
      masses.push_back(750);
      masses.push_back(1000);
      masses.push_back(1250);
      masses.push_back(1500);
      masses.push_back(1750);
      masses.push_back(2000);
      masses.push_back(2250);
      masses.push_back(2750);
      masses.push_back(3000);
      masses.push_back(3500);
      masses.push_back(4000);
      masses.push_back(4500);
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
  } else {
    if (coupling=="02") {
      masses.push_back(500);
      masses.push_back(625);
      masses.push_back(750);
      masses.push_back(875);
      masses.push_back(1000);
      masses.push_back(1125);
      masses.push_back(1250);
      masses.push_back(1375);
      masses.push_back(1625);
      masses.push_back(1750);
      masses.push_back(1875);
      masses.push_back(2000);
      masses.push_back(2125);
      masses.push_back(2250);
      masses.push_back(2375);
      masses.push_back(2500);
      masses.push_back(2625);
      masses.push_back(2750);
      masses.push_back(2875);
      masses.push_back(3000);
      masses.push_back(3500);
      masses.push_back(3750);
      masses.push_back(3875);
      masses.push_back(4000);
      masses.push_back(4125);
      masses.push_back(4250);
      masses.push_back(4375);
      masses.push_back(4625);
      masses.push_back(4875);
      masses.push_back(5000);
    }
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
  Interpolation(w, masses, coupling);

  return;
}

