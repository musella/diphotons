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

  TFile *fileWS = new TFile("dataset750_0nom_02.root");                  // obtained running with genOnly=0
  RooWorkspace *wfullsim = (RooWorkspace*) fileWS->Get("w");
  TFile *fileWSfineScan = new TFile("signalModel76x_02_500to998_0T_v6.root");     // 750 included in the morphing 
  RooWorkspace *wfineScan = (RooWorkspace*) fileWSfineScan->Get("w");
 
  // RooRealvar
  RooRealVar *mgg = wfineScan->var("mgg");
  mgg->Print();
  cout << endl;

  // Convolution for a few masses
  RooAbsPdf *conv750EBEB  = wfineScan->pdf("Convolution_catEBEB_mass750_kpl02");
  RooAbsPdf *conv750EBEE  = wfineScan->pdf("Convolution_catEBEE_mass750_kpl02");
  RooAbsPdf *conv748EBEB  = wfineScan->pdf("Convolution_catEBEB_mass748_kpl02");
  RooAbsPdf *conv748EBEE  = wfineScan->pdf("Convolution_catEBEE_mass748_kpl02");
  RooAbsPdf *conv746EBEB  = wfineScan->pdf("Convolution_catEBEB_mass746_kpl02");
  RooAbsPdf *conv746EBEE  = wfineScan->pdf("Convolution_catEBEE_mass746_kpl02");
  RooAbsPdf *conv752EBEB  = wfineScan->pdf("Convolution_catEBEB_mass752_kpl02");
  RooAbsPdf *conv752EBEE  = wfineScan->pdf("Convolution_catEBEE_mass752_kpl02");
  RooAbsPdf *conv754EBEB  = wfineScan->pdf("Convolution_catEBEB_mass754_kpl02");
  RooAbsPdf *conv754EBEE  = wfineScan->pdf("Convolution_catEBEE_mass754_kpl02");

  // Dataset
  RooAbsData *dataset750EBEB = wfullsim->data("SigWeight_catEBEB_mass750");
  RooAbsData *dataset750EBEE = wfullsim->data("SigWeight_catEBEE_mass750");

  RooPlot *frame2Cat0 = mgg->frame(Range(700,800),Bins(200));
  TCanvas *c2 = new TCanvas("c2","c2",1);
  dataset750EBEB->plotOn(frame2Cat0, LineColor(kRed), LineStyle(kSolid));
  conv750EBEB->plotOn(frame2Cat0, LineColor(kGreen), LineStyle(kDashed));
  frame2Cat0->GetXaxis()->SetTitle("m_{#gamma#gamma}");
  frame2Cat0->SetTitle("EBEB");
  frame2Cat0->Draw();
  c2->SaveAs("testCatBis0off.png");
  c2->SaveAs("testCatBis0off.root");

  RooPlot *frame2Cat1 = mgg->frame(Range(700,800),Bins(200));
  TCanvas *c2b = new TCanvas("c2b","c2b",1);
  dataset750EBEE->plotOn(frame2Cat1, LineColor(kRed), LineStyle(kSolid));
  conv750EBEE->plotOn(frame2Cat1, LineColor(kGreen), LineStyle(kDashed));
  frame2Cat1->GetXaxis()->SetTitle("m_{#gamma#gamma}");
  frame2Cat1->SetTitle("EBEE");
  frame2Cat1->Draw();
  c2b->SaveAs("testCatBis1off.png");
  c2b->SaveAs("testCatBis1off.root");

  RooPlot *frame22Cat0 = mgg->frame(Range(700,800),Bins(200));
  TCanvas *c22 = new TCanvas("c22","c22",1);
  conv750EBEB->plotOn(frame22Cat0, LineColor(kGreen), LineStyle(kDashed));
  conv748EBEB->plotOn(frame22Cat0, LineColor(kBlue), LineStyle(kDashed));
  conv746EBEB->plotOn(frame22Cat0, LineColor(kViolet), LineStyle(kDashed));
  conv752EBEB->plotOn(frame22Cat0, LineColor(kOrange), LineStyle(kDashed));
  conv754EBEB->plotOn(frame22Cat0, LineColor(kRed), LineStyle(kDashed));
  frame22Cat0->GetXaxis()->SetTitle("m_{#gamma#gamma}");
  frame22Cat0->SetTitle("Convolution vs data");
  frame22Cat0->Draw();
  c22->SaveAs("testCatBis0off_convOnly.png");
  c22->SaveAs("testCatBis0off_convOnly.root");

  RooPlot *frame22Cat1 = mgg->frame(Range(700,800),Bins(200));
  TCanvas *c22b = new TCanvas("c22b","c22b",1);
  conv750EBEE->plotOn(frame22Cat1, LineColor(kGreen), LineStyle(kDashed));
  conv748EBEE->plotOn(frame22Cat1, LineColor(kBlue), LineStyle(kDashed));
  conv746EBEE->plotOn(frame22Cat1, LineColor(kViolet), LineStyle(kDashed));
  conv752EBEE->plotOn(frame22Cat1, LineColor(kOrange), LineStyle(kDashed));
  conv754EBEE->plotOn(frame22Cat1, LineColor(kRed), LineStyle(kDashed));
  frame22Cat1->GetXaxis()->SetTitle("m_{#gamma#gamma}");
  frame22Cat1->SetTitle("Convolution vs data");
  frame22Cat1->Draw();
  c22b->SaveAs("testCatBis1off_convOnly.png");
  c22b->SaveAs("testCatBis1off_convOnly.root");

  return;
}

