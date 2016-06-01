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
#include "TLegend.h"
#include "TChain.h"
#include "TMath.h"
#include "TROOT.h"
#include "TVectorD.h"
#include <iostream>
#include "TCanvas.h"

using namespace RooFit;
using namespace std;

void runfits() {

  TFile *p38 = new TFile("signalModel76x_02_500to998__resolv4.root");
  TFile *p0  = new TFile("signalModel76x_02_500to998_0T_v6.root");

  RooWorkspace *w38 = (RooWorkspace*) p38->Get("w");
  RooWorkspace *w0  = (RooWorkspace*) p0->Get("w");
  
  RooRealVar *mgg = w38->var("mgg");

  // Convolution for different recos
  RooAbsPdf *conv750EBEE_38 = w38->pdf("Convolution_catEBEE_mass750_kpl02");
  RooAbsPdf *conv750EBEE_0  = w0->pdf("Convolution_catEBEE_mass750_kpl02");

  // plot
  RooPlot *frameA = mgg->frame(Range(700,800),Bins(100));
  TCanvas *c1 = new TCanvas("c1","c1",1);
  conv750EBEE_38->plotOn(frameA, LineColor(kRed));
  conv750EBEE_0->plotOn(frameA, LineColor(kBlue));
  frameA->Draw();
  double max = frameA->GetMaximum();
  //frameA->GetYaxis()->SetRangeUser(0., max*1.05);
  frameA->GetYaxis()->SetRangeUser(0., 0.055);
  frameA->GetXaxis()->SetTitle("m_{#gamma#gamma}");
  frameA->SetTitle("EBEE");

  // legend
  TLegend* legmc = new TLegend(710, 0.035, 750, 0.04, "", "");
  legmc->SetTextSize(0.0286044);
  legmc->SetTextFont(42);
  legmc->SetBorderSize(0);
  legmc->SetFillStyle(0);
  legmc->AddEntry(frameA->getObject(0),"B=3.8T","LP");
  legmc->AddEntry(frameA->getObject(1),"B=0T","LP");

  frameA->Draw();
  legmc->Draw("same");
  c1->SaveAs("test.png");

  return;
}

