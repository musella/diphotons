#include "RooRealVar.h"
#include "RooAbsPdf.h"
#include "RooBinning.h"
#include "RooDataSet.h"
#include "RooDataHist.h"
#include "RooWorkspace.h"
#include "RooPlot.h"
#include "RooHistPdf.h"
#include "TFile.h"
#include "TTree.h"
#include "TChain.h"
#include "TMath.h"
#include "TROOT.h"
#include <iostream>

using namespace RooFit;
using namespace std;

// to be modified:
static const Int_t NCAT = 2;  

// Definition of the variables in the input ntuple
RooArgSet* defineVariables() {

  RooRealVar* mgg        = new RooRealVar("mgg",        "M(gg)",      300, 5050, "GeV");   
  RooRealVar* mggGen     = new RooRealVar("mggGen",     "M(gg) gen",  300, 5050, "GeV");   
  RooRealVar* eventClass = new RooRealVar("eventClass", "eventClass", -10,   10, "");
  RooRealVar* weight     = new RooRealVar("weight",     "weightings",   0, 1000, "");   

  RooArgSet* ntplVars = new RooArgSet(*mgg, *mggGen, *eventClass, *weight);                  
  
  return ntplVars;
}

// Loading signal data and making roodatasets
void AddSigData(RooWorkspace* w, int mass, TString coupling) {
  
  TString myMass = TString::Format("%d",mass);

  Int_t ncat = NCAT;
  
  // Variables
  RooArgSet* ntplVars = defineVariables();

  // Files
  TString inDir = "../macro/allFilesWithResolAtZ_rereco76x_2classes_v4/";          // chiara
  TChain* sigTree = new TChain();
  cout << "reading file " 
       << inDir+TString(Form("FormSigMod_kpl"))+coupling+TString(Form("_M%d.root/DiPhotonTree", mass)) << endl;
  sigTree->Add(inDir+TString(Form("FormSigMod_kpl"))+coupling+TString(Form("_M%d.root/DiPhotonTree", mass)));
  sigTree->SetTitle("sigTree");
  sigTree->SetName("sigTree");
  
  // Minimal common preselection cut on mgg and mggGen
  TString mainCut = TString::Format("mgg>=300 && mgg<=5050 && mggGen>=300 && mggGen<=5050");  
  RooDataSet sigWeighted("sigWeighted","dataset",sigTree,*ntplVars,mainCut,"weight");   
  sigWeighted.Print();

  // Split in categories 
  cout << endl;
  cout << "preparing dataset with observable mgg" << endl;
  RooDataSet* signal[NCAT];
  for (int c=0; c<ncat; ++c) {
    if (c==0) signal[c] = (RooDataSet*) sigWeighted.reduce(*w->var("mgg"),mainCut+TString::Format("&& eventClass==0"));
    if (c==1) signal[c] = (RooDataSet*) sigWeighted.reduce(*w->var("mgg"),mainCut+TString::Format("&& eventClass==1"));

    TString myCut;
    if (NCAT==2) {
      if (c==0) myCut = "EBEB";  
      if (c==1) myCut = "EBEE";
    } 
    w->import(*signal[c],Rename("SigWeight_cat"+myCut+"_mass"+myMass));
    
    cout << "cat " << c << ", signal[c]: " << endl;
    signal[c]->Print("v");
    cout << "---- for category " << c << ", nX for signal[c]:  " << signal[c]->sumEntries() << endl; 
    cout << endl;
  }

  // Saving the WS in a root file
  cout<< endl; 
  TString filename("/tmp/crovelli/myWSwithMorphing.root"); 
  TFile fileWs(filename,"RECREATE");
  fileWs.cd(); 
  w->writeToFile(filename);       
  cout << "Write signal workspace in: " << filename << " file" << endl;  
  cout << endl;  
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
  if (coupling=="01") {
    masses.push_back(750);
  } else if (coupling=="001") {
    masses.push_back(750);
  } else if (coupling=="02") {  
    masses.push_back(750);
  }

  // loading dat
  cout << endl; 
  cout << "------------------------------------------" << endl; 
  cout << endl; 
  cout << "Now add signal data" << endl;
  for (int ii=0; ii<(int)masses.size(); ii++) {
    int theMass = masses[ii];
    cout << "adding mass " << theMass << endl;
    AddSigData(w, theMass, coupling);   
  }

  return;
}

