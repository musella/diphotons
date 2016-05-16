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

void runfits() {

  // To be changed
  float coupling = 0.2;
  int mass = 760;

  // cutting in +- 4Gamma
  float rangeInf = mass - 4. * 1.4 * coupling * coupling * mass; 
  float rangeSup = mass + 4. * 1.4 * coupling * coupling * mass; 
  int bins = (int)( (rangeSup-rangeInf)/4. );    
  if (mass<1500 && coupling>0.195 && coupling<0.205)               bins = (int)( (rangeSup-rangeInf)/4. );
  if (mass<3000 && mass>=1500 && coupling>0.195 && coupling<0.205) bins = (int)( (rangeSup-rangeInf)/5. );
  if (mass>=3000 && coupling>0.195 && coupling<0.205)              bins = (int)( (rangeSup-rangeInf)/8. );
  if (mass<1500 && coupling>0.095 && coupling<0.105)               bins = (int)( (rangeSup-rangeInf)/3. );
  if (mass>=1500 && coupling>0.095 && coupling<0.105)              bins = (int)( (rangeSup-rangeInf)/4. );
  cout << "rangeInf: " << rangeInf << ", rangeSup: " << rangeSup << ", bins = " << bins << endl;
  

  // Files: dataset
  //TFile *fileWS = new TFile("myWSwithMorphing02__withDatasetForAllOldMassPoints.root");     
  TFile *fileWS = new TFile("myWSwithMorphing02__withDatasetForNewMassPoints.root");     
  RooWorkspace *wfullsim = (RooWorkspace*) fileWS->Get("w");

  // Files: moriond model
  TFile *fileWSfineScan;
  if (mass<1000) fileWSfineScan = new TFile("signalModel76x_02_500to998__resolv4__MOR.root");
  else fileWSfineScan = new TFile("signalModel76x_02_1000to4900__resolv4__MOR.root");
  RooWorkspace *wfineScan = (RooWorkspace*) fileWSfineScan->Get("w");

  // Files: new model
  TFile *fileWSfineScanNew;
  if (mass<1000) fileWSfineScanNew = new TFile("myWSwithMorphing_02_500to998__NEW.root");
  else fileWSfineScanNew = new TFile("myWSwithMorphing_02_1000to5000__NEW.root");
  RooWorkspace *wfineScanNew = (RooWorkspace*) fileWSfineScanNew->Get("w");

 
  // RooRealvar
  RooRealVar *mgg = wfineScan->var("mgg");
  mgg->Print();
  cout << endl;
  mgg->setRange(rangeInf,rangeSup);                // per test su tutto il range commento questo
  mgg->Print();
  cout << endl;

  // Convolution for a few masses: moriond
  TString myMorPdf;
  if (coupling>0.009 && coupling<0.011)  myMorPdf = TString::Format("Convolution_catEBEB_mass%d_kpl001",mass);
  if (coupling>0.095 && coupling<0.105)  myMorPdf = TString::Format("Convolution_catEBEB_mass%d_kpl01",mass);
  if (coupling>0.195 && coupling<0.205)  myMorPdf = TString::Format("Convolution_catEBEB_mass%d_kpl02",mass); 
  cout << "Reading PDF " << myMorPdf << endl;
  RooAbsPdf *convEBEB = wfineScan->pdf(myMorPdf);
  cout << endl;

  // Convolution for a few masses: new
  TString myNewPdf;
  if (coupling>0.009 && coupling<0.011)  myNewPdf = TString::Format("Convolution_catEBEB_mass%d_kpl001",mass);
  if (coupling>0.095 && coupling<0.105)  myNewPdf = TString::Format("Convolution_catEBEB_mass%d_kpl01",mass);
  if (coupling>0.195 && coupling<0.205)  myNewPdf = TString::Format("Convolution_catEBEB_mass%d_kpl02",mass); 
  cout << "Reading New PDF " << myNewPdf << endl;
  RooAbsPdf *convEBEBnew = wfineScanNew->pdf(myNewPdf);
  cout << endl;

  // Dataset
  TString myDataset = TString::Format("SigWeight_catEBEB_mass%d",mass);
  // per test su tutto il range commento da qui
  RooAbsData *datasetEBEBbef = wfullsim->data(myDataset);
  cout << "Reading dataset " << myDataset << endl;
  TString myCut = TString::Format("mgg>=%f",rangeInf) + TString::Format("&& mgg<=%f",rangeSup);
  cout << "cutting in " << myCut << endl;
  RooAbsData *datasetEBEB = datasetEBEBbef->reduce(*mgg,myCut);
  // fino a qui. E scommento la linea sotto
  // RooAbsData *datasetEBEB = wfullsim->data(myDataset);

  // Plot
  RooPlot *frame2Cat0 = mgg->frame(Range(rangeInf,rangeSup),Bins(bins));
  //RooPlot *frame2Cat0 = mgg->frame(Range(1000,2000),Bins(bins));
  TCanvas *c2 = new TCanvas("c2","c2",1);
  datasetEBEB->plotOn(frame2Cat0);
  mgg->setRange("normRange",rangeInf,rangeSup);
  convEBEB->plotOn(frame2Cat0, LineColor(kBlue), LineStyle(kDashed));
  //convEBEBnew->plotOn(frame2Cat0, LineColor(kYellow), LineStyle(kDashed));
  convEBEB->plotOn(frame2Cat0, LineColor(kGreen), LineStyle(kDashed), NormRange("normRange"));
  //convEBEBnew->plotOn(frame2Cat0, LineColor(kRed), LineStyle(kDashed), NormRange("normRange"));

  frame2Cat0->GetXaxis()->SetTitle("m_{#gamma#gamma}");
  frame2Cat0->SetTitle("EBEB");
  //c2->SetLogy();
  frame2Cat0->Draw();
  c2->SaveAs("test_morOnly_normOrNot.png");
  

  return;
}

