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
void MakeIntrinsicWidthHisto(TString filename, bool newFile, int mass, string couplingS) {

  TString myMass = TString::Format("%d",mass);
  Int_t ncat = NCAT;

  // the roorealvar  - chiara
  RooRealVar* deltaMgen = new RooRealVar("deltaMgen", "", -4000., 4000., "GeV");    

  // Output file 
  TFile *theIntrinsicWFile;
  if (newFile) theIntrinsicWFile = new TFile(filename,"RECREATE");
  else theIntrinsicWFile = new TFile(filename,"UPDATE");
  
  // Input file and tree
  TString inDir = "../macro/allFilesGenOnly/scanWidth/";
  TChain* sigTree = new TChain();

  cout << "reading file " 
       << inDir+"FormSigMod_kpl"+TString(couplingS)+TString(Form("_M%d.root/DiPhotonTree", mass)) << endl;
  sigTree->Add(  inDir+"FormSigMod_kpl"+TString(couplingS)+TString(Form("_M%d.root/DiPhotonTree", mass)) );
  sigTree->SetTitle("sigTree");
  sigTree->SetName("sigTree");

  // Minimal common preselection cut on mgg and mggGen
  TString mainCut = TString::Format("mggGen>=0 && mggGen<=12000");   

  // Loop over categories
  for (int c=0; c<ncat; ++c) {

    // Histos - chiara
    TH1D *intWidthH = new TH1D("intWidthH","intWidthH",40000,-4000.,4000.);
    if (couplingS=="001") intWidthH = new TH1D("intWidthH","intWidthH",40000,-4000.,4000.);
    else if (couplingS=="005") intWidthH = new TH1D("intWidthH","intWidthH",40000,-4000.,4000.);
    else if (couplingS=="007") intWidthH = new TH1D("intWidthH","intWidthH",16000,-4000.,4000.);
    else if (couplingS=="01")  intWidthH = new TH1D("intWidthH","intWidthH",8000,-4000.,4000.);
    else if (couplingS=="015") intWidthH = new TH1D("intWidthH","intWidthH",8000,-4000.,4000.);
    else if (couplingS=="02")  intWidthH = new TH1D("intWidthH","intWidthH",4000,-4000.,4000.);
    else if (couplingS=="025") intWidthH = new TH1D("intWidthH","intWidthH",4000,-4000.,4000.);
    else if (couplingS=="03")  intWidthH = new TH1D("intWidthH","intWidthH",4000,-4000.,4000.);
    else cout << "PROBLEM!" << endl;
    intWidthH->Sumw2();
  
    // Projecting the tree
    TString express = TString::Format("mggGen-%d",mass);
    if (c==0) sigTree->Project("intWidthH",express,mainCut+TString::Format("&& eventClass==0"));
    if (c==1) sigTree->Project("intWidthH",express,mainCut+TString::Format("&& eventClass==1"));

    // Now make the roodatahist
    RooDataHist intWidthRDH("intWidthRDH","intWidthRDH",*deltaMgen,Import(*intWidthH));   
    cout << "Intrinsic width datahist summary for mass " << mass << " and coupling " << couplingS << endl;  
    intWidthRDH.Print();
    cout << endl;  

    // Saving in the root file
    theIntrinsicWFile->cd();
    TString myCut;
    if (c==0)      myCut = "EBEB";  
    else if (c==1) myCut = "EBEE";
    TString nameRDH = TString::Format("intWidthRDH_mass%d",mass)+TString::Format("_cat")+myCut+"_kpl"+couplingS;
    intWidthRDH.Write(nameRDH);

    delete intWidthH;
  }

  // Closing the output file
  theIntrinsicWFile->Close();
}

//-------------------------------------------------------
void WidthInterpolation(RooWorkspace* w, int mass, vector<string> couplingsS, bool newfile) {  
  
  // Variable for the intrinsic width - chiara
  RooRealVar* deltaMgen = new RooRealVar("deltaMgen", "", -4000., 4000., "GeV");      
  RooArgList varlist;  
  varlist.add(*deltaMgen);   

  // Morphing variable
  RooRealVar* muWidth = new RooRealVar("muWidth", "", 0, 0.5, "");   

  // PDFs
  RooArgList pdfsCat0, pdfsCat1; 

  // Reference points 
  int numCouplings = (int)couplingsS.size(); 
  TVectorD paramVec(numCouplings); 

  // Files with the roodatahists
  TFile *fileWidth = new TFile("/tmp/crovelli/IntrinsicWidthHistos.root"); 

  // Loop over couplings for the wanted mass
  for (int ii=0; ii<(int)couplingsS.size(); ii++) { 

    TString myCoupling = couplingsS[ii];
    float theCoupling=0.01;
    if (couplingsS[ii]=="001") theCoupling = 0.01;   
    if (couplingsS[ii]=="005") theCoupling = 0.05;   
    if (couplingsS[ii]=="007") theCoupling = 0.07;   
    if (couplingsS[ii]=="01")  theCoupling = 0.1;   
    if (couplingsS[ii]=="015") theCoupling = 0.15;   
    if (couplingsS[ii]=="02")  theCoupling = 0.2;   
    if (couplingsS[ii]=="025") theCoupling = 0.25;   
    if (couplingsS[ii]=="03")  theCoupling = 0.3;   

    // Reference points 
    paramVec[ii] = (double)theCoupling;  

    for (int c=0; c<NCAT; ++c) {  
      
      cout << "---------- Category = " << c << ", mass = " << mass << ", coupling = " << theCoupling << endl; 
      TString myCut = "EBEB";
      if (c==1) myCut = "EBEE"; 
      
      // reading the roodatahist from the file  
      TString myRDH = TString(Form("intWidthRDH_mass%d",mass))+TString(Form("_cat"))+myCut+"_kpl"+myCoupling;
      cout << myRDH << endl;
      RooDataHist *widthRDH = (RooDataHist*)fileWidth->Get(myRDH);    
      widthRDH->Print();   
      cout << "Just got the width RooDataHist " << myRDH << endl;     
      cout << endl;         

      // moving to the corresponding roohistpdf  
      TString myRHpdfName = TString(Form("intWidthRHPDF_mass%d",mass))+TString(Form("_cat"))+myCut+"_kpl"+myCoupling;
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

    } // loop over cat
  }   // loop over couplings

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

  // Now we evaluate with a fine coupling scan samples and save the corresponding roodatahists  
  cout << endl;
  cout << endl;
  cout << "----------------------------" << endl;  
  cout << endl;
  cout << "now evaluating the morphing every 0.01, from 0.01 to 0.3: doing mass " << endl; 

  for (int iCoupl=0; iCoupl<30; iCoupl++) {    
    float thisCoupl = 0.01 + iCoupl*0.01;        
    cout << "Fine scan: " << thisCoupl << endl;  
    muWidth->setVal(thisCoupl);          
    for (int c=0; c<NCAT; ++c) {  
      TString myCut = "EBEB";  
      if (c==1) myCut = "EBEE";  
      RooDataHist *fittWidthRDH;                      
      // chiara
      if (thisCoupl>=0 && thisCoupl<0.07)    deltaMgen->setBins(40000);
      if (thisCoupl>=0.07 && thisCoupl<0.09) deltaMgen->setBins(16000);
      if (thisCoupl>=0.10 && thisCoupl<0.20) deltaMgen->setBins(8000);
      if (thisCoupl>=0.20 && thisCoupl<0.31) deltaMgen->setBins(4000);
      if(c==0) fittWidthRDH = morphWidthCat0->generateBinned(*deltaMgen,10000,kTRUE);   
      if(c==1) fittWidthRDH = morphWidthCat1->generateBinned(*deltaMgen,10000,kTRUE);   
      fittWidthRDH->Print();
      TString myFitRDHa = TString::Format("widthRDH_mass%d_cat",mass)+myCut;    
      TString myFitRDH  = myFitRDHa+TString::Format("_kpl%f",thisCoupl);
      fittWidthRDH->SetTitle(myFitRDH);  
      fittWidthRDH->SetName(myFitRDH);  
      w->import(*fittWidthRDH);
    }
  }

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

  for (int iCoupl=0; iCoupl<30; iCoupl++) {    
    float thisCoupl = 0.01 + iCoupl*0.01;        
    for (int c=0; c<NCAT; ++c) {    
      TString myCut = "EBEB";  
      if (c==1) myCut = "EBEE";  
      TString myFitRDHa = TString::Format("widthRDH_mass%d_cat",mass)+myCut;    
      TString myFitRDH  = myFitRDHa+TString::Format("_kpl%f",thisCoupl);
      RooDataHist *RDH = (RooDataHist*)w->data(myFitRDH);   
      RDH->Write();     
    }
  }

  fileFittoWidth->Close();
}

// To run the analysis
void runfits(int mass=750) {

  RooWorkspace *w = new RooWorkspace("w");    
  
  cout << "running for mass " << mass << endl;

  // range of couplings - to be used to make the convolution
  vector<string> couplingsS;
  couplingsS.push_back("001");
  couplingsS.push_back("005");
  couplingsS.push_back("007");
  couplingsS.push_back("01");
  couplingsS.push_back("015");
  couplingsS.push_back("02");
  couplingsS.push_back("025");
  couplingsS.push_back("03");

  // make intrinsic width histograms and roodatahists using the wanted coupling
  cout << endl; 
  cout << endl; 
  cout << "--------------------------------------------------------------------------" << endl; 
  cout << endl; 
  string fileIntrinsic = "/tmp/crovelli/IntrinsicWidthHistos.root";
  cout << "Now prepare intrinsic width histograms and save in root file " << fileIntrinsic << endl;
  for (int ii=0; ii<(int)couplingsS.size(); ii++) {
    string theCouplingS = couplingsS[ii];
    cout << "Width at coupling " << theCouplingS << " with mass " << mass << endl;
    if (ii==0) MakeIntrinsicWidthHisto(fileIntrinsic, 1, mass, theCouplingS);    
    else MakeIntrinsicWidthHisto(fileIntrinsic, 0, mass, theCouplingS);
  }
  
  // make the interpolation save in another file the roodatahists 
  cout << endl; 
  cout << endl; 
  cout << "--------------------------------------------------------------------------" << endl; 
  cout << endl; 
  cout << "Now make the interpolation" << endl; 
  WidthInterpolation(w, mass, couplingsS, 1); 

  return;
}


//  LocalWords:  chiara
