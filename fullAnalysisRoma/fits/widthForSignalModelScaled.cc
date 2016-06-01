#include "RooArgList.h"
#include "RooRealVar.h"
#include "RooAbsPdf.h"
#include "RooBinning.h"
#include "RooDataSet.h"
#include "RooDataHist.h"
#include "RooWorkspace.h"
#include "RooPlot.h"
#include "RooHistPdf.h"
#include "RooMomentMorph.h"
#include "RooFitResult.h"     
#include "RooCustomizer.h"
#include "RooProduct.h"
#include "RooLinearVar.h"
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

  // Roorealvar: deltaM/gamma - chiara
  RooRealVar* redMass   = new RooRealVar("redMass", "", -4., 4., "");

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

    // Histos - chiara
    TH1D *intWidthH = new TH1D("intWidthH","intWidthH", 21, -4.1, 4.1);             // chiara: cosi' ho bin da ~0.4 in deltaM/Gamma
  
    // Projecting the tree
    TString express;   // deltaM/Gamma
    if(coupling=="001") express = TString::Format("(mggGen-%d)/(2.5*1.4*0.01*0.01*%d)",mass,mass);   // chiara: 2.5 aggiuntivo!
    if(coupling=="01")  express = TString::Format("(mggGen-%d)/(1.4*0.1*0.1*%d)",mass,mass);
    if(coupling=="02")  express = TString::Format("(mggGen-%d)/(1.4*0.2*0.2*%d)",mass,mass);
    cout << "chiara, debug: variable is " << express << endl;
    if (c==0) sigTree->Project("intWidthH",express,mainCut+TString::Format("&& eventClass==0"));
    if (c==1) sigTree->Project("intWidthH",express,mainCut+TString::Format("&& eventClass==1"));
    cout << "Overflow: " << intWidthH->GetBinContent(22) << ", underflow: " << intWidthH->GetBinContent(0)  << endl;   // chiara
    cout << "Overflow + underflow = " << (float(intWidthH->GetBinContent(22))+float(intWidthH->GetBinContent(0)))/(float)intWidthH->GetEntries() << endl;     // chiara

    // Now make the roodatahist
    RooDataHist intWidthRDHa("intWidthRDHa","intWidthRDHa",*redMass,intWidthH);     
    cout << "Intrinsic width datahist summary for mass " << mass << ", large binning" << endl;  
    intWidthRDHa.Print();
    cout << endl;  

    // Now generate from this roodatahist, with more bins
    RooHistPdf *intWidthRHPDFa = new RooHistPdf("intWidthRHPDFa","intWidthRHPDFa",*redMass,intWidthRDHa,2);   
    cout << "Converting in RooHistPdf to change the binning" << endl;
    intWidthRHPDFa->Print();
    cout << endl;  
    redMass->setBins(81);
    RooDataHist *intWidthRDH = intWidthRHPDFa->generateBinned(*redMass,10000,kTRUE);   
    intWidthRDH->SetTitle("intWidthRDH");
    intWidthRDH->SetName("intWidthRDH");
    cout << "Intrinsic width new datahist with more bins" << endl;
    intWidthRDH->Print();
    cout << endl;  

    // Saving in the root file
    theIntrinsicWFile->cd();
    TString myCut;
    if (c==0)      myCut = "EBEB";  
    else if (c==1) myCut = "EBEE";
    TString nameRDH = TString::Format("intWidthRDH_mass%d",mass)+TString::Format("_cat")+myCut+TString::Format("_kpl")+coupling;
    intWidthRDH->Write(nameRDH);

    // Plot to check
    TCanvas *c1 = new TCanvas("c1","c1",1);
    RooPlot* myPlot = redMass->frame(); 
    intWidthRDH->plotOn(myPlot, LineColor(kRed), LineStyle(kDashed), Rescale(1./intWidthRDH->sumEntries()));    
    myPlot->Draw();
    TString canvasName = TString(Form("/tmp/crovelli/CheckIntrinsicWidth_Cat"))+TString(Form(myCut))+TString(Form("_mass"))+TString(Form(myMass))+TString(Form("kpl"))+TString(Form(coupling))+TString(Form(".png"));
    myPlot->SetMaximum(0.1);
    c1->SaveAs(canvasName);       

    delete c1;
    delete intWidthRDH;
    delete intWidthRHPDFa;
    delete intWidthH;
  }

  // Closing the output file
  theIntrinsicWFile->Close();
}

//-------------------------------------------------------
void WidthInterpolation(RooWorkspace* w, vector<int> masses, string coupling, bool newfile) {  

  // RooRealVars
  RooRealVar* deltaMgen;                 // mgen-mX     chiara  
  if (coupling=="001") deltaMgen = new RooRealVar("deltaMgen", "",   -5.,  5.,    "GeV");     // asking for deltaMgen/gamma<4 => it is 2.8 at 5TeV
  if (coupling=="01")  deltaMgen = new RooRealVar("deltaMgen", "",  -325., 325.,  "GeV");     // asking for deltaMgen/gamma<4 => it is 280 at 5TeV
  if (coupling=="02")  deltaMgen = new RooRealVar("deltaMgen", "", -1205., 1205., "GeV");     // asking for deltaMgen/gamma<4 => it is 1120 at 5TeV

  cout << "chiara: deltaMgen" << endl;
  deltaMgen->Print();
  cout << endl;

  // The RooRealVar used for to fill morphing
  RooRealVar* redMass = new RooRealVar("redMass", "", -4., 4., "");   // chiara
  cout << "chiara: redMass" << endl;
  redMass->Print();
  cout << endl;

  RooArgList varlist;  
  varlist.add(*redMass);   
  cout << "chiara: varlist" << endl;
  varlist.Print();
  cout << endl;

  // The interpolation variable
  RooRealVar* muWidth = new RooRealVar("muWidth", "", 0, 12000, "GeV");    
  cout << "chiara: muWidth" << endl;
  muWidth->Print();
  cout << endl;

  // PDFs 
  RooArgList pdfsCat0, pdfsCat1; 

  // Reference points 
  int numMass = (int)masses.size(); 
  TVectorD paramVec(numMass); 

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
      RooHistPdf *myHistPdfWidth = new RooHistPdf("myHistPdfWidth","myHistPdfWidth",*redMass,*widthRDH,0) ;  
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
  }   // loop over masses

  // Now morphing for the two categories    
  cout << endl;   
  cout << "morphing for the intrinsic width functions" << endl; 
  cout << "CatEBEB" << endl;
  pdfsCat0.Print();  
  cout << "CatEBEE" << endl;
  pdfsCat1.Print(); 

  cout << "morphing, catEBEB" << endl;
  RooMomentMorph *morphWidthCat0 = new RooMomentMorph("morphWidthCat0","morphWidthCat0",*muWidth,varlist,pdfsCat0,paramVec,RooMomentMorph::Linear);
  morphWidthCat0->useHorizontalMorphing(false);
  morphWidthCat0->Print();      
  cout << endl;
  cout << "morphing, catEBEE" << endl;
  RooMomentMorph *morphWidthCat1 = new RooMomentMorph("morphWidthCat1","morphWidthCat1",*muWidth,varlist,pdfsCat1,paramVec,RooMomentMorph::Linear);
  morphWidthCat1->useHorizontalMorphing(false);
  morphWidthCat1->Print();      
  cout << endl;
  cout << endl;


  // Plot to check
  cout << "NOW TEST mc vs fit: EBEB" << endl;
  TString myRDHtestEBEBs = TString(Form("intWidthRDH_mass4000_catEBEB_kpl"))+coupling;
  RooDataHist *myRDHtestEBEB = (RooDataHist*)fileWidth->Get(myRDHtestEBEBs);
  cout << "RDH EBEB" << endl;
  myRDHtestEBEB->Print();
  RooHistPdf *myRPDFtestEBEB = new RooHistPdf("myRPDFtestEBEB","myRPDFtestEBEB",*redMass,*myRDHtestEBEB,0) ;
  cout << "RPDF EBEB" << endl;
  myRPDFtestEBEB->Print();
  cout << endl;
  TCanvas *c2EBEB = new TCanvas("c2EBEB","c2EBEB",1);
  RooPlot* myPlot2EBEB = redMass->frame(); 
  muWidth->setVal(4000);
  morphWidthCat0->plotOn(myPlot2EBEB, LineColor(kBlue));
  myRPDFtestEBEB->plotOn(myPlot2EBEB, LineColor(kRed));
  myPlot2EBEB->Draw();
  c2EBEB->SaveAs("checkMorphingPreChangeEBEB_vsDataMass4000.png");       
  cout << endl;
  delete c2EBEB;

  cout << "NOW TEST mc vs fit: EBEE" << endl;
  TString myRDHtestEBEEs = TString(Form("intWidthRDH_mass4000_catEBEE_kpl"))+coupling;
  RooDataHist *myRDHtestEBEE = (RooDataHist*)fileWidth->Get(myRDHtestEBEEs);
  cout << "RDH EBEE" << endl;
  myRDHtestEBEE->Print();
  RooHistPdf *myRPDFtestEBEE = new RooHistPdf("myRPDFtestEBEE","myRPDFtestEBEE",*redMass,*myRDHtestEBEE,0) ;
  cout << "RPDF EBEE" << endl;
  myRPDFtestEBEE->Print();
  cout << endl;
  TCanvas *c2EBEE = new TCanvas("c2EBEE","c2EBEE",1);
  RooPlot* myPlot2EBEE = redMass->frame();  
  muWidth->setVal(4000);
  morphWidthCat1->plotOn(myPlot2EBEE, LineColor(kBlue));
  myRPDFtestEBEE->plotOn(myPlot2EBEE, LineColor(kRed));
  myPlot2EBEE->Draw();
  c2EBEE->SaveAs("checkMorphingPreChangeEBEE_vsDataMass4000.png");       
  cout << endl;
  delete c2EBEE;

  cout << "NOW TEST mc vs fit: EBEB" << endl;
  myRDHtestEBEBs = TString(Form("intWidthRDH_mass750_catEBEB_kpl"))+coupling;
  myRDHtestEBEB = (RooDataHist*)fileWidth->Get(myRDHtestEBEBs);
  cout << "RDH EBEB" << endl;
  myRDHtestEBEB->Print();
  myRPDFtestEBEB = new RooHistPdf("myRPDFtestEBEB","myRPDFtestEBEB",*redMass,*myRDHtestEBEB,0) ;
  cout << "RPDF EBEB" << endl;
  myRPDFtestEBEB->Print();
  cout << endl;
  TCanvas *c2EBEBb = new TCanvas("c2EBEBb","c2EBEBb",1);
  RooPlot* myPlot2EBEBb = redMass->frame();
  muWidth->setVal(750);
  morphWidthCat0->plotOn(myPlot2EBEBb, LineColor(kBlue));
  myRPDFtestEBEB->plotOn(myPlot2EBEBb, LineColor(kRed));
  myPlot2EBEBb->Draw();
  c2EBEBb->SaveAs("checkMorphingPreChangeEBEB_vsDataMass750.png");       
  cout << endl;
  delete c2EBEBb;

  cout << "NOW TEST mc vs fit: EBEE" << endl;
  myRDHtestEBEEs = TString(Form("intWidthRDH_mass750_catEBEE_kpl"))+coupling;
  myRDHtestEBEE = (RooDataHist*)fileWidth->Get(myRDHtestEBEEs);
  cout << "RDH EBEE" << endl;
  myRDHtestEBEE->Print();
  myRPDFtestEBEE = new RooHistPdf("myRPDFtestEBEE","myRPDFtestEBEE",*redMass,*myRDHtestEBEE,0) ;
  cout << "RPDF EBEE" << endl;
  myRPDFtestEBEE->Print();
  cout << endl;
  TCanvas *c2EBEEb = new TCanvas("c2EBEEb","c2EBEEb",1);
  RooPlot* myPlot2EBEEb = redMass->frame();
  muWidth->setVal(750);
  morphWidthCat1->plotOn(myPlot2EBEEb, LineColor(kBlue));
  myRPDFtestEBEE->plotOn(myPlot2EBEEb, LineColor(kRed));
  myPlot2EBEEb->Draw();
  c2EBEEb->SaveAs("checkMorphingPreChangeEBEE_vsDataMass750.png");       
  cout << endl;
  cout << endl;
  delete c2EBEEb;


  cout << "NOW TEST2 evol: EBEB" << endl;
  TCanvas *c3EBEB = new TCanvas("c3EBEB","c3EBEB",1);
  RooPlot* myPlot3EBEB = redMass->frame();
  muWidth->setVal(500); morphWidthCat0->plotOn(myPlot3EBEB, LineColor(kBlue));
  muWidth->setVal(550); morphWidthCat0->plotOn(myPlot3EBEB, LineColor(kRed));
  muWidth->setVal(600); morphWidthCat0->plotOn(myPlot3EBEB, LineColor(kGreen));
  muWidth->setVal(650); morphWidthCat0->plotOn(myPlot3EBEB, LineColor(kOrange));
  muWidth->setVal(700); morphWidthCat0->plotOn(myPlot3EBEB, LineColor(kViolet));
  muWidth->setVal(750); morphWidthCat0->plotOn(myPlot3EBEB, LineColor(kYellow));
  muWidth->setVal(800); morphWidthCat0->plotOn(myPlot3EBEB, LineColor(kBlue));
  muWidth->setVal(850); morphWidthCat0->plotOn(myPlot3EBEB, LineColor(kRed));
  muWidth->setVal(900); morphWidthCat0->plotOn(myPlot3EBEB, LineColor(kGreen));
  muWidth->setVal(950); morphWidthCat0->plotOn(myPlot3EBEB, LineColor(kOrange));
  muWidth->setVal(1000); morphWidthCat0->plotOn(myPlot3EBEB, LineColor(kYellow));
  myPlot3EBEB->Draw();
  c3EBEB->SaveAs("checkMorphingPreChangeEBEB_manyMasses__500to1000.png");       

  myPlot3EBEB = redMass->frame();
  muWidth->setVal(1000); morphWidthCat0->plotOn(myPlot3EBEB, LineColor(kBlue));
  muWidth->setVal(1050); morphWidthCat0->plotOn(myPlot3EBEB, LineColor(kRed));
  muWidth->setVal(1100); morphWidthCat0->plotOn(myPlot3EBEB, LineColor(kGreen));
  muWidth->setVal(1150); morphWidthCat0->plotOn(myPlot3EBEB, LineColor(kOrange));
  muWidth->setVal(1200); morphWidthCat0->plotOn(myPlot3EBEB, LineColor(kViolet));
  muWidth->setVal(1250); morphWidthCat0->plotOn(myPlot3EBEB, LineColor(kYellow));
  muWidth->setVal(1300); morphWidthCat0->plotOn(myPlot3EBEB, LineColor(kBlue));
  muWidth->setVal(1350); morphWidthCat0->plotOn(myPlot3EBEB, LineColor(kRed));
  muWidth->setVal(1400); morphWidthCat0->plotOn(myPlot3EBEB, LineColor(kGreen));
  muWidth->setVal(1450); morphWidthCat0->plotOn(myPlot3EBEB, LineColor(kOrange));
  muWidth->setVal(1500); morphWidthCat0->plotOn(myPlot3EBEB, LineColor(kYellow));
  myPlot3EBEB->Draw();
  c3EBEB->SaveAs("checkMorphingPreChangeEBEB_manyMasses__1000to1500.png");       

  myPlot3EBEB = redMass->frame();
  muWidth->setVal(1500); morphWidthCat0->plotOn(myPlot3EBEB, LineColor(kBlue));
  muWidth->setVal(1550); morphWidthCat0->plotOn(myPlot3EBEB, LineColor(kRed));
  muWidth->setVal(1600); morphWidthCat0->plotOn(myPlot3EBEB, LineColor(kGreen));
  muWidth->setVal(1650); morphWidthCat0->plotOn(myPlot3EBEB, LineColor(kOrange));
  muWidth->setVal(1700); morphWidthCat0->plotOn(myPlot3EBEB, LineColor(kViolet));
  muWidth->setVal(1750); morphWidthCat0->plotOn(myPlot3EBEB, LineColor(kYellow));
  muWidth->setVal(1800); morphWidthCat0->plotOn(myPlot3EBEB, LineColor(kBlue));
  muWidth->setVal(1850); morphWidthCat0->plotOn(myPlot3EBEB, LineColor(kRed));
  muWidth->setVal(1900); morphWidthCat0->plotOn(myPlot3EBEB, LineColor(kGreen));
  muWidth->setVal(1950); morphWidthCat0->plotOn(myPlot3EBEB, LineColor(kOrange));
  muWidth->setVal(2000); morphWidthCat0->plotOn(myPlot3EBEB, LineColor(kYellow));
  myPlot3EBEB->Draw();
  c3EBEB->SaveAs("checkMorphingPreChangeEBEB_manyMasses__1500to2000.png");       

  myPlot3EBEB = redMass->frame();
  muWidth->setVal(2000); morphWidthCat0->plotOn(myPlot3EBEB, LineColor(kBlue));
  muWidth->setVal(2050); morphWidthCat0->plotOn(myPlot3EBEB, LineColor(kRed));
  muWidth->setVal(2100); morphWidthCat0->plotOn(myPlot3EBEB, LineColor(kGreen));
  muWidth->setVal(2150); morphWidthCat0->plotOn(myPlot3EBEB, LineColor(kOrange));
  muWidth->setVal(2200); morphWidthCat0->plotOn(myPlot3EBEB, LineColor(kViolet));
  muWidth->setVal(2250); morphWidthCat0->plotOn(myPlot3EBEB, LineColor(kYellow));
  muWidth->setVal(2300); morphWidthCat0->plotOn(myPlot3EBEB, LineColor(kBlue));
  muWidth->setVal(2350); morphWidthCat0->plotOn(myPlot3EBEB, LineColor(kRed));
  muWidth->setVal(2400); morphWidthCat0->plotOn(myPlot3EBEB, LineColor(kGreen));
  muWidth->setVal(2450); morphWidthCat0->plotOn(myPlot3EBEB, LineColor(kOrange));
  muWidth->setVal(2500); morphWidthCat0->plotOn(myPlot3EBEB, LineColor(kYellow));
  myPlot3EBEB->Draw();
  c3EBEB->SaveAs("checkMorphingPreChangeEBEB_manyMasses__2000to2500.png");       

  myPlot3EBEB = redMass->frame();
  muWidth->setVal(2500); morphWidthCat0->plotOn(myPlot3EBEB, LineColor(kBlue));
  muWidth->setVal(2550); morphWidthCat0->plotOn(myPlot3EBEB, LineColor(kRed));
  muWidth->setVal(2600); morphWidthCat0->plotOn(myPlot3EBEB, LineColor(kGreen));
  muWidth->setVal(2650); morphWidthCat0->plotOn(myPlot3EBEB, LineColor(kOrange));
  muWidth->setVal(2700); morphWidthCat0->plotOn(myPlot3EBEB, LineColor(kViolet));
  muWidth->setVal(2750); morphWidthCat0->plotOn(myPlot3EBEB, LineColor(kYellow));
  muWidth->setVal(2800); morphWidthCat0->plotOn(myPlot3EBEB, LineColor(kBlue));
  muWidth->setVal(2850); morphWidthCat0->plotOn(myPlot3EBEB, LineColor(kRed));
  muWidth->setVal(2900); morphWidthCat0->plotOn(myPlot3EBEB, LineColor(kGreen));
  muWidth->setVal(2950); morphWidthCat0->plotOn(myPlot3EBEB, LineColor(kOrange));
  muWidth->setVal(3000); morphWidthCat0->plotOn(myPlot3EBEB, LineColor(kYellow));
  myPlot3EBEB->Draw();
  c3EBEB->SaveAs("checkMorphingPreChangeEBEB_manyMasses__2500to3000.png");       

  myPlot3EBEB = redMass->frame();
  muWidth->setVal(3000); morphWidthCat0->plotOn(myPlot3EBEB, LineColor(kBlue));
  muWidth->setVal(3050); morphWidthCat0->plotOn(myPlot3EBEB, LineColor(kRed));
  muWidth->setVal(3100); morphWidthCat0->plotOn(myPlot3EBEB, LineColor(kGreen));
  muWidth->setVal(3150); morphWidthCat0->plotOn(myPlot3EBEB, LineColor(kOrange));
  muWidth->setVal(3200); morphWidthCat0->plotOn(myPlot3EBEB, LineColor(kViolet));
  muWidth->setVal(3250); morphWidthCat0->plotOn(myPlot3EBEB, LineColor(kYellow));
  muWidth->setVal(3300); morphWidthCat0->plotOn(myPlot3EBEB, LineColor(kBlue));
  muWidth->setVal(3350); morphWidthCat0->plotOn(myPlot3EBEB, LineColor(kRed));
  muWidth->setVal(3400); morphWidthCat0->plotOn(myPlot3EBEB, LineColor(kGreen));
  muWidth->setVal(3450); morphWidthCat0->plotOn(myPlot3EBEB, LineColor(kOrange));
  muWidth->setVal(3500); morphWidthCat0->plotOn(myPlot3EBEB, LineColor(kYellow));
  myPlot3EBEB->Draw();
  c3EBEB->SaveAs("checkMorphingPreChangeEBEB_manyMasses__3000to3500.png");       

  myPlot3EBEB = redMass->frame();
  muWidth->setVal(3500); morphWidthCat0->plotOn(myPlot3EBEB, LineColor(kBlue));
  muWidth->setVal(3550); morphWidthCat0->plotOn(myPlot3EBEB, LineColor(kRed));
  muWidth->setVal(3600); morphWidthCat0->plotOn(myPlot3EBEB, LineColor(kGreen));
  muWidth->setVal(3650); morphWidthCat0->plotOn(myPlot3EBEB, LineColor(kOrange));
  muWidth->setVal(3700); morphWidthCat0->plotOn(myPlot3EBEB, LineColor(kViolet));
  muWidth->setVal(3750); morphWidthCat0->plotOn(myPlot3EBEB, LineColor(kYellow));
  muWidth->setVal(3800); morphWidthCat0->plotOn(myPlot3EBEB, LineColor(kBlue));
  muWidth->setVal(3850); morphWidthCat0->plotOn(myPlot3EBEB, LineColor(kRed));
  muWidth->setVal(3900); morphWidthCat0->plotOn(myPlot3EBEB, LineColor(kGreen));
  muWidth->setVal(3950); morphWidthCat0->plotOn(myPlot3EBEB, LineColor(kOrange));
  muWidth->setVal(4000); morphWidthCat0->plotOn(myPlot3EBEB, LineColor(kYellow));
  myPlot3EBEB->Draw();
  c3EBEB->SaveAs("checkMorphingPreChangeEBEB_manyMasses__3500to4000.png");       

  myPlot3EBEB = redMass->frame();
  muWidth->setVal(4000); morphWidthCat0->plotOn(myPlot3EBEB, LineColor(kBlue));
  muWidth->setVal(4050); morphWidthCat0->plotOn(myPlot3EBEB, LineColor(kRed));
  muWidth->setVal(4100); morphWidthCat0->plotOn(myPlot3EBEB, LineColor(kGreen));
  muWidth->setVal(4150); morphWidthCat0->plotOn(myPlot3EBEB, LineColor(kOrange));
  muWidth->setVal(4200); morphWidthCat0->plotOn(myPlot3EBEB, LineColor(kViolet));
  muWidth->setVal(4250); morphWidthCat0->plotOn(myPlot3EBEB, LineColor(kYellow));
  muWidth->setVal(4300); morphWidthCat0->plotOn(myPlot3EBEB, LineColor(kBlue));
  muWidth->setVal(4350); morphWidthCat0->plotOn(myPlot3EBEB, LineColor(kRed));
  muWidth->setVal(4400); morphWidthCat0->plotOn(myPlot3EBEB, LineColor(kGreen));
  muWidth->setVal(4450); morphWidthCat0->plotOn(myPlot3EBEB, LineColor(kOrange));
  muWidth->setVal(4500); morphWidthCat0->plotOn(myPlot3EBEB, LineColor(kYellow));
  myPlot3EBEB->Draw();
  c3EBEB->SaveAs("checkMorphingPreChangeEBEB_manyMasses__4000to4500.png");       

  myPlot3EBEB = redMass->frame();
  muWidth->setVal(4500); morphWidthCat0->plotOn(myPlot3EBEB, LineColor(kBlue));
  muWidth->setVal(4550); morphWidthCat0->plotOn(myPlot3EBEB, LineColor(kRed));
  muWidth->setVal(4600); morphWidthCat0->plotOn(myPlot3EBEB, LineColor(kGreen));
  muWidth->setVal(4650); morphWidthCat0->plotOn(myPlot3EBEB, LineColor(kOrange));
  muWidth->setVal(4700); morphWidthCat0->plotOn(myPlot3EBEB, LineColor(kViolet));
  muWidth->setVal(4750); morphWidthCat0->plotOn(myPlot3EBEB, LineColor(kYellow));
  muWidth->setVal(4800); morphWidthCat0->plotOn(myPlot3EBEB, LineColor(kBlue));
  muWidth->setVal(4850); morphWidthCat0->plotOn(myPlot3EBEB, LineColor(kRed));
  muWidth->setVal(4900); morphWidthCat0->plotOn(myPlot3EBEB, LineColor(kGreen));
  muWidth->setVal(4950); morphWidthCat0->plotOn(myPlot3EBEB, LineColor(kOrange));
  myPlot3EBEB->Draw();
  c3EBEB->SaveAs("checkMorphingPreChangeEBEB_manyMasses__4500to4950.png");       
  cout << endl;
  delete c3EBEB;


  cout << "NOW TEST2 evol: EBEE" << endl;
  TCanvas *c3EBEE = new TCanvas("c3EBEE","c3EBEE",1);
  RooPlot* myPlot3EBEE = redMass->frame();
  muWidth->setVal(500); morphWidthCat1->plotOn(myPlot3EBEE, LineColor(kBlue));
  muWidth->setVal(550); morphWidthCat1->plotOn(myPlot3EBEE, LineColor(kRed));
  muWidth->setVal(600); morphWidthCat1->plotOn(myPlot3EBEE, LineColor(kGreen));
  muWidth->setVal(650); morphWidthCat1->plotOn(myPlot3EBEE, LineColor(kOrange));
  muWidth->setVal(700); morphWidthCat1->plotOn(myPlot3EBEE, LineColor(kViolet));
  muWidth->setVal(750); morphWidthCat1->plotOn(myPlot3EBEE, LineColor(kYellow));
  muWidth->setVal(800); morphWidthCat1->plotOn(myPlot3EBEE, LineColor(kBlue));
  muWidth->setVal(850); morphWidthCat1->plotOn(myPlot3EBEE, LineColor(kRed));
  muWidth->setVal(900); morphWidthCat1->plotOn(myPlot3EBEE, LineColor(kGreen));
  muWidth->setVal(950); morphWidthCat1->plotOn(myPlot3EBEE, LineColor(kOrange));
  muWidth->setVal(1000); morphWidthCat1->plotOn(myPlot3EBEE, LineColor(kYellow));
  myPlot3EBEE->Draw();
  c3EBEE->SaveAs("checkMorphingPreChangeEBEE_manyMasses__500to1000.png");       

  myPlot3EBEE = redMass->frame();
  muWidth->setVal(1000); morphWidthCat1->plotOn(myPlot3EBEE, LineColor(kBlue));
  muWidth->setVal(1050); morphWidthCat1->plotOn(myPlot3EBEE, LineColor(kRed));
  muWidth->setVal(1100); morphWidthCat1->plotOn(myPlot3EBEE, LineColor(kGreen));
  muWidth->setVal(1150); morphWidthCat1->plotOn(myPlot3EBEE, LineColor(kOrange));
  muWidth->setVal(1200); morphWidthCat1->plotOn(myPlot3EBEE, LineColor(kViolet));
  muWidth->setVal(1250); morphWidthCat1->plotOn(myPlot3EBEE, LineColor(kYellow));
  muWidth->setVal(1300); morphWidthCat1->plotOn(myPlot3EBEE, LineColor(kBlue));
  muWidth->setVal(1350); morphWidthCat1->plotOn(myPlot3EBEE, LineColor(kRed));
  muWidth->setVal(1400); morphWidthCat1->plotOn(myPlot3EBEE, LineColor(kGreen));
  muWidth->setVal(1450); morphWidthCat1->plotOn(myPlot3EBEE, LineColor(kOrange));
  muWidth->setVal(1500); morphWidthCat1->plotOn(myPlot3EBEE, LineColor(kYellow));
  myPlot3EBEE->Draw();
  c3EBEE->SaveAs("checkMorphingPreChangeEBEE_manyMasses__1000to1500.png");       

  myPlot3EBEE = redMass->frame();
  muWidth->setVal(1500); morphWidthCat1->plotOn(myPlot3EBEE, LineColor(kBlue));
  muWidth->setVal(1550); morphWidthCat1->plotOn(myPlot3EBEE, LineColor(kRed));
  muWidth->setVal(1600); morphWidthCat1->plotOn(myPlot3EBEE, LineColor(kGreen));
  muWidth->setVal(1650); morphWidthCat1->plotOn(myPlot3EBEE, LineColor(kOrange));
  muWidth->setVal(1700); morphWidthCat1->plotOn(myPlot3EBEE, LineColor(kViolet));
  muWidth->setVal(1750); morphWidthCat1->plotOn(myPlot3EBEE, LineColor(kYellow));
  muWidth->setVal(1800); morphWidthCat1->plotOn(myPlot3EBEE, LineColor(kBlue));
  muWidth->setVal(1850); morphWidthCat1->plotOn(myPlot3EBEE, LineColor(kRed));
  muWidth->setVal(1900); morphWidthCat1->plotOn(myPlot3EBEE, LineColor(kGreen));
  muWidth->setVal(1950); morphWidthCat1->plotOn(myPlot3EBEE, LineColor(kOrange));
  muWidth->setVal(2000); morphWidthCat1->plotOn(myPlot3EBEE, LineColor(kYellow));
  myPlot3EBEE->Draw();
  c3EBEE->SaveAs("checkMorphingPreChangeEBEE_manyMasses__1500to2000.png");       

  myPlot3EBEE = redMass->frame();
  muWidth->setVal(2000); morphWidthCat1->plotOn(myPlot3EBEE, LineColor(kBlue));
  muWidth->setVal(2050); morphWidthCat1->plotOn(myPlot3EBEE, LineColor(kRed));
  muWidth->setVal(2100); morphWidthCat1->plotOn(myPlot3EBEE, LineColor(kGreen));
  muWidth->setVal(2150); morphWidthCat1->plotOn(myPlot3EBEE, LineColor(kOrange));
  muWidth->setVal(2200); morphWidthCat1->plotOn(myPlot3EBEE, LineColor(kViolet));
  muWidth->setVal(2250); morphWidthCat1->plotOn(myPlot3EBEE, LineColor(kYellow));
  muWidth->setVal(2300); morphWidthCat1->plotOn(myPlot3EBEE, LineColor(kBlue));
  muWidth->setVal(2350); morphWidthCat1->plotOn(myPlot3EBEE, LineColor(kRed));
  muWidth->setVal(2400); morphWidthCat1->plotOn(myPlot3EBEE, LineColor(kGreen));
  muWidth->setVal(2450); morphWidthCat1->plotOn(myPlot3EBEE, LineColor(kOrange));
  muWidth->setVal(2500); morphWidthCat1->plotOn(myPlot3EBEE, LineColor(kYellow));
  myPlot3EBEE->Draw();
  c3EBEE->SaveAs("checkMorphingPreChangeEBEE_manyMasses__2000to2500.png");       

  myPlot3EBEE = redMass->frame();
  muWidth->setVal(2500); morphWidthCat1->plotOn(myPlot3EBEE, LineColor(kBlue));
  muWidth->setVal(2550); morphWidthCat1->plotOn(myPlot3EBEE, LineColor(kRed));
  muWidth->setVal(2600); morphWidthCat1->plotOn(myPlot3EBEE, LineColor(kGreen));
  muWidth->setVal(2650); morphWidthCat1->plotOn(myPlot3EBEE, LineColor(kOrange));
  muWidth->setVal(2700); morphWidthCat1->plotOn(myPlot3EBEE, LineColor(kViolet));
  muWidth->setVal(2750); morphWidthCat1->plotOn(myPlot3EBEE, LineColor(kYellow));
  muWidth->setVal(2800); morphWidthCat1->plotOn(myPlot3EBEE, LineColor(kBlue));
  muWidth->setVal(2850); morphWidthCat1->plotOn(myPlot3EBEE, LineColor(kRed));
  muWidth->setVal(2900); morphWidthCat1->plotOn(myPlot3EBEE, LineColor(kGreen));
  muWidth->setVal(2950); morphWidthCat1->plotOn(myPlot3EBEE, LineColor(kOrange));
  muWidth->setVal(3000); morphWidthCat1->plotOn(myPlot3EBEE, LineColor(kYellow));
  myPlot3EBEE->Draw();
  c3EBEE->SaveAs("checkMorphingPreChangeEBEE_manyMasses__2500to3000.png");       

  myPlot3EBEE = redMass->frame();
  muWidth->setVal(3000); morphWidthCat1->plotOn(myPlot3EBEE, LineColor(kBlue));
  muWidth->setVal(3050); morphWidthCat1->plotOn(myPlot3EBEE, LineColor(kRed));
  muWidth->setVal(3100); morphWidthCat1->plotOn(myPlot3EBEE, LineColor(kGreen));
  muWidth->setVal(3150); morphWidthCat1->plotOn(myPlot3EBEE, LineColor(kOrange));
  muWidth->setVal(3200); morphWidthCat1->plotOn(myPlot3EBEE, LineColor(kViolet));
  muWidth->setVal(3250); morphWidthCat1->plotOn(myPlot3EBEE, LineColor(kYellow));
  muWidth->setVal(3300); morphWidthCat1->plotOn(myPlot3EBEE, LineColor(kBlue));
  muWidth->setVal(3350); morphWidthCat1->plotOn(myPlot3EBEE, LineColor(kRed));
  muWidth->setVal(3400); morphWidthCat1->plotOn(myPlot3EBEE, LineColor(kGreen));
  muWidth->setVal(3450); morphWidthCat1->plotOn(myPlot3EBEE, LineColor(kOrange));
  muWidth->setVal(3500); morphWidthCat1->plotOn(myPlot3EBEE, LineColor(kYellow));
  myPlot3EBEE->Draw();
  c3EBEE->SaveAs("checkMorphingPreChangeEBEE_manyMasses__3000to3500.png");       

  myPlot3EBEE = redMass->frame();
  muWidth->setVal(3500); morphWidthCat1->plotOn(myPlot3EBEE, LineColor(kBlue));
  muWidth->setVal(3550); morphWidthCat1->plotOn(myPlot3EBEE, LineColor(kRed));
  muWidth->setVal(3600); morphWidthCat1->plotOn(myPlot3EBEE, LineColor(kGreen));
  muWidth->setVal(3650); morphWidthCat1->plotOn(myPlot3EBEE, LineColor(kOrange));
  muWidth->setVal(3700); morphWidthCat1->plotOn(myPlot3EBEE, LineColor(kViolet));
  muWidth->setVal(3750); morphWidthCat1->plotOn(myPlot3EBEE, LineColor(kYellow));
  muWidth->setVal(3800); morphWidthCat1->plotOn(myPlot3EBEE, LineColor(kBlue));
  muWidth->setVal(3850); morphWidthCat1->plotOn(myPlot3EBEE, LineColor(kRed));
  muWidth->setVal(3900); morphWidthCat1->plotOn(myPlot3EBEE, LineColor(kGreen));
  muWidth->setVal(3950); morphWidthCat1->plotOn(myPlot3EBEE, LineColor(kOrange));
  muWidth->setVal(4000); morphWidthCat1->plotOn(myPlot3EBEE, LineColor(kYellow));
  myPlot3EBEE->Draw();
  c3EBEE->SaveAs("checkMorphingPreChangeEBEE_manyMasses__3500to4000.png");       

  myPlot3EBEE = redMass->frame();
  muWidth->setVal(4000); morphWidthCat1->plotOn(myPlot3EBEE, LineColor(kBlue));
  muWidth->setVal(4050); morphWidthCat1->plotOn(myPlot3EBEE, LineColor(kRed));
  muWidth->setVal(4100); morphWidthCat1->plotOn(myPlot3EBEE, LineColor(kGreen));
  muWidth->setVal(4150); morphWidthCat1->plotOn(myPlot3EBEE, LineColor(kOrange));
  muWidth->setVal(4200); morphWidthCat1->plotOn(myPlot3EBEE, LineColor(kViolet));
  muWidth->setVal(4250); morphWidthCat1->plotOn(myPlot3EBEE, LineColor(kYellow));
  muWidth->setVal(4300); morphWidthCat1->plotOn(myPlot3EBEE, LineColor(kBlue));
  muWidth->setVal(4350); morphWidthCat1->plotOn(myPlot3EBEE, LineColor(kRed));
  muWidth->setVal(4400); morphWidthCat1->plotOn(myPlot3EBEE, LineColor(kGreen));
  muWidth->setVal(4450); morphWidthCat1->plotOn(myPlot3EBEE, LineColor(kOrange));
  muWidth->setVal(4500); morphWidthCat1->plotOn(myPlot3EBEE, LineColor(kYellow));
  myPlot3EBEE->Draw();
  c3EBEE->SaveAs("checkMorphingPreChangeEBEE_manyMasses__4000to4500.png");       

  myPlot3EBEE = redMass->frame();
  muWidth->setVal(4500); morphWidthCat1->plotOn(myPlot3EBEE, LineColor(kBlue));
  muWidth->setVal(4550); morphWidthCat1->plotOn(myPlot3EBEE, LineColor(kRed));
  muWidth->setVal(4600); morphWidthCat1->plotOn(myPlot3EBEE, LineColor(kGreen));
  muWidth->setVal(4650); morphWidthCat1->plotOn(myPlot3EBEE, LineColor(kOrange));
  muWidth->setVal(4700); morphWidthCat1->plotOn(myPlot3EBEE, LineColor(kViolet));
  muWidth->setVal(4750); morphWidthCat1->plotOn(myPlot3EBEE, LineColor(kYellow));
  muWidth->setVal(4800); morphWidthCat1->plotOn(myPlot3EBEE, LineColor(kBlue));
  muWidth->setVal(4850); morphWidthCat1->plotOn(myPlot3EBEE, LineColor(kRed));
  muWidth->setVal(4900); morphWidthCat1->plotOn(myPlot3EBEE, LineColor(kGreen));
  muWidth->setVal(4950); morphWidthCat1->plotOn(myPlot3EBEE, LineColor(kOrange));
  myPlot3EBEE->Draw();
  c3EBEE->SaveAs("checkMorphingPreChangeEBEE_manyMasses__4500to4950.png");       
  cout << endl;
  delete c3EBEE;

  // Close the input file 
  fileWidth->Close();     



  // Now we redefine the variable: so far (mgen-mX)/GammaX => now (mgen-mX)
  RooCustomizer customCat0(*morphWidthCat0,"reparamCat0");             
  cout << "chiara, custom, EBEB" << endl;
  customCat0.Print();
  RooCustomizer customCat1(*morphWidthCat1,"reparamCat1");             
  cout << "chiara, custom, EBEE" << endl;
  customCat1.Print();
  cout << endl;
  cout << endl;

  RooRealVar* hundk = new RooRealVar("hundk","", 1,30.);                         // kpl*100
  if (coupling=="001") hundk->setVal(1);
  if (coupling=="01")  hundk->setVal(10);
  if (coupling=="02")  hundk->setVal(20);
  hundk->setConstant(true); 
  cout << "chiara: hundk" << endl;
  hundk->Print();
  cout << endl;

  RooRealVar* fix1d4 = new RooRealVar("fix1d4","fix1d4", 1.4e-4);
  fix1d4->setConstant(true);
  cout << "chiara: fix1d4" << endl;
  fix1d4->Print();
  cout << endl;

  RooProduct * relInvGammaHden = new RooProduct("relInvGammaHden","relInvGammaHden",RooArgList(*fix1d4,*hundk,*hundk));      // gamma/M 
  cout << "chiara: relInvGammaHden" << endl;
  relInvGammaHden->Print();
  cout << endl;

  RooProduct * invGammaHden = new RooProduct("invGammaHden","invGammaHden",RooArgList(*muWidth,*relInvGammaHden));                            // gamma
  cout << "chiara: invGammaHden" << endl;
  invGammaHden->Print();
  cout << endl;

  RooFormulaVar * invGammaH = new RooFormulaVar("invGammaH","1./@0",RooArgList(*invGammaHden));                                               // 1/gamma
  cout << "chiara: invGammaH" << endl;
  invGammaH->Print();
  cout << endl;

  RooRealVar* fix0 = new RooRealVar("fix0","fix0", 0);
  fix0->setConstant(true);
  cout << "chiara: fix0" << endl;
  fix0->Print();
  cout << endl;

  RooLinearVar * linRedMass = new RooLinearVar("linRedMass","linRedMass",*deltaMgen,*invGammaH,*fix0);              // new morphing is function of deltaMgen
  cout << "chiara: linRedMass" << endl;
  linRedMass->Print();  
  cout << endl;

  // change of variable
  customCat0.replaceArg(*redMass,*linRedMass);
  cout << "chiara: customCat0" << endl;
  customCat0.Print();
  cout << endl;
  customCat1.replaceArg(*redMass,*linRedMass);
  cout << "chiara: customCat1" << endl;
  customCat1.Print();
  cout << endl;

  // new functions
  RooAbsPdf* reMorphCat0 = (RooAbsPdf*)customCat0.build();                
  cout << "chiara: reMorphCat0" << endl;
  reMorphCat0->Print();
  cout << endl;
  RooAbsPdf* reMorphCat1 = (RooAbsPdf*)customCat1.build();                
  cout << "chiara: reMorphCat1" << endl;
  reMorphCat1->Print();
  cout << endl;


  // chiara debug: new cout after fixing one reference mass
  cout << "chiara: debug with mass 750" << endl;
  muWidth->setVal(750); 
  cout << "=> invGammaHden" << endl;
  invGammaHden->Print();
  cout << endl;
  cout << "=> invGammaH" << endl;
  invGammaH->Print();
  cout << endl;
  cout << "=> linRedMass" << endl;
  linRedMass->Print();  
  cout << endl;
  cout << "=> reMorphCat0" << endl;
  reMorphCat0->Print();
  cout << endl;
  cout << "=> reMorphCat1" << endl;
  reMorphCat1->Print();
  cout << endl;

  //cout << "chiara: further debug" << endl;
  //deltaMgen->setVal(0.1);
  //linRedMass->Print();
  //reMorphCat0->Print();
  //cout << "chiara: further debug - now the opposite" << endl;
  //linRedMass->setVal(0.00952381);
  //deltaMgen->Print();
  //reMorphCat0->Print();

  // chiara debug: plot after fixing one reference mass  
  TCanvas *c4 = new TCanvas("c4","c4",1);
  RooPlot* myPlot4 = deltaMgen->frame();
  muWidth->setVal(750); 
  reMorphCat0->plotOn(myPlot4, LineColor(kRed));
  reMorphCat1->plotOn(myPlot4, LineColor(kGreen));
  myPlot4->Draw();
  c4->SaveAs("checkMorphingAfterChange_750.png");       
  cout << endl;
  delete c4;

  TCanvas *c4b = new TCanvas("c4b","c4b",1);
  RooPlot* myPlot4b = deltaMgen->frame();
  muWidth->setVal(550); 
  reMorphCat0->plotOn(myPlot4b, LineColor(kRed));
  reMorphCat1->plotOn(myPlot4b, LineColor(kGreen));
  myPlot4b->Draw();
  c4b->SaveAs("checkMorphingAfterChange_550.png");       
  cout << endl;
  delete c4b;

  TCanvas *c4c = new TCanvas("c4c","c4c",1);
  RooPlot* myPlot4c = deltaMgen->frame();
  muWidth->setVal(4300); 
  reMorphCat0->plotOn(myPlot4c, LineColor(kRed));
  reMorphCat1->plotOn(myPlot4c, LineColor(kGreen));
  myPlot4c->Draw();
  c4c->SaveAs("checkMorphingAfterChange_4300.png");       
  cout << endl;
  delete c4c;


  // Now we evaluate with a fine mass scan samples and save the corresponding roodatahists  
  cout << endl;
  cout << endl;
  cout << "----------------------------" << endl;  
  cout << endl;
  cout << "now evaluating the morphing every 50GeV, from 500GeV to 5000GeV: doing mass " << endl; 

  // This is to have 2GeV steps between 500 and 1000 GeV  - chiara
  for (int iGenMass=0; iGenMass<250; iGenMass++) {  
    int thisMass = 500 + iGenMass*2; 
    cout << "Fine scan (2GeV): " << thisMass << endl;  
    muWidth->setVal(thisMass);
    for (int c=0; c<NCAT; ++c) {  
      TString myCut = "EBEB";  
      if (c==1) myCut = "EBEE";  
      RooDataHist *fittWidthRDH; 
      // chiara: in questo modo il range e' maggiore di quello comunque gia' tagliato. Ho bin di 2 GeV centrati in zero
      if (coupling=="001") {
	deltaMgen->setRange(-5.,5.);               
	deltaMgen->setBins(5);                   
      } else if (coupling=="01") {
	deltaMgen->setRange(-325.,325.);           
	//deltaMgen->setBins(325);                   
	deltaMgen->setBins(1300);                   
      } else if (coupling=="02") {
	deltaMgen->setRange(-1205.,1205.);         
	deltaMgen->setBins(1205);                   
      }
      if(c==0) fittWidthRDH = reMorphCat0->generateBinned(*deltaMgen,10000,kTRUE);   
      if(c==1) fittWidthRDH = reMorphCat1->generateBinned(*deltaMgen,10000,kTRUE);   
      fittWidthRDH->Print();
      TString myFitRDHa = TString::Format("widthRDH_mass%d_cat",thisMass)+myCut;    
      TString myFitRDH  = myFitRDHa+"_kpl"+coupling;
      fittWidthRDH->SetTitle(myFitRDH);  
      fittWidthRDH->SetName(myFitRDH);  
      w->import(*fittWidthRDH);
    }
  }

  // Then 4GeV steps between 1000 and 1600 GeV    
  for (int iGenMass=0; iGenMass<150; iGenMass++) { 
    int thisMass = 1000 + iGenMass*4.;  
    cout << "Medium (4GeV) scan: " << thisMass << endl;  
    muWidth->setVal(thisMass);
    for (int c=0; c<NCAT; ++c) {  
      TString myCut = "EBEB";  
      if (c==1) myCut = "EBEE";  
      RooDataHist *fittWidthRDH; 
      if (coupling=="001") {
	deltaMgen->setRange(-5.,5.);               
	deltaMgen->setBins(5);                   
      } else if (coupling=="01") {
	deltaMgen->setRange(-325.,325.);           
	deltaMgen->setBins(1300);                   
	//deltaMgen->setBins(325);                   
      } else if (coupling=="02") {
	deltaMgen->setRange(-1205.,1205.);         
	deltaMgen->setBins(1205);                   
      }
      if(c==0) fittWidthRDH = reMorphCat0->generateBinned(*deltaMgen,10000,kTRUE);   
      if(c==1) fittWidthRDH = reMorphCat1->generateBinned(*deltaMgen,10000,kTRUE);   
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
      if (coupling=="001") {
	deltaMgen->setRange(-5.,5.);               
	deltaMgen->setBins(5);                   
      } else if (coupling=="01") {
	deltaMgen->setRange(-325.,325.);           
	deltaMgen->setBins(1300);                   
	//deltaMgen->setBins(325);                   
      } else if (coupling=="02") {
	deltaMgen->setRange(-1205.,1205.);         
	deltaMgen->setBins(1205);                   
      }
      if(c==0) fittWidthRDH = reMorphCat0->generateBinned(*deltaMgen,10000,kTRUE);   
      if(c==1) fittWidthRDH = reMorphCat1->generateBinned(*deltaMgen,10000,kTRUE);   
      fittWidthRDH->Print();
      TString myFitRDHa = TString::Format("widthRDH_mass%d_cat",thisMass)+myCut;    
      TString myFitRDH  = myFitRDHa+"_kpl"+coupling;
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
      masses.push_back(2750);
      masses.push_back(3000);
      masses.push_back(3500);
      masses.push_back(4000);
      masses.push_back(4500);
    } else if (coupling=="001") {
      masses.push_back(500);
      masses.push_back(750);
      masses.push_back(1000);
      masses.push_back(2000);
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

