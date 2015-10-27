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

// Preparing the intrinsic width histogram
void MakeIntrinsicWidthHisto(TString filename, bool newFile, int mass, TString coupling) {

  TString myMass = TString::Format("%d",mass);
  Int_t ncat = NCAT;

  // the roorealvar 
  RooRealVar* deltaMgen;   // chiara
  if (coupling=="001") deltaMgen = new RooRealVar("deltaMgen", "",   -70, 70,   "GeV"); 
  if (coupling=="01")  deltaMgen = new RooRealVar("deltaMgen", "", -1000, 1000, "GeV");
  if (coupling=="02")  deltaMgen = new RooRealVar("deltaMgen", "", -2500, 2500, "GeV"); 

  // Output file 
  TFile *theIntrinsicWFile;
  if (newFile) theIntrinsicWFile = new TFile(filename,"RECREATE");
  else theIntrinsicWFile = new TFile(filename,"UPDATE");
  
  // Input file and tree
  TString inDir = "../macro/allFiles/";
  TChain* sigTree = new TChain();
  cout << "reading file " 
       << inDir+TString(Form("FormSigMod_kpl"))+coupling+TString(Form("_M%d.root/DiPhotonTree", mass)) << endl;
  sigTree->Add(inDir+TString(Form("FormSigMod_kpl"))+coupling+TString(Form("_M%d.root/DiPhotonTree", mass)));
  sigTree->SetTitle("sigTree");
  sigTree->SetName("sigTree");

  // Minimal common preselection cut on mgg and mggGen
  TString mainCut = TString::Format("mgg>=0 && mgg<=12000 && mggGen>=0 && mggGen<=12000");   

  // Loop over categories
  for (int c=0; c<ncat; ++c) {

    // Histos
    TH1D *intWidthH;       // chiara
    if (coupling=="001") intWidthH = new TH1D("intWidthH","intWidthH",  70,  -70.,70.);  
    if (coupling=="01")  intWidthH = new TH1D("intWidthH","intWidthH",1000,-1000.,1000.);  
    if (coupling=="02")  intWidthH = new TH1D("intWidthH","intWidthH",2500,-2500.,2500.);  
    intWidthH->Sumw2();
  
    // Projecting the tree
    TString express = TString::Format("mggGen-%d",mass);
    if (c==0) sigTree->Project("intWidthH",express,mainCut+TString::Format("&& eventClass==0"));
    if (c==1) sigTree->Project("intWidthH",express,mainCut+TString::Format("&& eventClass==1"));

    // Now make the roodatahist
    RooDataHist intWidthRDH("intWidthRDH","intWidthRDH",*deltaMgen,Import(*intWidthH));   
    cout << "Intrinsic width datahist summary for mass " << mass << endl;  
    intWidthRDH.Print();
    cout << endl;  

    // Saving in the root file
    theIntrinsicWFile->cd();
    TString myCut;
    if (c==0)      myCut = "EBEB";  
    else if (c==1) myCut = "EBEE";
    TString nameRDH = TString::Format("intWidthRDH_mass%d",mass)+TString::Format("_cat")+myCut+TString::Format("_kpl")+coupling;
    intWidthRDH.Write(nameRDH);

    // Plot to check
    TCanvas *c1 = new TCanvas("c1","c1",1);
    RooPlot* myPlot; //chiara
    if (coupling=="001") myPlot = deltaMgen->frame(Range(  -70,70), Bins(70));    
    if (coupling=="01")  myPlot = deltaMgen->frame(Range(-1000,1000),Bins(1000));    
    if (coupling=="02")  myPlot = deltaMgen->frame(Range(-2500,2500),Bins(2500));    
    intWidthRDH.plotOn(myPlot, LineColor(kRed), LineStyle(kDashed));    
    myPlot->Draw();
    c1->SetLogy();
    TString canvasName = TString(Form("CheckIntrinsicWidth_Cat"))+TString(Form(myCut))+TString(Form("_mass"))+TString(Form(myMass))+TString(Form("kpl"))+TString(Form(coupling))+TString(Form("_LOG.png"));
    c1->SaveAs(canvasName);       
    RooPlot* myPlotZoom = deltaMgen->frame(Range(-300,300),Bins(300));  
    if (coupling=="001") myPlotZoom = deltaMgen->frame(Range(-12,12),Bins(12));
    if (coupling=="02")  myPlotZoom = deltaMgen->frame(Range(-500,500),Bins(500));
    intWidthRDH.plotOn(myPlotZoom, LineColor(kRed), LineStyle(kDashed));
    myPlotZoom->Draw();
    canvasName = TString(Form("CheckIntrinsicWidthZoom_Cat"))+TString(Form(myCut))+TString(Form("_mass"))+TString(Form(myMass))+TString(Form("kpl"))+TString(Form(coupling))+TString(Form(".png"));
    c1->SetLogy(0);
    c1->SaveAs(canvasName);

    delete c1;
    delete intWidthH;
  }

  // Closing the output file
  theIntrinsicWFile->Close();
}

// To run the analysis
void runfits() {

  for (int iCoupling=0; iCoupling<3; iCoupling++) {

    string coupling;
    if (iCoupling==0) coupling="001";
    if (iCoupling==1) coupling="01";
    if (iCoupling==2) coupling="02";
    cout << "running for coupling " << iCoupling << ", " << coupling << endl;

    // range of masses - to be used to make the convolution
    vector<int> masses;
    if (coupling=="01") {
      masses.push_back(500);
      masses.push_back(750);
      masses.push_back(1000);
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
      //masses.push_back(5000);
      ////masses.push_back(5500);
      ////masses.push_back(6000);
      ////masses.push_back(7000);
    } else if (coupling=="001") {
      masses.push_back(500);
      masses.push_back(750);
      masses.push_back(1000);
      // masses.push_back(1500);
      masses.push_back(2000);
      // masses.push_back(3000);
      masses.push_back(4000);
      masses.push_back(5000);
      ////masses.push_back(6000);
      ////masses.push_back(7000);
    } else if (coupling=="02") {  
      masses.push_back(500);
      masses.push_back(750);
      masses.push_back(1000);
      masses.push_back(1500);
      masses.push_back(2000);
      masses.push_back(3000);
      masses.push_back(4000);
      masses.push_back(5000);
      ////// masses.push_back(6000);
      ////masses.push_back(7000);
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
      if (ii==0 && iCoupling==0) MakeIntrinsicWidthHisto(fileIntrinsic, 1, theMass, coupling);    
      else MakeIntrinsicWidthHisto(fileIntrinsic, 0, theMass, coupling);
    }

  } // loop over couplings

  return;
}

