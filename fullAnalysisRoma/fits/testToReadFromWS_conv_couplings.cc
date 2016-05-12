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

  TFile *p001 = new TFile("/tmp/crovelli/signalModel76x_001_1000to4900_0T.root");
  TFile *p01  = new TFile("/tmp/crovelli/signalModel76x_01_1000to4900_0T.root");
  TFile *p02  = new TFile("/tmp/crovelli/signalModel76x_02_1000to4900_0T.root");

  RooWorkspace *w001 = (RooWorkspace*) p001->Get("w");
  RooWorkspace *w01  = (RooWorkspace*) p01->Get("w");
  RooWorkspace *w02  = (RooWorkspace*) p02->Get("w");
  
  RooRealVar *mgg = w001->var("mgg");

  // Convolution for different recos
  RooAbsPdf *conv2500EBEB_001 = w001->pdf("Convolution_catEBEB_mass2500_kpl001");
  RooAbsPdf *conv2500EBEB_01  = w01->pdf("Convolution_catEBEB_mass2500_kpl01");
  RooAbsPdf *conv2500EBEB_02  = w02->pdf("Convolution_catEBEB_mass2500_kpl02");

  // plot
  RooPlot *frameA = mgg->frame(Range(2200,2800),Bins(100));
  TCanvas *c1 = new TCanvas("c1","c1",1);
  conv2500EBEB_001->plotOn(frameA, LineColor(kRed));
  conv2500EBEB_01->plotOn(frameA, LineColor(kBlue));
  conv2500EBEB_02->plotOn(frameA, LineColor(kGreen));
  frameA->Draw();
  double max = frameA->GetMaximum();
  frameA->GetYaxis()->SetRangeUser(0., max*1.05);
  frameA->GetXaxis()->SetTitle("m_{#gamma#gamma}");
  frameA->SetTitle("Width x Resolution");

  frameA->Draw();
  c1->SaveAs("test.png");

  return;
}

