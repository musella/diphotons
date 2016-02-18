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
static const Int_t genOnly = 1;

// Preparing the intrinsic width histogram
void MakeIntrinsicWidthHisto(TString filename, bool newFile, int mass, TString coupling) {

  TString myMass = TString::Format("%d",mass);
  Int_t ncat = NCAT;

  // the roorealvar 
  RooRealVar* deltaMgen;   // chiara
  if (coupling=="001") deltaMgen = new RooRealVar("deltaMgen", "",   -70, 70,   "GeV"); 
  if (coupling=="005") deltaMgen = new RooRealVar("deltaMgen", "",  -500, 500,  "GeV"); 
  if (coupling=="007") deltaMgen = new RooRealVar("deltaMgen", "",  -700, 700,  "GeV"); 
  if (coupling=="01")  deltaMgen = new RooRealVar("deltaMgen", "", -1000, 1000, "GeV");
  if (coupling=="015") deltaMgen = new RooRealVar("deltaMgen", "", -2000, 2000, "GeV");
  if (coupling=="02")  deltaMgen = new RooRealVar("deltaMgen", "", -2500, 2500, "GeV"); 
  if (coupling=="025") deltaMgen = new RooRealVar("deltaMgen", "", -3000, 3000, "GeV"); 
  if (coupling=="03")  deltaMgen = new RooRealVar("deltaMgen", "", -4000, 4000, "GeV"); 
  if (coupling=="035") deltaMgen = new RooRealVar("deltaMgen", "", -5000, 5000, "GeV"); 
  if (coupling=="04")  deltaMgen = new RooRealVar("deltaMgen", "", -6000, 6000, "GeV"); 

  // Output file 
  TFile *theIntrinsicWFile;
  if (newFile) theIntrinsicWFile = new TFile(filename,"RECREATE");
  else theIntrinsicWFile = new TFile(filename,"UPDATE");
  
  // Input file and tree
  TString inDir = "../macro/allFiles/";
  if (genOnly==1) inDir = "../macro/allFilesGenOnly/";
  TChain* sigTree = new TChain();
  cout << "reading file " 
       << inDir+TString(Form("FormSigMod_kpl"))+coupling+TString(Form("_M%d.root/DiPhotonTree", mass)) << endl;
  sigTree->Add(inDir+TString(Form("FormSigMod_kpl"))+coupling+TString(Form("_M%d.root/DiPhotonTree", mass)));
  sigTree->SetTitle("sigTree");
  sigTree->SetName("sigTree");

  // Minimal common preselection cut on mgg and mggGen
  TString mainCut = TString::Format("mgg>=0 && mgg<=12000 && mggGen>=0 && mggGen<=12000");   
  if (genOnly==1) mainCut = TString::Format("mggGen>=0 && mggGen<=12000");   

  // Loop over categories
  for (int c=0; c<ncat; ++c) {

    // Histos
    TH1D *intWidthH;       // chiara
    if (coupling=="001") intWidthH = new TH1D("intWidthH","intWidthH",  70,  -70.,70.);  
    if (coupling=="005") intWidthH = new TH1D("intWidthH","intWidthH", 500, -500.,500.);  
    if (coupling=="007") intWidthH = new TH1D("intWidthH","intWidthH", 700, -700.,700.);  
    if (coupling=="01")  intWidthH = new TH1D("intWidthH","intWidthH",1000,-1000.,1000.);  
    if (coupling=="015") intWidthH = new TH1D("intWidthH","intWidthH",2000,-2000.,2000.);  
    if (coupling=="02")  intWidthH = new TH1D("intWidthH","intWidthH",2500,-2500.,2500.);  
    if (coupling=="025") intWidthH = new TH1D("intWidthH","intWidthH",3000,-3000.,3000.);  
    if (coupling=="03")  intWidthH = new TH1D("intWidthH","intWidthH",4000,-4000.,4000.);  
    if (coupling=="035") intWidthH = new TH1D("intWidthH","intWidthH",5000,-5000.,5000.);  
    if (coupling=="04")  intWidthH = new TH1D("intWidthH","intWidthH",6000,-6000.,6000.);  
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
    if (coupling=="005") myPlot = deltaMgen->frame(Range( -500,500), Bins(500));       
    if (coupling=="007") myPlot = deltaMgen->frame(Range( -700,700), Bins(700));       
    if (coupling=="01")  myPlot = deltaMgen->frame(Range(-1000,1000),Bins(1000));    
    if (coupling=="015") myPlot = deltaMgen->frame(Range(-2000,2000),Bins(2000));    
    if (coupling=="02")  myPlot = deltaMgen->frame(Range(-2500,2500),Bins(2500)); 
    if (coupling=="025") myPlot = deltaMgen->frame(Range(-3000,3000),Bins(3000)); 
    if (coupling=="03")  myPlot = deltaMgen->frame(Range(-4000,4000),Bins(4000)); 
    if (coupling=="035") myPlot = deltaMgen->frame(Range(-5000,5000),Bins(5000)); 
    if (coupling=="04")  myPlot = deltaMgen->frame(Range(-6000,6000),Bins(6000)); 
    intWidthRDH.plotOn(myPlot, LineColor(kRed), LineStyle(kDashed));    
    myPlot->Draw();
    c1->SetLogy();
    TString canvasName = TString(Form("/tmp/crovelli/CheckIntrinsicWidth_Cat"))+TString(Form(myCut))+TString(Form("_mass"))+TString(Form(myMass))+TString(Form("kpl"))+TString(Form(coupling))+TString(Form("_LOG.png"));
    c1->SaveAs(canvasName);       
    RooPlot* myPlotZoom = deltaMgen->frame(Range(-300,300),Bins(300));  
    if (coupling=="001") myPlotZoom = deltaMgen->frame(Range(-12,12),Bins(12));
    if (coupling=="005") myPlotZoom = deltaMgen->frame(Range(-60,60),Bins(60));
    if (coupling=="007") myPlotZoom = deltaMgen->frame(Range(-100,100),Bins(100));
    if (coupling=="015") myPlotZoom = deltaMgen->frame(Range(-400,400),Bins(400));
    if (coupling=="02")  myPlotZoom = deltaMgen->frame(Range(-500,500),Bins(500));
    if (coupling=="025") myPlotZoom = deltaMgen->frame(Range(-700,700),Bins(700));
    if (coupling=="03")  myPlotZoom = deltaMgen->frame(Range(-900,900),Bins(900));
    if (coupling=="035") myPlotZoom = deltaMgen->frame(Range(-1200,1200),Bins(1200));
    if (coupling=="04")  myPlotZoom = deltaMgen->frame(Range(-1600,1600),Bins(1600));
    intWidthRDH.plotOn(myPlotZoom, LineColor(kRed), LineStyle(kDashed));
    myPlotZoom->Draw();
    canvasName = TString(Form("/tmp/crovelli/CheckIntrinsicWidthZoom_Cat"))+TString(Form(myCut))+TString(Form("_mass"))+TString(Form(myMass))+TString(Form("kpl"))+TString(Form(coupling))+TString(Form(".png"));
    c1->SetLogy(0);
    c1->SaveAs(canvasName);

    delete c1;
    delete intWidthH;
  }

  // Closing the output file
  theIntrinsicWFile->Close();
}

//-------------------------------------------------------
void WidthInterpolation(RooWorkspace* w, vector<int> masses, string coupling, bool newfile) {  
  
  // Variable for the intrinsic width
  RooRealVar* deltaMgen;   // chiara
  if (coupling=="001") deltaMgen = new RooRealVar("deltaMgen", "",   -70, 70,   "GeV"); 
  if (coupling=="005") deltaMgen = new RooRealVar("deltaMgen", "",  -500, 500,   "GeV"); 
  if (coupling=="007") deltaMgen = new RooRealVar("deltaMgen", "",  -700, 700,   "GeV"); 
  if (coupling=="01")  deltaMgen = new RooRealVar("deltaMgen", "", -1000, 1000, "GeV");
  if (coupling=="015") deltaMgen = new RooRealVar("deltaMgen", "", -2000, 2000,   "GeV"); 
  if (coupling=="02")  deltaMgen = new RooRealVar("deltaMgen", "", -2500, 2500, "GeV"); 
  if (coupling=="025") deltaMgen = new RooRealVar("deltaMgen", "", -3000, 3000, "GeV"); 
  if (coupling=="03")  deltaMgen = new RooRealVar("deltaMgen", "", -4000, 4000, "GeV"); 
  if (coupling=="035") deltaMgen = new RooRealVar("deltaMgen", "", -5000, 5000, "GeV"); 
  if (coupling=="04")  deltaMgen = new RooRealVar("deltaMgen", "", -6000, 6000, "GeV"); 
  RooArgList varlist;  
  varlist.add(*deltaMgen);   

  // Morphing variable
  RooRealVar* muWidth = new RooRealVar("muWidth", "", 0, 12000, "GeV");    

  // PDFs
  RooArgList pdfsCat0, pdfsCat1; 

  // Reference points 
  int numMass = (int)masses.size(); 
  TVectorD paramVec(numMass); 

  // To plot    
  RooPlot *frameCat0, *frameCat1;     // chiara
  if (coupling=="001") frameCat0 = deltaMgen->frame(Range(  -70,70), Bins(70));    
  if (coupling=="005") frameCat0 = deltaMgen->frame(Range( -500,500), Bins(500));       
  if (coupling=="007") frameCat0 = deltaMgen->frame(Range( -700,700), Bins(700));       
  if (coupling=="01")  frameCat0 = deltaMgen->frame(Range(-1000,1000),Bins(1000));    
  if (coupling=="015") frameCat0 = deltaMgen->frame(Range(-2000,2000),Bins(2000));    
  if (coupling=="02")  frameCat0 = deltaMgen->frame(Range(-2500,2500),Bins(2500)); 
  if (coupling=="025") frameCat0 = deltaMgen->frame(Range(-3000,3000),Bins(3000)); 
  if (coupling=="03")  frameCat0 = deltaMgen->frame(Range(-4000,4000),Bins(4000)); 
  if (coupling=="035") frameCat0 = deltaMgen->frame(Range(-5000,5000),Bins(5000)); 
  if (coupling=="04")  frameCat0 = deltaMgen->frame(Range(-6000,6000),Bins(6000)); 
  if (coupling=="001") frameCat1 = deltaMgen->frame(Range(  -70,70), Bins(70));    
  if (coupling=="005") frameCat1 = deltaMgen->frame(Range( -500,500), Bins(500));       
  if (coupling=="007") frameCat1 = deltaMgen->frame(Range( -700,700), Bins(700));       
  if (coupling=="01")  frameCat1 = deltaMgen->frame(Range(-1000,1000),Bins(1000));    
  if (coupling=="015") frameCat1 = deltaMgen->frame(Range(-2000,2000),Bins(2000));    
  if (coupling=="02")  frameCat1 = deltaMgen->frame(Range(-2500,2500),Bins(2500));    
  if (coupling=="025") frameCat1 = deltaMgen->frame(Range(-3000,3000),Bins(3000));    
  if (coupling=="03")  frameCat1 = deltaMgen->frame(Range(-4000,4000),Bins(4000));    
  if (coupling=="035") frameCat1 = deltaMgen->frame(Range(-5000,5000),Bins(5000));    
  if (coupling=="04")  frameCat1 = deltaMgen->frame(Range(-6000,6000),Bins(6000));    
  
  // Files with the roodatahists
  TFile *fileWidth = new TFile("/tmp/crovelli/IntrinsicWidthHistos.root"); 

  // Loop over masses 
  for (int ii=0; ii<(int)masses.size(); ii++) { 
    int theMass = masses[ii];   
    TString myMass = TString::Format("%d",theMass); 

    // Reference points 
    paramVec[ii] = (double)theMass;  

    for (int c=0; c<NCAT; ++c) {  

      cout << "---------- Category = " << c << ", mass = " << myMass << endl; 
      TString myCut = "EBEB";
      if (c==1) myCut = "EBEE"; 
      
      // reading the roodatahist from the file  
      TString myRDH = TString(Form("intWidthRDH_mass%d",theMass))+TString(Form("_cat"))+myCut+TString(Form("_kpl"))+coupling;
      RooDataHist *widthRDH = (RooDataHist*)fileWidth->Get(myRDH);    
      widthRDH->Print();   
      cout << "Just got the width RooDataHist " << myRDH << endl;     
      cout << endl;         

      // moving to the corresponding roohistpdf  
      TString myRHpdfName = TString(Form("intWidthRHPDF_mass%d",theMass))+TString(Form("_cat"))+myCut+TString(Form("_kpl"))+coupling;
      RooHistPdf *myHistPdfWidth = new RooHistPdf("myHistPdfWidth","myHistPdfWidth",*deltaMgen,*widthRDH,0) ;  
      myHistPdfWidth->SetTitle(myRHpdfName);      
      myHistPdfWidth->SetName(myRHpdfName);            
      myHistPdfWidth->Print();   
      cout << "Just got the width RooHistPdf " << myRHpdfName << endl;    
      cout << endl;   

      // adding to the list of pdfs    
      if (c==0) pdfsCat0.add(*myHistPdfWidth); 
      if (c==1) pdfsCat1.add(*myHistPdfWidth); 
      cout << "RooHistPdfs added to the pdf list" << endl;  
      cout << endl;  

      // plot to check  
      if (c==0) myHistPdfWidth->plotOn(frameCat0,LineColor(kBlue), LineStyle(kSolid));     
      if (c==1) myHistPdfWidth->plotOn(frameCat1,LineColor(kBlue), LineStyle(kSolid)); 

    } // loop over cat
  }   // loop over masses

  // Now morphing for the two categories    
  cout << endl;   
  cout << "morphing for the intrinsic width functions" << endl; 
  pdfsCat0.Print();  
  pdfsCat1.Print(); 
  RooMomentMorph *morphWidthCat0 = new RooMomentMorph("morphWidthCat0","morphWidthCat0",*muWidth,varlist,pdfsCat0,paramVec,RooMomentMorph::Linear);
  morphWidthCat0->Print();      
  RooMomentMorph *morphWidthCat1 = new RooMomentMorph("morphWidthCat1","morphWidthCat1",*muWidth,varlist,pdfsCat1,paramVec,RooMomentMorph::Linear);
  morphWidthCat1->Print();      
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
    for (int c=0; c<NCAT; ++c) {  
      TString myCut = "EBEB";  
      if (c==1) myCut = "EBEE";  
      RooDataHist *fittWidthRDH; 
      if (coupling=="001") deltaMgen->setBins(70);
      if (coupling=="005") deltaMgen->setBins(500);
      if (coupling=="007") deltaMgen->setBins(700);
      if (coupling=="01")  deltaMgen->setBins(1000);
      if (coupling=="015") deltaMgen->setBins(2000);
      if (coupling=="02")  deltaMgen->setBins(2500);
      if (coupling=="025") deltaMgen->setBins(3000);
      if (coupling=="03")  deltaMgen->setBins(4000);
      if (coupling=="035") deltaMgen->setBins(5000);
      if (coupling=="04")  deltaMgen->setBins(6000);
      if(c==0) fittWidthRDH = morphWidthCat0->generateBinned(*deltaMgen,10000,kTRUE);   
      if(c==1) fittWidthRDH = morphWidthCat1->generateBinned(*deltaMgen,10000,kTRUE);   
      fittWidthRDH->Print();
      TString myFitRDHa = TString::Format("widthRDH_mass%d_cat",thisMass)+myCut;    
      TString myFitRDH  = myFitRDHa+"_kpl"+coupling;
      fittWidthRDH->SetTitle(myFitRDH);  
      fittWidthRDH->SetName(myFitRDH);  
      w->import(*fittWidthRDH);
    }
  }

  /*
  // Then 4GeV steps between 1000 and 1600 GeV    
  for (int iGenMass=0; iGenMass<150; iGenMass++) { 
    int thisMass = 1000 + iGenMass*4.;  
    cout << "Medium (4GeV) scan: " << thisMass << endl;  
    muWidth->setVal(thisMass);
    for (int c=0; c<NCAT; ++c) {  
      TString myCut = "EBEB";  
      if (c==1) myCut = "EBEE";  
      RooDataHist *fittWidthRDH; 
      if (coupling=="001") deltaMgen->setBins(70);
      if (coupling=="005") deltaMgen->setBins(500);
      if (coupling=="007") deltaMgen->setBins(700);
      if (coupling=="01")  deltaMgen->setBins(1000);
      if (coupling=="015") deltaMgen->setBins(2000);
      if (coupling=="02")  deltaMgen->setBins(2500);
      if (coupling=="025") deltaMgen->setBins(3000);
      if (coupling=="03")  deltaMgen->setBins(4000);
      if (coupling=="035") deltaMgen->setBins(5000);
      if (coupling=="04")  deltaMgen->setBins(6000);
      if(c==0) fittWidthRDH = morphWidthCat0->generateBinned(*deltaMgen,10000,kTRUE);   
      if(c==1) fittWidthRDH = morphWidthCat1->generateBinned(*deltaMgen,10000,kTRUE);   
      fittWidthRDH->Print();
      TString myFitRDHa = TString::Format("widthRDH_mass%d_cat",thisMass)+myCut;    
      TString myFitRDH  = myFitRDHa+"_kpl"+coupling;
      fittWidthRDH->SetTitle(myFitRDH);  
      fittWidthRDH->SetName(myFitRDH);  
      w->import(*fittWidthRDH);
    }
  }
  // Finally 10GeV steps between 1600 and 5000 GeV    
  for (int iGenMass=0; iGenMass<34; iGenMass++) {  
    int thisMass = 1600 + iGenMass*100.;   
    cout << "Coarse (100GeV) scan: " << thisMass << endl; 
    muWidth->setVal(thisMass);
    for (int c=0; c<NCAT; ++c) {  
      TString myCut = "EBEB";  
      if (c==1) myCut = "EBEE";  
      RooDataHist *fittWidthRDH; 
      if (coupling=="001") deltaMgen->setBins(70);
      if (coupling=="005") deltaMgen->setBins(500);
      if (coupling=="007") deltaMgen->setBins(700);
      if (coupling=="01")  deltaMgen->setBins(1000);
      if (coupling=="015") deltaMgen->setBins(2000);
      if (coupling=="02")  deltaMgen->setBins(2500);
      if (coupling=="025") deltaMgen->setBins(3000);
      if (coupling=="03")  deltaMgen->setBins(4000);
      if (coupling=="035") deltaMgen->setBins(5000);
      if (coupling=="04")  deltaMgen->setBins(6000);
      if(c==0) fittWidthRDH = morphWidthCat0->generateBinned(*deltaMgen,10000,kTRUE);   
      if(c==1) fittWidthRDH = morphWidthCat1->generateBinned(*deltaMgen,10000,kTRUE);   
      fittWidthRDH->Print();
      TString myFitRDHa = TString::Format("widthRDH_mass%d_cat",thisMass)+myCut;    
      TString myFitRDH  = myFitRDHa+"_kpl"+coupling;
      fittWidthRDH->SetTitle(myFitRDH);  
      fittWidthRDH->SetName(myFitRDH);  
      w->import(*fittWidthRDH);
    }
  }
  */

  // Finally saving in a second rootfile 
  cout << endl;
  cout << endl;
  cout << "----------------------------" << endl;  
  cout << endl;
  cout << "Now salving the histos in a root file" << endl;  
  TFile *fileFittoWidth;
  if (newfile) fileFittoWidth = new TFile("/tmp/crovelli/WidthHistosGenOnlyScan.root","RECREATE");  
  else fileFittoWidth = new TFile("/tmp/crovelli/WidthHistosGenOnlyScan.root","UPDATE");  
  fileFittoWidth->cd();

  for (int iGenMass=0; iGenMass<250; iGenMass++) {   
    int thisMass = 500 + iGenMass*2;  
    for (int c=0; c<NCAT; ++c) {    
      TString myCut = "EBEB";  
      if (c==1) myCut = "EBEE";  
      TString myFitRDHa = TString::Format("widthRDH_mass%d_cat",thisMass)+myCut;    
      TString myFitRDH  = myFitRDHa+TString::Format("_kpl")+coupling;
      RooDataHist *RDH = (RooDataHist*)w->data(myFitRDH);   
      RDH->Write();     
    }
  }

  /*
  for (int iGenMass=0; iGenMass<150; iGenMass++) {   
    int thisMass = 1000 + iGenMass*4;  
    for (int c=0; c<NCAT; ++c) {    
      TString myCut = "EBEB";  
      if (c==1) myCut = "EBEE";  
      TString myFitRDHa = TString::Format("widthRDH_mass%d_cat",thisMass)+myCut;    
      TString myFitRDH  = myFitRDHa+TString::Format("_kpl")+coupling;
      RooDataHist *RDH = (RooDataHist*)w->data(myFitRDH);   
      RDH->Write();     
    }
  }
  for (int iGenMass=0; iGenMass<34; iGenMass++) {   
    int thisMass = 1600 + iGenMass*100;  
    for (int c=0; c<NCAT; ++c) {    
      TString myCut = "EBEB";  
      if (c==1) myCut = "EBEE";  
      TString myFitRDHa = TString::Format("widthRDH_mass%d_cat",thisMass)+myCut;    
      TString myFitRDH  = myFitRDHa+TString::Format("_kpl")+coupling;
      RooDataHist *RDH = (RooDataHist*)w->data(myFitRDH);   
      RDH->Write();     
    }
  }
  */

  fileFittoWidth->Close();
}

// To run the analysis
void runfits(string coupling="01") {

  RooWorkspace *w = new RooWorkspace("w");    

  cout << "running for coupling " << coupling << endl;

  // range of masses - to be used to make the convolution
  vector<int> masses;
  if (genOnly==0) {              // full sim samples
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
  } else {                    // gen only fine scan

    for (int iGenMass=0; iGenMass<37; iGenMass++) {
      int thisMass = 500 + iGenMass*125;
      
      // missing masses
      if (thisMass==3125) continue;
      if (thisMass==3250) continue;
      if (thisMass==3375) continue;
      if (coupling=="01"  && thisMass==1875) continue;
      if (coupling=="001" && thisMass==3500) continue;
      if (coupling=="001" && thisMass==4375) continue;
      if (coupling=="001" && thisMass==4875) continue;
      if (coupling=="02"  && thisMass==1500) continue;
      if (coupling=="02"  && thisMass==3625) continue;
      if (coupling=="02"  && thisMass==4500) continue;
      if (coupling=="02"  && thisMass==4750) continue;
      if (coupling=="005" && thisMass==5000) continue;
      if (coupling=="007" && thisMass==2000) continue;
      if (coupling=="007" && thisMass==5000) continue;
      if (coupling=="015" && thisMass==4375) continue;
      if (coupling=="035" && thisMass==2875) continue;
      if (coupling=="04"  && thisMass==4375) continue;

      cout << "adding mass " << thisMass << endl;
      masses.push_back(thisMass);
    }
  }

  // make intrinsic width histograms and roodatahists using the wanted coupling
  cout << endl; 
  cout << endl; 
  cout << "--------------------------------------------------------------------------" << endl; 
  cout << endl; 
  string fileIntrinsic = "/tmp/crovelli/IntrinsicWidthHistos.root";
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

