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

// Preparing the resolution histograms
void MakeResolutionHisto8TeV(TString filename, bool newFile, int mass, int isExtra, TString coupling) {
  
  TString myMass = TString::Format("%d",mass);
  
  // the roorealvar 
  RooRealVar* deltaM = new RooRealVar("deltaM", "", -1000, 200, "GeV");   

  // Output file 
  TFile *theResoFile;
  if (newFile) theResoFile = new TFile(filename,"RECREATE");
  else theResoFile = new TFile(filename,"UPDATE");
  
  // Input file and tree
  TString inDir = "../macro/allFiles8TeV_smearing1perc/";
  TChain* sigTree = new TChain();
  cout << "reading file " 
       << inDir+TString(Form("FormSigMod_kpl"))+coupling+TString(Form("_M%d.root/DiPhotonTree", mass)) << endl;
  sigTree->Add(inDir+TString(Form("FormSigMod_kpl"))+coupling+TString(Form("_M%d.root/DiPhotonTree", mass)));
  sigTree->SetTitle("sigTree");
  sigTree->SetName("sigTree");

  // Minimal common preselection cut on mgg and mggGen
  TString mainCut = TString::Format("(mgg>=0 && mgg<=12000 && mggGen>=0 && mggGen<=12000");     

  // Histos
  TH1D *resolH = new TH1D("resolH","resolH",600,-1000.,200.);    // chiara
  resolH->Sumw2();
  
  // Projecting the tree into the histo
  if (!isExtra) {                              // chiara. This works for 3500->4000 only
    cout << "mass = " << mass << ", no correction" << endl; 
    sigTree->Project("resolH","mgg-mggGen",mainCut+TString::Format(")*puweight"));
  } else {
    if (mass!=3500) cout << "ERROR" << endl;
    cout << "mass = " << mass << ", we apply resolution scaling to go to 4000" << endl; 
    sigTree->Project("resolH","4000.*(mgg-mggGen)/3500.",mainCut+TString::Format(")*puweight"));
    mass = 4000;
    myMass = "4000";
  }

  // Now make the roodatahist
  RooDataHist resolRDH("resolRDH","resolRDH",*deltaM,Import(*resolH));        
  cout << "Resolution datahist summary for mass " << mass << endl;  
  resolRDH.Print();
  cout << endl;  

  // Saving in the root file
  theResoFile->cd();
  TString nameRDH = TString::Format("resolRDH_mass%d",mass);
  resolRDH.Write(nameRDH);
  
  // Plot to check
  TCanvas *c1 = new TCanvas("c1","c1",1);
  RooPlot* myPlot = deltaM->frame(Range(-1000,200),Bins(600));    
  resolRDH.plotOn(myPlot, LineColor(kRed), LineStyle(kDashed));    
  myPlot->Draw();
  TString canvasName = TString(Form("CheckResol_mass"+myMass+".png"));
  c1->SaveAs(canvasName);       
  c1->SetLogy();
  canvasName = TString(Form("CheckResol_mass"+myMass+"_LOG.png"));
  c1->SaveAs(canvasName);
  RooPlot* myPlotZoom = deltaM->frame(Range(-100,100),Bins(100));   
  resolRDH.plotOn(myPlotZoom, LineColor(kBlue), LineStyle(kDashed));    
  myPlotZoom->Draw();
  canvasName = TString(Form("CheckResolZoom_mass"+myMass+".png"));
  c1->SaveAs(canvasName);       
  
  delete c1;
  delete resolH;
  
  // Closing the output file
  theResoFile->Close();
}

//-------------------------------------------------------

// Interpolation of resolution functions 
void ResolInterpolation8TeV(RooWorkspace* w, vector<int> masses) {
  
  // Variable for resolution
  RooRealVar* deltaM = new RooRealVar("deltaM", "", -1000, 200, "GeV");   
  RooArgList varlist;
  varlist.add(*deltaM);

  // Morphing variable
  RooRealVar* muRes = new RooRealVar("muRes", "", 0, 12000, "GeV");

  // PDFs
  RooArgList pdfs;

  // Reference points
  int numMass = (int)masses.size();
  TVectorD paramVec(numMass); 

  // To plot
  RooPlot *frame = deltaM->frame();

  // Files with the roodatahists 
  TFile *fileRes = new TFile("ResolutionHistos.root");

  // Loop over masses
  for (int ii=0; ii<(int)masses.size(); ii++) { 
    int theMass = masses[ii];
    TString myMass = TString::Format("%d",theMass);

    // Reference points
    paramVec[ii] = (double)theMass; 

    cout << "Mass = " << myMass << endl; 

    // reading the roodatahist from the file
    TString myRDH = TString(Form("resolRDH_mass%d",theMass));
    RooDataHist *resRDH = (RooDataHist*)fileRes->Get(myRDH);
    resRDH->Print();
    cout << "Just got the resolution RooDataHist " << myRDH << endl;
    cout << endl;
    
    // moving to the corresponding roohistpdf
    TString myRHpdfName = TString(Form("resolRHPDF_mass%d",theMass));
    RooHistPdf *myHistPdfRes = new RooHistPdf("myHistPdfRes","myHistPdfRes",*deltaM,*resRDH,0) ;  
    myHistPdfRes->SetTitle(myRHpdfName);
    myHistPdfRes->SetName(myRHpdfName);
    myHistPdfRes->Print();
    cout << "Just got the resolution RooHistPdf " << myRHpdfName << endl;
    cout << endl;

    // adding to the list of pdfs
    pdfs.add(*myHistPdfRes);         
    cout << "RooHistPdfs added to the pdf list" << endl;
    cout << endl;
    
    // plot to check
    myHistPdfRes->plotOn(frame,LineColor(kBlue), LineStyle(kSolid));
  }   // loop over masses
  

  // Now morphing
  cout << endl;
  cout << "morphing for the resolution functions" << endl;
  pdfs.Print();
  RooMomentMorph *morphRes = new RooMomentMorph("morphRes","morphRes",*muRes,varlist,pdfs,paramVec,RooMomentMorph::Linear);
  morphRes->Print();
  cout << endl;
  cout << endl;
  
  // Close the input file
  fileRes->Close();
  

  // Now we evaluate with a fine mass scan samples and save the corresponding roodatahists
  cout << endl;
  cout << endl;
  cout << "----------------------------" << endl;
  cout << endl;
  cout << "now evaluating the morphing" << endl;


  // This is to have 2GeV steps between 500 and 1000 GeV
  for (int iGenMass=0; iGenMass<250; iGenMass++) {    
    int thisMass = 500 + iGenMass*2;
    cout << "Fine (2GeV) scan: " << thisMass << endl;    
    muRes->setVal(thisMass);
    RooDataHist *fittResolRDH; 
    deltaM->setBins(600);       // chiara
    fittResolRDH = morphRes->generateBinned(*deltaM,10000,kTRUE);
    fittResolRDH->Print();
    TString myFitRDH = TString(Form("resolRDH_mass%d",thisMass));
    fittResolRDH->SetTitle(myFitRDH);
    fittResolRDH->SetName(myFitRDH);
    w->import(*fittResolRDH);
  }

  // Then 4GeV steps between 1000 and 1600 GeV
  for (int iGenMass=0; iGenMass<150; iGenMass++) {    
    int thisMass = 1000 + iGenMass*4.;
    cout << "Medium (4GeV) scan: " << thisMass << endl;
    muRes->setVal(thisMass);
    RooDataHist *fittResolRDH; 
    deltaM->setBins(600);       // chiara
    fittResolRDH = morphRes->generateBinned(*deltaM,10000,kTRUE);
    fittResolRDH->Print();
    TString myFitRDH = TString(Form("resolRDH_mass%d",thisMass));
    fittResolRDH->SetTitle(myFitRDH);
    fittResolRDH->SetName(myFitRDH);
    w->import(*fittResolRDH);
  }

  // Then 100GeV steps between 1600 and 4000 GeV
  for (int iGenMass=0; iGenMass<25; iGenMass++) {    
    int thisMass = 1600 + iGenMass*100.;
    cout << "Coarse (100GeV) scan: " << thisMass << endl;
    muRes->setVal(thisMass);
    RooDataHist *fittResolRDH; 
    deltaM->setBins(600);       // chiara
    fittResolRDH = morphRes->generateBinned(*deltaM,10000,kTRUE);
    fittResolRDH->Print();
    TString myFitRDH = TString(Form("resolRDH_mass%d",thisMass));
    fittResolRDH->SetTitle(myFitRDH);
    fittResolRDH->SetName(myFitRDH);
    w->import(*fittResolRDH);
  }
  


  // Finally saving in a second rootfile
  cout << endl;
  cout << endl;
  cout << "----------------------------" << endl;
  cout << endl;
  cout << "Now salving the histos in a root file" << endl;
  TFile fileFittoRes("ResHistosGenOnlyScan.root","RECREATE");
  fileFittoRes.cd();
  
  for (int iGenMass=0; iGenMass<250; iGenMass++) {    
    int thisMass = 500 + iGenMass*2;
    TString myFitRDH = TString(Form("resolRDH_mass%d",thisMass));
    RooDataHist *RDH = (RooDataHist*)w->data(myFitRDH);
    RDH->Write();
  }
  for (int iGenMass=0; iGenMass<150; iGenMass++) {    
    int thisMass = 1000 + iGenMass*4.;
    TString myFitRDH = TString(Form("resolRDH_mass%d",thisMass));
    RooDataHist *RDH = (RooDataHist*)w->data(myFitRDH);
    RDH->Write();
  }
  for (int iGenMass=0; iGenMass<25; iGenMass++) {    
    int thisMass = 1600 + iGenMass*100.;
    TString myFitRDH = TString(Form("resolRDH_mass%d",thisMass));
    RooDataHist *RDH = (RooDataHist*)w->data(myFitRDH);
    RDH->Write();
  }
  
  fileFittoRes.Close();
}

// To run the analysis
void runfits() {

  RooWorkspace *w = new RooWorkspace("w");
 
  // range of masses - to be used to compute the resolution
  vector<int> masses;
  masses.push_back(750);
  masses.push_back(1000);
  masses.push_back(1250);
  masses.push_back(1500);
  masses.push_back(1750);
  masses.push_back(2000);
  masses.push_back(2250);
  masses.push_back(2500);
  masses.push_back(2750);
  masses.push_back(3000);
  masses.push_back(3250);
  masses.push_back(3500);
  masses.push_back(4000);    // chiara: new for paper. The sample does not exist. Use the 3500 file and fill wtih deltaM * 4000 / 3500 

  // make resolution histograms and roodatahists using k=0.01 or k=0.05 or k=0.1 according to the mass
  cout << endl; 
  cout << endl; 
  cout << "------------------------------------------" << endl; 
  cout << endl; 
  string fileResol = "ResolutionHistos.root";
  cout << "Now prepare resolution histograms and save in root file " << fileResol << endl;
  for (int ii=0; ii<(int)masses.size(); ii++) {
    int theMass = masses[ii];
    string myResKpl = "001";
    if (theMass==2750) myResKpl = "005";        
    if (theMass==3250) myResKpl = "01";  
    if (theMass==3500) myResKpl = "01";  
    if (theMass==4000) myResKpl = "01";  

    cout << "resolution at mass " << theMass << " with coupling " << myResKpl << endl;
    if (theMass!=4000) {
      if (ii==0) MakeResolutionHisto8TeV(fileResol, 1, theMass, 0, myResKpl);     
      else MakeResolutionHisto8TeV(fileResol, 0, theMass, 0, myResKpl);           
    }
    if (theMass==4000) {
      if (ii==0) MakeResolutionHisto8TeV(fileResol, 1, 3500, 1, myResKpl);  
      else MakeResolutionHisto8TeV(fileResol, 0, 3500, 1, myResKpl);        
    }
  } 

  // make the interpolation of the resolutions and save in another file the roodatahists
  cout << endl; 
  cout << endl; 
  cout << "--------------------------------------------------------------------------" << endl; 
  cout << endl;    
  cout << "Now make the interpolation" << endl; 
  ResolInterpolation8TeV(w, masses);

  return;
}

