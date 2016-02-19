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

  TFile *pas  = new TFile("nominalWSwithSmear_k001_m500to998.root");
  TFile *rr4x = new TFile("newFiles/convolution_700to800/rerecoResol/myWSwithMorphing_001_700to800.root");
  TFile *pfix = new TFile("newFiles/convolution_700to800/fixedPromptResol/myWSwithMorphing_001_700to800.root");
  
  RooWorkspace *wpas  = (RooWorkspace*) pas->Get("w");
  RooWorkspace *wrr4x = (RooWorkspace*) rr4x->Get("w");
  RooWorkspace *wpfix = (RooWorkspace*) pfix->Get("w");
  
  RooRealVar *mgg = wpas->var("mgg");

  // Convolution for different recos
  RooAbsPdf *conv750EBEE_pas  = wpas->pdf("Convolution_catEBEE_mass750_kpl001");
  RooAbsPdf *conv750EBEE_rr4x = wrr4x->pdf("Convolution_catEBEE_mass750_kpl001");
  RooAbsPdf *conv750EBEE_pfix = wpfix->pdf("Convolution_catEBEE_mass750_kpl001");

  // plot
  RooPlot *frameA = mgg->frame(Range(700,800),Bins(100));
  TCanvas *c1 = new TCanvas("c1","c1",1);
  conv750EBEE_pas->plotOn(frameA, LineColor(kYellow));
  conv750EBEE_rr4x->plotOn(frameA, LineColor(kRed));
  conv750EBEE_pfix->plotOn(frameA, LineColor(kViolet));
  frameA->Draw();
  double max = frameA->GetMaximum();
  frameA->GetYaxis()->SetRangeUser(0., max*1.05);
  frameA->GetXaxis()->SetTitle("m_{#gamma#gamma}");
  frameA->SetTitle("Width x Resolution");

  // legend
  TLegend* legmc = new TLegend(770, 0.02, 790, 0.035, "", "");
  legmc->SetTextSize(0.0286044);
  legmc->SetTextFont(42);
  legmc->SetBorderSize(0);
  legmc->SetFillStyle(0);
  legmc->AddEntry(frameA->getObject(0),"pas","LP");
  legmc->AddEntry(frameA->getObject(1),"74x rereco","LP");
  legmc->AddEntry(frameA->getObject(2),"check","LP");

  frameA->Draw();
  legmc->Draw("same");
  c1->SaveAs("test.png");

  return;
}

