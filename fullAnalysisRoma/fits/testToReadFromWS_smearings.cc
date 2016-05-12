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
#include "TLegend.h"

using namespace RooFit;
using namespace std;

void runfits() {

  TFile *fileWS = new TFile("dataset750_0nom_02.root");                  // obtained running with genOnly=0
  RooWorkspace *wfullsim = (RooWorkspace*) fileWS->Get("w");
  TFile *fileWSfineScan = new TFile("signalModel76x_02_500to998_0T_v6.root");    
  RooWorkspace *wfineScan = (RooWorkspace*) fileWSfineScan->Get("w");
  TFile *fileWSfineScanDown = new TFile("signalModel76x_02_500to998_0T_v6_smearDown.root");   
  RooWorkspace *wfineScanDown = (RooWorkspace*) fileWSfineScanDown->Get("w");
  TFile *fileWSfineScanUp = new TFile("signalModel76x_02_500to998_0T_v6_smearUp.root");   
  RooWorkspace *wfineScanUp = (RooWorkspace*) fileWSfineScanUp->Get("w");

  // RooRealvar
  RooRealVar *mgg = wfineScan->var("mgg");
  mgg->Print();
  cout << endl;

  // Convolution for a few masses
  RooAbsPdf *conv750EBEB     = wfineScan->pdf("Convolution_catEBEB_mass750_kpl02");
  RooAbsPdf *conv750EBEE     = wfineScan->pdf("Convolution_catEBEE_mass750_kpl02");
  RooAbsPdf *conv750EBEBdown = wfineScanDown->pdf("Convolution_catEBEB_mass750_kpl02");
  RooAbsPdf *conv750EBEEdown = wfineScanDown->pdf("Convolution_catEBEE_mass750_kpl02");
  RooAbsPdf *conv750EBEBup   = wfineScanUp->pdf("Convolution_catEBEB_mass750_kpl02");
  RooAbsPdf *conv750EBEEup   = wfineScanUp->pdf("Convolution_catEBEE_mass750_kpl02");

  // Dataset
  RooAbsData *dataset750EBEB = wfullsim->data("SigWeight_catEBEB_mass750");
  RooAbsData *dataset750EBEE = wfullsim->data("SigWeight_catEBEE_mass750");

  RooPlot *frame2Cat0 = mgg->frame(Range(700,800),Bins(200));
  TCanvas *c2 = new TCanvas("c2","c2",1);
  dataset750EBEB->plotOn(frame2Cat0, LineColor(kBlack), LineStyle(kSolid));
  conv750EBEB->plotOn(frame2Cat0, LineColor(kGreen), LineStyle(kDashed));
  conv750EBEBdown->plotOn(frame2Cat0, LineColor(kBlue), LineStyle(kDashed));
  conv750EBEBup->plotOn(frame2Cat0, LineColor(kRed), LineStyle(kDashed));
  frame2Cat0->GetXaxis()->SetTitle("m_{#gamma#gamma}");
  frame2Cat0->SetTitle("EBEB");

  // legend                                                                                                                            
  TLegend* legmc = new TLegend(710, 250000, 750, 300000, "", "");
  legmc->SetTextSize(0.0286044);
  legmc->SetTextFont(42);
  legmc->SetBorderSize(0);
  legmc->SetFillStyle(0);
  legmc->AddEntry(frame2Cat0->getObject(1),"nominal","LP");
  legmc->AddEntry(frame2Cat0->getObject(2),"smearing down","LP");
  legmc->AddEntry(frame2Cat0->getObject(3),"smearing up","LP");

  frame2Cat0->Draw();
  legmc->Draw("same");
  c2->SaveAs("checkSmear_0T_M750_02_EBEB.png");


  RooPlot *frame2Cat1 = mgg->frame(Range(700,800),Bins(200));
  TCanvas *c2b = new TCanvas("c2b","c2b",1);
  dataset750EBEE->plotOn(frame2Cat1, LineColor(kBlack), LineStyle(kSolid));
  conv750EBEE->plotOn(frame2Cat1, LineColor(kGreen), LineStyle(kDashed));
  conv750EBEEdown->plotOn(frame2Cat1, LineColor(kBlue), LineStyle(kDashed));
  conv750EBEEup->plotOn(frame2Cat1, LineColor(kRed), LineStyle(kDashed));
  frame2Cat1->GetXaxis()->SetTitle("m_{#gamma#gamma}");
  frame2Cat1->SetTitle("EBEE");
  frame2Cat1->Draw();
  legmc->Draw("same");
  c2b->SaveAs("checkSmear_0T_M750_02_EBEE.png");

  return;
}

