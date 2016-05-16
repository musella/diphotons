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
#include "RooCustomizer.h"
#include "RooLinearVar.h"
#include "RooProduct.h"

using namespace RooFit;
using namespace std;

// to be modified:
static const Int_t NCAT = 2;  

// Preparing the intrinsic width histogram
void MakeIntrinsicWidthHisto(TString filename, bool newFile, int mass, string couplingS) {
  
  TString myMass = TString::Format("%d",mass);
  Int_t ncat = NCAT;

  // Roorealvar: deltaM/gamma - chiara
  RooRealVar* redMass   = new RooRealVar("redMass", "", -4., 4., "");

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
    TH1D *intWidthH = new TH1D("intWidthH","intWidthH", 21, -4.1, 4.1);   // chiara: cosi' ho bin da ~0.4 in deltaM/Gamma
    intWidthH->Sumw2();
    
    // Projecting the tree
    TString express;   // deltaM/Gamma  
    if(couplingS=="001") express = TString::Format("(mggGen-%d)/(2.5*1.4*0.01*0.01*%d)",mass,mass);  // chiara: 2.5 in piu'
    if(couplingS=="005") express = TString::Format("(mggGen-%d)/(1.4*0.05*0.05*%d)",mass,mass);
    if(couplingS=="007") express = TString::Format("(mggGen-%d)/(1.4*0.07*0.07*%d)",mass,mass);
    if(couplingS=="01")  express = TString::Format("(mggGen-%d)/(1.4*0.1*0.1*%d)",mass,mass);
    if(couplingS=="015") express = TString::Format("(mggGen-%d)/(1.4*0.15*0.15*%d)",mass,mass);
    if(couplingS=="02")  express = TString::Format("(mggGen-%d)/(1.4*0.2*0.2*%d)",mass,mass);
    if(couplingS=="025") express = TString::Format("(mggGen-%d)/(1.4*0.25*0.25*%d)",mass,mass);
    if(couplingS=="03")  express = TString::Format("(mggGen-%d)/(1.4*0.3*0.3*%d)",mass,mass);
    cout << "chiara, debug: variable is " << express << endl;
    if (c==0) sigTree->Project("intWidthH",express,mainCut+TString::Format("&& eventClass==0"));
    if (c==1) sigTree->Project("intWidthH",express,mainCut+TString::Format("&& eventClass==1"));
    cout << "Overflow: " << intWidthH->GetBinContent(22) << ", underflow: " << intWidthH->GetBinContent(0)  << endl; 
    cout << "Overflow + underflow = " << (float(intWidthH->GetBinContent(22))+float(intWidthH->GetBinContent(0)))/(float)intWidthH->GetEntries() << endl;    

    // Now make the roodatahist
    RooDataHist intWidthRDHa("intWidthRDHa","intWidthRDHa",*redMass,intWidthH);
    cout << "Intrinsic width datahist summary for mass " << mass << " and coupling " << couplingS << ", large binning" << endl;  
    intWidthRDHa.Print();
    cout << endl;  

    // Now generate from this roodatahist, with more bins   
    RooHistPdf *intWidthRHPDFa = new RooHistPdf("intWidthRHPDFa","intWidthRHPDFa",*redMass,intWidthRDHa,2);
    cout << "Converting in RooHistPdf to change the binning" << endl;
    intWidthRHPDFa->Print();
    cout << endl;
    redMass->setBins(81);                   // chiara: @750GeV resol=11GeV. With 81bins => bining in dM/G=0.1 => 4GeV in dM for k=0.2, 6.5GeV for k=0.25, 9.4GeV for k=0.3  
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
    TString nameRDH = TString::Format("intWidthRDH_mass%d",mass)+TString::Format("_cat")+myCut+TString::Format("_kpl")+couplingS;
    intWidthRDH->Write(nameRDH);

    // Plot to check  
    TCanvas *c1 = new TCanvas("c1","c1",1);
    RooPlot* myPlot = redMass->frame();
    intWidthRDH->plotOn(myPlot, LineColor(kRed), LineStyle(kDashed), Rescale(1./intWidthRDH->sumEntries()));
    myPlot->Draw();
    TString canvasName = TString(Form("/tmp/crovelli/CheckIntrinsicWidth_Cat"))+TString(Form(myCut))+TString(Form("_mass"))+TString(Form(myMass))+TString(Form("kpl"))+couplingS+TString(Form(".png"));
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
void WidthInterpolation(RooWorkspace* w, int mass, vector<string> couplingsS, bool newfile) {  
  
  // RooRealVars 
  RooRealVar* deltaMgen;                 // mgen-mX     chiara 
  deltaMgen = new RooRealVar("deltaMgen", "", -395., 395., "GeV");  
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

  // This mass
  RooRealVar* mH = new RooRealVar("mH","mH", mass);
  mH->setConstant(true);
  cout << "chiara: mH" << endl;
  mH->Print();
  cout << endl;

  // The interpolation variable: kpl        
  RooRealVar* muWidth = new RooRealVar("muWidth", "", 0, 0.5, "");   
  cout << "chiara: muWidth" << endl;
  muWidth->Print();
  cout << endl;

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
  }   // loop over couplings

  // Now morphing for the two categories    
  cout << endl;   
  cout << "morphing for the intrinsic width functions" << endl; 
  pdfsCat0.Print();  
  pdfsCat1.Print(); 
  RooMomentMorph *morphWidthCat0 = new RooMomentMorph("morphWidthCat0","morphWidthCat0",*muWidth,varlist,pdfsCat0,paramVec,RooMomentMorph::Linear);
  morphWidthCat0->useHorizontalMorphing(false);
  morphWidthCat0->Print();      
  RooMomentMorph *morphWidthCat1 = new RooMomentMorph("morphWidthCat1","morphWidthCat1",*muWidth,varlist,pdfsCat1,paramVec,RooMomentMorph::Linear);
  morphWidthCat1->useHorizontalMorphing(false);
  morphWidthCat1->Print();      
  cout << endl;
  cout << endl;

  // Plot to check  
  cout << "NOW TEST mc vs fit: EBEB" << endl;
  TString myRDHtestEBEBs = "intWidthRDH_mass750_catEBEB_kpl005";
  RooDataHist *myRDHtestEBEB = (RooDataHist*)fileWidth->Get(myRDHtestEBEBs);
  cout << "RDH EBEB" << endl;
  myRDHtestEBEB->Print();
  RooHistPdf *myRPDFtestEBEB = new RooHistPdf("myRPDFtestEBEB","myRPDFtestEBEB",*redMass,*myRDHtestEBEB,0) ;
  cout << "RPDF EBEB" << endl;
  myRPDFtestEBEB->Print();
  cout << endl;
  TCanvas *c2EBEB = new TCanvas("c2EBEB","c2EBEB",1);
  RooPlot* myPlot2EBEB = redMass->frame();
  muWidth->setVal(0.05);
  morphWidthCat0->plotOn(myPlot2EBEB, LineColor(kBlue));
  myRPDFtestEBEB->plotOn(myPlot2EBEB, LineColor(kRed));
  myPlot2EBEB->Draw();
  c2EBEB->SaveAs("checkMorphingPreChangeEBEB_vsDataK005.png");
  cout << endl;
  delete c2EBEB;

  cout << "NOW TEST mc vs fit: EBEE" << endl;
  TString myRDHtestEBEEs = "intWidthRDH_mass750_catEBEE_kpl005";
  RooDataHist *myRDHtestEBEE = (RooDataHist*)fileWidth->Get(myRDHtestEBEEs);
  cout << "RDH EBEE" << endl;
  myRDHtestEBEE->Print();
  RooHistPdf *myRPDFtestEBEE = new RooHistPdf("myRPDFtestEBEE","myRPDFtestEBEE",*redMass,*myRDHtestEBEE,0) ;
  cout << "RPDF EBEE" << endl;
  myRPDFtestEBEE->Print();
  cout << endl;
  TCanvas *c2EBEE = new TCanvas("c2EBEE","c2EBEE",1);
  RooPlot* myPlot2EBEE = redMass->frame();
  muWidth->setVal(0.05);
  morphWidthCat1->plotOn(myPlot2EBEE, LineColor(kBlue));
  myRPDFtestEBEE->plotOn(myPlot2EBEE, LineColor(kRed));
  myPlot2EBEE->Draw();
  c2EBEE->SaveAs("checkMorphingPreChangeEBEE_vsDataK005.png");
  cout << endl;
  delete c2EBEE;

  cout << "NOW TEST mc vs fit: EBEB" << endl;
  myRDHtestEBEBs = "intWidthRDH_mass750_catEBEB_kpl015";
  myRDHtestEBEB = (RooDataHist*)fileWidth->Get(myRDHtestEBEBs);
  cout << "RDH EBEB" << endl;
  myRDHtestEBEB->Print();
  myRPDFtestEBEB = new RooHistPdf("myRPDFtestEBEB","myRPDFtestEBEB",*redMass,*myRDHtestEBEB,0) ;
  cout << "RPDF EBEB" << endl;
  myRPDFtestEBEB->Print();
  cout << endl;
  TCanvas *c2EBEBb = new TCanvas("c2EBEBb","c2EBEBb",1);
  RooPlot* myPlot2EBEBb = redMass->frame();
  muWidth->setVal(0.15);
  morphWidthCat0->plotOn(myPlot2EBEBb, LineColor(kBlue));
  myRPDFtestEBEB->plotOn(myPlot2EBEBb, LineColor(kRed));
  myPlot2EBEBb->Draw();
  c2EBEBb->SaveAs("checkMorphingPreChangeEBEB_vsDataK015.png");
  cout << endl;
  delete c2EBEBb;

  cout << "NOW TEST mc vs fit: EBEE" << endl;
  myRDHtestEBEEs = "intWidthRDH_mass750_catEBEE_kpl015";
  myRDHtestEBEE = (RooDataHist*)fileWidth->Get(myRDHtestEBEEs);
  cout << "RDH EBEE" << endl;
  myRDHtestEBEE->Print();
  myRPDFtestEBEE = new RooHistPdf("myRPDFtestEBEE","myRPDFtestEBEE",*redMass,*myRDHtestEBEE,0) ;
  cout << "RPDF EBEE" << endl;
  myRPDFtestEBEE->Print();
  cout << endl;
  TCanvas *c2EBEEb = new TCanvas("c2EBEEb","c2EBEEb",1);
  RooPlot* myPlot2EBEEb = redMass->frame();
  muWidth->setVal(0.15);
  morphWidthCat1->plotOn(myPlot2EBEEb, LineColor(kBlue));
  myRPDFtestEBEE->plotOn(myPlot2EBEEb, LineColor(kRed));
  myPlot2EBEEb->Draw();
  c2EBEEb->SaveAs("checkMorphingPreChangeEBEE_vsDataK015.png");
  cout << endl;
  delete c2EBEEb;

  cout << "NOW TEST mc vs fit: EBEB" << endl;
  myRDHtestEBEBs = "intWidthRDH_mass750_catEBEB_kpl025";
  myRDHtestEBEB = (RooDataHist*)fileWidth->Get(myRDHtestEBEBs);
  cout << "RDH EBEB" << endl;
  myRDHtestEBEB->Print();
  myRPDFtestEBEB = new RooHistPdf("myRPDFtestEBEB","myRPDFtestEBEB",*redMass,*myRDHtestEBEB,0) ;
  cout << "RPDF EBEB" << endl;
  myRPDFtestEBEB->Print();
  cout << endl;
  TCanvas *c2EBEBc = new TCanvas("c2EBEBc","c2EBEBc",1);
  RooPlot* myPlot2EBEBc = redMass->frame();
  muWidth->setVal(0.25);
  morphWidthCat0->plotOn(myPlot2EBEBc, LineColor(kBlue));
  myRPDFtestEBEB->plotOn(myPlot2EBEBc, LineColor(kRed));
  myPlot2EBEBc->Draw();
  c2EBEBc->SaveAs("checkMorphingPreChangeEBEB_vsDataK025.png");
  cout << endl;
  delete c2EBEBc;

  cout << "NOW TEST mc vs fit: EBEE" << endl;
  myRDHtestEBEEs = "intWidthRDH_mass750_catEBEE_kpl025";
  myRDHtestEBEE = (RooDataHist*)fileWidth->Get(myRDHtestEBEEs);
  cout << "RDH EBEE" << endl;
  myRDHtestEBEE->Print();
  myRPDFtestEBEE = new RooHistPdf("myRPDFtestEBEE","myRPDFtestEBEE",*redMass,*myRDHtestEBEE,0) ;
  cout << "RPDF EBEE" << endl;
  myRPDFtestEBEE->Print();
  cout << endl;
  TCanvas *c2EBEEc = new TCanvas("c2EBEEc","c2EBEEc",1);
  RooPlot* myPlot2EBEEc = redMass->frame();
  muWidth->setVal(0.25);
  morphWidthCat1->plotOn(myPlot2EBEEc, LineColor(kBlue));
  myRPDFtestEBEE->plotOn(myPlot2EBEEc, LineColor(kRed));
  myPlot2EBEEc->Draw();
  c2EBEEc->SaveAs("checkMorphingPreChangeEBEE_vsDataK025.png");
  cout << endl;
  delete c2EBEEc;


  cout << "NOW TEST2 evol: EBEB" << endl;
  TCanvas *c3EBEB = new TCanvas("c3EBEB","c3EBEB",1);
  RooPlot* myPlot3EBEB = redMass->frame();
  muWidth->setVal(0.01); morphWidthCat0->plotOn(myPlot3EBEB, LineColor(kBlue));
  muWidth->setVal(0.05); morphWidthCat0->plotOn(myPlot3EBEB, LineColor(kRed));
  muWidth->setVal(0.07); morphWidthCat0->plotOn(myPlot3EBEB, LineColor(kGreen));
  muWidth->setVal(0.1);  morphWidthCat0->plotOn(myPlot3EBEB, LineColor(kOrange));
  muWidth->setVal(0.15); morphWidthCat0->plotOn(myPlot3EBEB, LineColor(kViolet));
  muWidth->setVal(0.2);  morphWidthCat0->plotOn(myPlot3EBEB, LineColor(kYellow));
  muWidth->setVal(0.25); morphWidthCat0->plotOn(myPlot3EBEB, LineColor(kPink));
  muWidth->setVal(0.3);  morphWidthCat0->plotOn(myPlot3EBEB, LineColor(kBlack));
  myPlot3EBEB->Draw();
  c3EBEB->SaveAs("checkMorphingPreChangeEBEB_allCoupl.png");

  cout << "NOW TEST2 evol: EBEE" << endl;
  TCanvas *c3EBEE = new TCanvas("c3EBEE","c3EBEE",1);
  RooPlot* myPlot3EBEE = redMass->frame();
  muWidth->setVal(0.01); morphWidthCat1->plotOn(myPlot3EBEE, LineColor(kBlue));
  muWidth->setVal(0.05); morphWidthCat1->plotOn(myPlot3EBEE, LineColor(kRed));
  muWidth->setVal(0.07); morphWidthCat1->plotOn(myPlot3EBEE, LineColor(kGreen));
  muWidth->setVal(0.1);  morphWidthCat1->plotOn(myPlot3EBEE, LineColor(kOrange));
  muWidth->setVal(0.15); morphWidthCat1->plotOn(myPlot3EBEE, LineColor(kViolet));
  muWidth->setVal(0.2);  morphWidthCat1->plotOn(myPlot3EBEE, LineColor(kYellow));
  muWidth->setVal(0.25); morphWidthCat1->plotOn(myPlot3EBEE, LineColor(kPink));
  muWidth->setVal(0.3);  morphWidthCat1->plotOn(myPlot3EBEE, LineColor(kBlack));
  myPlot3EBEE->Draw();
  c3EBEE->SaveAs("checkMorphingPreChangeEBEE_allCoupl.png");

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

  RooRealVar* fix1d4 = new RooRealVar("fix1d4","fix1d4", 1.4e-4);
  fix1d4->setConstant(true);
  cout << "chiara: fix1d4" << endl;
  fix1d4->Print();
  cout << endl;

  RooRealVar* fix10000 = new RooRealVar("fix10000","fix10000", 10000);
  fix10000->setConstant(true);
  cout << "chiara: fix10000" << endl;
  fix10000->Print();
  cout << endl;

  RooProduct * relInvGammaHden = new RooProduct("relInvGammaHden","relInvGammaHden",RooArgList(*fix1d4,*muWidth,*muWidth,*fix10000));  // gamma/M
  cout << "chiara: relInvGammaHden" << endl;
  relInvGammaHden->Print();
  cout << endl;

  RooProduct * invGammaHden = new RooProduct("invGammaHden","invGammaHden",RooArgList(*mH,*relInvGammaHden));   // gamma
  cout << "chiara: invGammaHden" << endl;
  invGammaHden->Print();
  cout << endl;

  RooFormulaVar * invGammaH = new RooFormulaVar("invGammaH","1./@0",RooArgList(*invGammaHden));    // 1./gamma
  cout << "chiara: invGammaH" << endl;
  cout << endl;

  RooRealVar* fix0 = new RooRealVar("fix0","fix0", 0);
  fix0->setConstant(true);
  cout << "chiara: fix0" << endl;
  fix0->Print();
  cout << endl;

  RooLinearVar * linRedMass = new RooLinearVar("linRedMass","linRedMass",*deltaMgen,*invGammaH,*fix0);  
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

  // chiara debug: plot after fixing one reference k
  TCanvas *c4 = new TCanvas("c4","c4",1);
  RooPlot* myPlot4 = deltaMgen->frame();
  muWidth->setVal(0.05);
  reMorphCat0->plotOn(myPlot4, LineColor(kRed));
  reMorphCat1->plotOn(myPlot4, LineColor(kGreen));
  myPlot4->Draw();
  c4->SaveAs("checkMorphingAfterChange_005.png");
  cout << endl;
  delete c4;

  // chiara debug: new cout after fixing one reference coupling
  cout << "chiara: debug with k=0.1" << endl;
  muWidth->setVal(0.1);
  cout << "=> invGammaHden" << endl;
  invGammaHden->Print();
  cout << "=> invGammaH" << endl;
  invGammaH->Print();
  cout << "=> linRedMass" << endl;
  linRedMass->Print();
  cout << "=> reMorphCat0" << endl;
  reMorphCat0->Print();
  cout << "=> reMorphCat1" << endl;
  reMorphCat1->Print();
  cout << endl;
  cout << "chiara: debug with k=0.25" << endl;
  muWidth->setVal(0.25);
  cout << "=> invGammaHden" << endl;
  invGammaHden->Print();
  cout << "=> invGammaH" << endl;
  invGammaH->Print();
  cout << "=> linRedMass" << endl;
  linRedMass->Print();
  cout << "=> reMorphCat0" << endl;
  reMorphCat0->Print();
  cout << "=> reMorphCat1" << endl;
  reMorphCat1->Print();


  // Now we evaluate with a fine coupling scan samples and save the corresponding roodatahists  
  cout << endl;
  cout << endl;
  cout << "----------------------------" << endl;  
  cout << endl;
  cout << "now evaluating the morphing every 0.01, from 0.01 to 0.3: doing mass " << endl; 

  for (int iCoupl=4; iCoupl<30; iCoupl++) {      // chiaraaaaa
    float thisCoupl = 0.01 + iCoupl*0.01;        
    cout << "Fine scan: " << thisCoupl << endl;  
    muWidth->setVal(thisCoupl);          
    for (int c=0; c<NCAT; ++c) {  
      TString myCut = "EBEB";  
      if (c==1) myCut = "EBEE";  
      RooDataHist *fittWidthRDH;                      
      
      deltaMgen->setRange(-395.,395.); 
      deltaMgen->setBins(395); 
      //if (thisCoupl<0.04) {
      //deltaMgen->setRange(-10.,10.); 
      //deltaMgen->setBins(10); 
      //}

      if(c==0) fittWidthRDH = reMorphCat0->generateBinned(*deltaMgen,10000,kTRUE);   
      if(c==1) fittWidthRDH = reMorphCat1->generateBinned(*deltaMgen,10000,kTRUE);   
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
  
  for (int iCoupl=4; iCoupl<30; iCoupl++) {       // chiaraaaaa
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
