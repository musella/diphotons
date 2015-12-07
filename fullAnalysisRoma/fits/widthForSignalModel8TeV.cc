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

// Preparing the intrinsic width histogram
void MakeIntrinsicWidthHisto(TString filename, bool newFile, int mass, TString coupling) {

  TString myMass = TString::Format("%d",mass);

  // the roorealvar 
  RooRealVar* deltaMgen;   // chiara
  if (coupling=="001") deltaMgen = new RooRealVar("deltaMgen", "",   -70, 70,   "GeV"); 
  if (coupling=="005") deltaMgen = new RooRealVar("deltaMgen", "",  -500, 500,  "GeV"); 
  if (coupling=="01")  deltaMgen = new RooRealVar("deltaMgen", "", -1000, 1000, "GeV");

  // Output file 
  TFile *theIntrinsicWFile;
  if (newFile) theIntrinsicWFile = new TFile(filename,"RECREATE");
  else theIntrinsicWFile = new TFile(filename,"UPDATE");
  
  // Input file and tree
  TString inDir = "../macro/allFiles8TeV_resol1perc/";
  TChain* sigTree = new TChain();
  cout << "reading file " 
       << inDir+TString(Form("FormSigMod_kpl"))+coupling+TString(Form("_M%d.root/DiPhotonTree", mass)) << endl;
  sigTree->Add(inDir+TString(Form("FormSigMod_kpl"))+coupling+TString(Form("_M%d.root/DiPhotonTree", mass)));
  sigTree->SetTitle("sigTree");
  sigTree->SetName("sigTree");

  // Minimal common preselection cut on mgg and mggGen
  TString mainCut = TString::Format("mgg>=0 && mgg<=12000 && mggGen>=0 && mggGen<=12000");   

  // Histos
  TH1D *intWidthH;   // chiara
  if (coupling=="001") intWidthH = new TH1D("intWidthH","intWidthH",  70,  -70.,70.);  
  if (coupling=="005") intWidthH = new TH1D("intWidthH","intWidthH", 500, -500.,500.);  
  if (coupling=="01")  intWidthH = new TH1D("intWidthH","intWidthH",1000,-1000.,1000.);  
  intWidthH->Sumw2();

  // Projecting the tree
  TString express = TString::Format("mggGen-%d",mass);
  sigTree->Project("intWidthH",express,mainCut);
  
  // Now make the roodatahist
  RooDataHist intWidthRDH("intWidthRDH","intWidthRDH",*deltaMgen,Import(*intWidthH));   
  cout << "Intrinsic width datahist summary for mass " << mass << endl;  
  intWidthRDH.Print();
  cout << endl;  

  // Saving in the root file
  theIntrinsicWFile->cd();
  TString nameRDH = TString::Format("intWidthRDH_mass%d",mass)+TString::Format("_kpl")+coupling;
  intWidthRDH.Write(nameRDH);

  // Plot to check
  TCanvas *c1 = new TCanvas("c1","c1",1);
  RooPlot* myPlot; //chiara
  if (coupling=="001") myPlot = deltaMgen->frame(Range(  -70,70), Bins(70));    
  if (coupling=="005") myPlot = deltaMgen->frame(Range( -500,500), Bins(500));       
  if (coupling=="01")  myPlot = deltaMgen->frame(Range(-1000,1000),Bins(1000));    
  intWidthRDH.plotOn(myPlot, LineColor(kRed), LineStyle(kDashed));    
  myPlot->Draw();
  c1->SetLogy();
  TString canvasName = TString(Form("CheckIntrinsicWidth_mass"))+TString(Form(myMass))+TString(Form("kpl"))+TString(Form(coupling))+TString(Form("_LOG.png"));
  c1->SaveAs(canvasName);       
  RooPlot* myPlotZoom = deltaMgen->frame(Range(-300,300),Bins(300));  
  if (coupling=="001") myPlotZoom = deltaMgen->frame(Range(-12,12),Bins(12));
  if (coupling=="005") myPlotZoom = deltaMgen->frame(Range(-60,60),Bins(60));
  if (coupling=="007") myPlotZoom = deltaMgen->frame(Range(-100,100),Bins(100));
  intWidthRDH.plotOn(myPlotZoom, LineColor(kRed), LineStyle(kDashed));
  myPlotZoom->Draw();
  canvasName = TString(Form("CheckIntrinsicWidthZoom_mass"))+TString(Form(myMass))+TString(Form("kpl"))+TString(Form(coupling))+TString(Form(".png"));
  c1->SetLogy(0);
  c1->SaveAs(canvasName);
  
  delete c1;
  delete intWidthH;
  
  // Closing the output file
  theIntrinsicWFile->Close();
}

//-------------------------------------------------------
void WidthInterpolation(RooWorkspace* w, vector<int> masses, string coupling, bool newfile) {  
  
  // Variable for the intrinsic width
  RooRealVar* deltaMgen;   // chiara
  if (coupling=="001") deltaMgen = new RooRealVar("deltaMgen", "",   -70, 70,   "GeV"); 
  if (coupling=="005") deltaMgen = new RooRealVar("deltaMgen", "",  -500, 500,   "GeV"); 
  if (coupling=="01")  deltaMgen = new RooRealVar("deltaMgen", "", -1000, 1000, "GeV");
  RooArgList varlist;  
  varlist.add(*deltaMgen);   

  // Morphing variable
  RooRealVar* muWidth = new RooRealVar("muWidth", "", 0, 12000, "GeV");    

  // PDFs
  RooArgList pdfs; 

  // Reference points 
  int numMass = (int)masses.size(); 
  TVectorD paramVec(numMass); 
  
  // To plot    
  RooPlot *frame;
  if (coupling=="001") frame = deltaMgen->frame(Range(  -70,70), Bins(70));    
  if (coupling=="005") frame = deltaMgen->frame(Range( -500,500), Bins(500));       
  if (coupling=="01")  frame = deltaMgen->frame(Range(-1000,1000),Bins(1000));    
  
  // Files with the roodatahists
  TFile *fileWidth = new TFile("IntrinsicWidthHistos.root"); 

  // Loop over masses 
  for (int ii=0; ii<(int)masses.size(); ii++) { 
    int theMass = masses[ii];   
    TString myMass = TString::Format("%d",theMass); 
    
    // Reference points 
    paramVec[ii] = (double)theMass;  

    // reading the roodatahist from the file  
    TString myRDH = TString(Form("intWidthRDH_mass%d",theMass))+TString(Form("_kpl"))+coupling;
    RooDataHist *widthRDH = (RooDataHist*)fileWidth->Get(myRDH);    
    widthRDH->Print();   
    cout << "Just got the width RooDataHist " << myRDH << endl;     
    cout << endl;         
    
    // moving to the corresponding roohistpdf  
    TString myRHpdfName = TString(Form("intWidthRHPDF_mass%d",theMass))+TString(Form("_kpl"))+coupling;
    RooHistPdf *myHistPdfWidth = new RooHistPdf("myHistPdfWidth","myHistPdfWidth",*deltaMgen,*widthRDH,0) ;  
    myHistPdfWidth->SetTitle(myRHpdfName);      
    myHistPdfWidth->SetName(myRHpdfName);            
    myHistPdfWidth->Print();   
    cout << "Just got the width RooHistPdf " << myRHpdfName << endl;    
    cout << endl;   

    // adding to the list of pdfs    
    pdfs.add(*myHistPdfWidth); 
    cout << "RooHistPdfs added to the pdf list" << endl;  
    cout << endl;  
    
    // plot to check  
    myHistPdfWidth->plotOn(frame,LineColor(kBlue), LineStyle(kSolid));     
    
  }   // loop over masses
  
  // Now morphing for the two categories    
  cout << endl;   
  cout << "morphing for the intrinsic width functions" << endl; 
  pdfs.Print();  
  RooMomentMorph *morphWidth = new RooMomentMorph("morphWidth","morphWidth",*muWidth,varlist,pdfs,paramVec,RooMomentMorph::Linear);
  morphWidth->Print();      
  cout << endl;
  cout << endl;
  
  // Close the input file 
  fileWidth->Close();     

  // Now we evaluate with a fine mass scan samples and save the corresponding roodatahists  
  cout << endl;
  cout << endl;
  cout << "----------------------------" << endl;  
  cout << endl;
  cout << "now evaluating the morphing every 50GeV, from 500GeV to 5000GeV: doing mass " << endl; 
  
  // This is to have 2GeV steps between 500 and 1000 GeV 
  for (int iGenMass=0; iGenMass<250; iGenMass++) {  
    int thisMass = 500 + iGenMass*2; 
    cout << "Fine scan (2GeV): " << thisMass << endl;  
    muWidth->setVal(thisMass);
    RooDataHist *fittWidthRDH; 
    if (coupling=="001") deltaMgen->setBins(70);
    if (coupling=="005") deltaMgen->setBins(500);
    if (coupling=="01")  deltaMgen->setBins(1000);
    fittWidthRDH = morphWidth->generateBinned(*deltaMgen,10000,kTRUE);   
    fittWidthRDH->Print();
    TString myFitRDHa = TString::Format("widthRDH_mass%d",thisMass);    
    TString myFitRDH  = myFitRDHa+"_kpl"+coupling;
    fittWidthRDH->SetTitle(myFitRDH);  
    fittWidthRDH->SetName(myFitRDH);  
    w->import(*fittWidthRDH);
  }

  // Then 4GeV steps between 1000 and 1600 GeV    
  for (int iGenMass=0; iGenMass<150; iGenMass++) { 
    int thisMass = 1000 + iGenMass*4.;  
    cout << "Medium (4GeV) scan: " << thisMass << endl;  
    muWidth->setVal(thisMass);
    RooDataHist *fittWidthRDH; 
    if (coupling=="001") deltaMgen->setBins(70);
    if (coupling=="005") deltaMgen->setBins(500);
    if (coupling=="01")  deltaMgen->setBins(1000);
    fittWidthRDH = morphWidth->generateBinned(*deltaMgen,10000,kTRUE);   
    fittWidthRDH ->Print();
    TString myFitRDHa = TString::Format("widthRDH_mass%d",thisMass);    
    TString myFitRDH  = myFitRDHa+"_kpl"+coupling;
    fittWidthRDH->SetTitle(myFitRDH);  
    fittWidthRDH->SetName(myFitRDH);  
    w->import(*fittWidthRDH);
  }
  
  // Finally 10GeV steps between 1600 and 3500 GeV    
  for (int iGenMass=0; iGenMass<19; iGenMass++) {  
    int thisMass = 1600 + iGenMass*100.;   
    cout << "Coarse (100GeV) scan: " << thisMass << endl; 
    muWidth->setVal(thisMass);
    RooDataHist *fittWidthRDH; 
    if (coupling=="001") deltaMgen->setBins(70);
    if (coupling=="005") deltaMgen->setBins(500);
    if (coupling=="01")  deltaMgen->setBins(1000);
    fittWidthRDH = morphWidth->generateBinned(*deltaMgen,10000,kTRUE);   
    fittWidthRDH->Print();
    TString myFitRDHa = TString::Format("widthRDH_mass%d",thisMass);    
    TString myFitRDH  = myFitRDHa+"_kpl"+coupling;
    fittWidthRDH->SetTitle(myFitRDH);  
    fittWidthRDH->SetName(myFitRDH);  
    w->import(*fittWidthRDH);
  }
  
  // Finally saving in a second rootfile 
  cout << endl;
  cout << endl;
  cout << "----------------------------" << endl;  
  cout << endl;
  cout << "Now salving the histos in a root file" << endl;  
  TFile *fileFittoWidth;
  if (newfile) fileFittoWidth = new TFile("WidthHistosGenOnlyScan.root","RECREATE");  
  else fileFittoWidth = new TFile("WidthHistosGenOnlyScan.root","UPDATE");  
  fileFittoWidth->cd();

  for (int iGenMass=0; iGenMass<250; iGenMass++) {   
    int thisMass = 500 + iGenMass*2;  
    TString myFitRDHa = TString::Format("widthRDH_mass%d",thisMass);
    TString myFitRDH  = myFitRDHa+TString::Format("_kpl")+coupling;
    RooDataHist *RDH = (RooDataHist*)w->data(myFitRDH);   
    RDH->Write();     
  }

  for (int iGenMass=0; iGenMass<150; iGenMass++) {   
    int thisMass = 1000 + iGenMass*4;  
    TString myFitRDHa = TString::Format("widthRDH_mass%d",thisMass);
    TString myFitRDH  = myFitRDHa+TString::Format("_kpl")+coupling;
    RooDataHist *RDH = (RooDataHist*)w->data(myFitRDH);   
    RDH->Write();     
  }

  for (int iGenMass=0; iGenMass<19; iGenMass++) {   
    int thisMass = 1600 + iGenMass*100;  
    TString myFitRDHa = TString::Format("widthRDH_mass%d",thisMass);    
    TString myFitRDH  = myFitRDHa+TString::Format("_kpl")+coupling;
    RooDataHist *RDH = (RooDataHist*)w->data(myFitRDH);   
    RDH->Write();     
  }

  fileFittoWidth->Close();
}

// To run the analysis
void runfits(string coupling="01") {

  RooWorkspace *w = new RooWorkspace("w");    

  cout << "running for coupling " << coupling << endl;

  // range of masses - to be used to make the convolution
  vector<int> masses;
    if (coupling=="001") {         
      masses.push_back(750);
      masses.push_back(1000);
      masses.push_back(1250);
      masses.push_back(1500);
      masses.push_back(1750);
      masses.push_back(2000);
      masses.push_back(2250);
      masses.push_back(2500);
      masses.push_back(3000);
    } else if (coupling=="005") {  
      masses.push_back(1250);
      masses.push_back(1500);
      masses.push_back(1750);
      masses.push_back(2000);
      masses.push_back(2250);
      masses.push_back(2500);
      masses.push_back(2750);
      masses.push_back(3000);
    } else if (coupling=="01") { 
      masses.push_back(1500);
      masses.push_back(1750);
      masses.push_back(2000);
      masses.push_back(2250);
      masses.push_back(2500);
      masses.push_back(2750);
      masses.push_back(3000);
      masses.push_back(3250);
      masses.push_back(3500);
    }


  // make intrinsic width histograms and roodatahists using the wanted coupling
  cout << endl; 
  cout << endl; 
  cout << "--------------------------------------------------------------------------" << endl; 
  cout << endl; 
  string fileIntrinsic = "IntrinsicWidthHistos.root";
  cout << "Now prepare intrinsic width histograms and save in root file " << fileIntrinsic << endl;
  for (int ii=0; ii<(int)masses.size(); ii++) {
    int theMass = masses[ii];
    cout << "resolution at mass " << theMass << " with coupling " << coupling << endl;
    if (ii==0) MakeIntrinsicWidthHisto(fileIntrinsic, 1, theMass, coupling);    
    else MakeIntrinsicWidthHisto(fileIntrinsic, 0, theMass, coupling);
  }
  
  // make the interpolation of the resolutions and save in another file the roodatahists 
  cout << endl; 
  cout << endl; 
  cout << "--------------------------------------------------------------------------" << endl; 
  cout << endl; 
  cout << "Now make the interpolation" << endl; 
  WidthInterpolation(w, masses, coupling, 1); 

  return;
}

