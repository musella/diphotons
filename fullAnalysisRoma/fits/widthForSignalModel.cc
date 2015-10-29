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
  if (coupling=="01")  deltaMgen = new RooRealVar("deltaMgen", "", -1000, 1000, "GeV");
  if (coupling=="02")  deltaMgen = new RooRealVar("deltaMgen", "", -2500, 2500, "GeV"); 

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

//-------------------------------------------------------
void WidthInterpolation(RooWorkspace* w, vector<int> masses, string coupling, bool newfile) {  
  
  // Variable for the intrinsic width
  RooRealVar* deltaMgen;   // chiara
  if (coupling=="001") deltaMgen = new RooRealVar("deltaMgen", "",   -70, 70,   "GeV"); 
  if (coupling=="01")  deltaMgen = new RooRealVar("deltaMgen", "", -1000, 1000, "GeV");
  if (coupling=="02")  deltaMgen = new RooRealVar("deltaMgen", "", -2500, 2500, "GeV"); 
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
  if (coupling=="01")  frameCat0 = deltaMgen->frame(Range(-1000,1000),Bins(1000));    
  if (coupling=="02")  frameCat0 = deltaMgen->frame(Range(-2500,2500),Bins(2500));    
  if (coupling=="001") frameCat1 = deltaMgen->frame(Range(  -70,70), Bins(70));    
  if (coupling=="01")  frameCat1 = deltaMgen->frame(Range(-1000,1000),Bins(1000));    
  if (coupling=="02")  frameCat1 = deltaMgen->frame(Range(-2500,2500),Bins(2500));    
  
  // Files with the roodatahists
  TFile *fileWidth = new TFile("IntrinsicWidthHistos.root"); 

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
  for (int iGenMass=0; iGenMass<91; iGenMass++) {   
    int thisMass = 500 + iGenMass*50;  
    cout << thisMass << endl;  
    muWidth->setVal(thisMass);
    for (int c=0; c<NCAT; ++c) {  
      TString myCut = "EBEB";  
      if (c==1) myCut = "EBEE";  
      RooDataHist *fittWidthRDH; 
      if (coupling=="001") deltaMgen->setBins(70);
      if (coupling=="01")  deltaMgen->setBins(1000);
      if (coupling=="02")  deltaMgen->setBins(2500);
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
  for (int iGenMass=0; iGenMass<91; iGenMass++) {   
    int thisMass = 500 + iGenMass*50;  
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

void controlPlots(string coupling) {

  // the roorealvar
  RooRealVar* deltaMgen;   // chiara                                                                                                                                         
  if (coupling=="001") deltaMgen = new RooRealVar("deltaMgen", "",   -70, 70,   "GeV");
  if (coupling=="01")  deltaMgen = new RooRealVar("deltaMgen", "", -1000, 1000, "GeV");
  if (coupling=="02")  deltaMgen = new RooRealVar("deltaMgen", "", -2500, 2500, "GeV");

  // Files with the roodatahists
  TFile *fileFull  = new TFile("IntrinsicWidthHistos.root");
  TFile *fileMorph = new TFile("WidthHistosGenOnlyScan.root");

  // Some example mass - full
  RooDataHist *full4000_catEBEB, *full4000_catEBEE; 
  if (coupling=="001") full4000_catEBEB = (RooDataHist*)fileFull->Get("intWidthRDH_mass4000_catEBEB_kpl001");  
  if (coupling=="01")  full4000_catEBEB = (RooDataHist*)fileFull->Get("intWidthRDH_mass4000_catEBEB_kpl01");  
  if (coupling=="02")  full4000_catEBEB = (RooDataHist*)fileFull->Get("intWidthRDH_mass4000_catEBEB_kpl02");  
  if (coupling=="001") full4000_catEBEE = (RooDataHist*)fileFull->Get("intWidthRDH_mass4000_catEBEE_kpl001");  
  if (coupling=="01")  full4000_catEBEE = (RooDataHist*)fileFull->Get("intWidthRDH_mass4000_catEBEE_kpl01");  
  if (coupling=="02")  full4000_catEBEE = (RooDataHist*)fileFull->Get("intWidthRDH_mass4000_catEBEE_kpl02");  
  RooHistPdf *full4000_catEBEB_pdf = new RooHistPdf("full4000_catEBEB_pdf", "full4000_catEBEB_pdf", *deltaMgen,*full4000_catEBEB,0);
  RooHistPdf *full4000_catEBEE_pdf = new RooHistPdf("full4000_catEBEE_pdf", "full4000_catEBEE_pdf", *deltaMgen,*full4000_catEBEE,0);

  // Some example mass - morphing  
  RooDataHist *morph4000_catEBEB, *morph4000_catEBEE;
  if (coupling=="001") morph4000_catEBEB = (RooDataHist*)fileMorph->Get("widthRDH_mass4000_catEBEB_kpl001");  
  if (coupling=="01")  morph4000_catEBEB = (RooDataHist*)fileMorph->Get("widthRDH_mass4000_catEBEB_kpl01");  
  if (coupling=="02")  morph4000_catEBEB = (RooDataHist*)fileMorph->Get("widthRDH_mass4000_catEBEB_kpl02");  
  if (coupling=="001") morph4000_catEBEE = (RooDataHist*)fileMorph->Get("widthRDH_mass4000_catEBEE_kpl001");  
  if (coupling=="01")  morph4000_catEBEE = (RooDataHist*)fileMorph->Get("widthRDH_mass4000_catEBEE_kpl01");  
  if (coupling=="02")  morph4000_catEBEE = (RooDataHist*)fileMorph->Get("widthRDH_mass4000_catEBEE_kpl02");  
  RooHistPdf *morph4000_catEBEB_pdf = new RooHistPdf("morph4000_catEBEB_pdf","morph4000_catEBEB_pdf",*deltaMgen,*morph4000_catEBEB,0) ;
  RooHistPdf *morph4000_catEBEE_pdf = new RooHistPdf("morph4000_catEBEE_pdf","morph4000_catEBEE_pdf",*deltaMgen,*morph4000_catEBEE,0) ;

  // check EBEB                                                   
  TCanvas *c1 = new TCanvas("c1","c1",1);
  RooPlot* myPlot; //chiara
  if (coupling=="001") myPlot = deltaMgen->frame(Range(  -70,70), Bins(70));    
  if (coupling=="01")  myPlot = deltaMgen->frame(Range(-1000,1000),Bins(1000));    
  if (coupling=="02")  myPlot = deltaMgen->frame(Range(-2500,2500),Bins(2500));    
  RooPlot* myPlotZoom = deltaMgen->frame(Range(-300,300),Bins(300));  
  if (coupling=="001") myPlotZoom = deltaMgen->frame(Range(-12,12),Bins(12));
  if (coupling=="02")  myPlotZoom = deltaMgen->frame(Range(-500,500),Bins(500));
  myPlot->SetTitle("mG=4000");
  myPlotZoom->SetTitle("mG=4000");
  morph4000_catEBEB_pdf->plotOn(myPlot, LineColor(kRed), LineStyle(kDashed));
  full4000_catEBEB_pdf->plotOn(myPlot, LineColor(kBlue), LineStyle(kSolid));
  myPlot->Draw();
  TString canvasName = TString(Form("CheckMorphing_CatEBEB_mass4000.png"));
  c1->SaveAs(canvasName);
  c1->SetLogy();
  canvasName = TString(Form("CheckMorphing_CatEBEB_mass4000_Log.png"));
  c1->SaveAs(canvasName);
  morph4000_catEBEB_pdf->plotOn(myPlotZoom, LineColor(kRed), LineStyle(kDashed));
  full4000_catEBEB_pdf->plotOn(myPlotZoom, LineColor(kBlue), LineStyle(kSolid));
  myPlotZoom->Draw();
  canvasName = TString(Form("CheckMorphing_CatEBEB_mass4000_Zoom.png"));
  c1->SaveAs(canvasName);

  // check EBEE                                                                                                                                                              
  TCanvas *c11 = new TCanvas("c11","c11",1);
  RooPlot* myPlotB; //chiara
  if (coupling=="001") myPlotB = deltaMgen->frame(Range(  -70,70), Bins(70));    
  if (coupling=="01")  myPlotB = deltaMgen->frame(Range(-1000,1000),Bins(1000));    
  if (coupling=="02")  myPlotB = deltaMgen->frame(Range(-2500,2500),Bins(2500));    
  RooPlot* myPlotZoomB = deltaMgen->frame(Range(-300,300),Bins(300));  
  if (coupling=="001") myPlotZoomB = deltaMgen->frame(Range(-12,12),Bins(12));
  if (coupling=="02")  myPlotZoomB = deltaMgen->frame(Range(-500,500),Bins(500));
  myPlotB->SetTitle("mG=4000");
  myPlotZoomB->SetTitle("mG=4000");
  morph4000_catEBEE_pdf->plotOn(myPlotB, LineColor(kRed), LineStyle(kDashed));
  full4000_catEBEE_pdf->plotOn(myPlotB, LineColor(kBlue), LineStyle(kSolid));
  myPlotB->Draw();
  canvasName = TString(Form("CheckMorphing_CatEBEE_mass4000.png"));
  c11->SaveAs(canvasName);
  c11->SetLogy();
  canvasName = TString(Form("CheckMorphing_CatEBEE_mass4000_Log.png"));
  c11->SaveAs(canvasName);
  morph4000_catEBEE_pdf->plotOn(myPlotZoomB, LineColor(kRed), LineStyle(kDashed));
  full4000_catEBEE_pdf->plotOn(myPlotZoomB, LineColor(kBlue), LineStyle(kSolid));
  myPlotZoomB->Draw();
  canvasName = TString(Form("CheckMorphing_CatEBEE_mass4000_Zoom.png"));
  c11->SaveAs(canvasName);
}


// To run the analysis
void runfits() {

  RooWorkspace *w = new RooWorkspace("w");    

  for (int iCoupling=0; iCoupling<3; iCoupling++) {

    if (iCoupling!=0) continue;           // chiara!!!

    string coupling;
    if (iCoupling==0) coupling="001";
    if (iCoupling==1) coupling="01";
    if (iCoupling==2) coupling="02";
    cout << "running for coupling " << iCoupling << ", " << coupling << endl;

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
      if (coupling=="01") {  
	masses.push_back(500);
	masses.push_back(625);
	masses.push_back(750);
	masses.push_back(875);
	masses.push_back(1000);
	masses.push_back(1125);
	masses.push_back(1250);
	masses.push_back(1375);
	masses.push_back(1500);
	masses.push_back(1625);
	masses.push_back(1750);
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
	masses.push_back(3625);
	masses.push_back(3750);
	masses.push_back(3875);
	masses.push_back(4000);
	masses.push_back(4125);
	masses.push_back(4250);
	masses.push_back(4375);
	masses.push_back(4500);
	masses.push_back(4625);
	masses.push_back(4750);
	masses.push_back(4875);
	masses.push_back(5000);
      } else if (coupling=="001") {  
	masses.push_back(500);
	masses.push_back(625);
	masses.push_back(750);
	masses.push_back(875);
	masses.push_back(1000);
	masses.push_back(1125);
	masses.push_back(1250);
	masses.push_back(1375);
	masses.push_back(1500);
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
	masses.push_back(3625);
	masses.push_back(3750);
	masses.push_back(3875);
	//	masses.push_back(4000);
	masses.push_back(4125);
	masses.push_back(4250);
	masses.push_back(4500);
	masses.push_back(4625);
	masses.push_back(4750);
	masses.push_back(5000);
      } else if (coupling=="02") {  
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

    // make the interpolation of the resolutions and save in another file the roodatahists 
    cout << endl; 
    cout << endl; 
    cout << "--------------------------------------------------------------------------" << endl; 
    cout << endl; 
    cout << "Now make the interpolation" << endl; 
    if (iCoupling==0) WidthInterpolation(w, masses, coupling, 1); 
    else WidthInterpolation(w, masses, coupling, 0); 

    // control plots
    cout << endl; 
    cout << endl; 
    cout << "--------------------------------------------------------------------------" << endl; 
    cout << endl; 
    cout << "Preparing control plots" << endl; 
    controlPlots(coupling);

  } // loop over couplings

  return;
}

