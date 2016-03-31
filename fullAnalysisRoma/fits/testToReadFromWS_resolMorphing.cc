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

  TFile *fileResNominal     = new TFile("daTenereVersioneFinale_smearings/ResolutionHistos_nominalSmearings.root");
  TFile *filePointFromMorph = new TFile("daTenereVersioneFinale_smearings/ResHistosGenOnlyScan_nominalSmearings.root");

  RooRealVar* deltaM = new RooRealVar("deltaM", "",   -10000, 10000,   "GeV");

  // Comparing couplings
  RooDataHist *resolRDH_mass1000_catEBEE_nom   = (RooDataHist*)fileResNominal->Get("resolRDH_mass1000_catEBEE");
  RooDataHist *resolRDH_mass1000_catEBEE_morph = (RooDataHist*)filePointFromMorph->Get("resolRDH_mass1000_catEBEE");
  RooDataHist *resolRDH_mass996_catEBEE_morph  = (RooDataHist*)filePointFromMorph->Get("resolRDH_mass996_catEBEE");
  RooDataHist *resolRDH_mass992_catEBEE_morph  = (RooDataHist*)filePointFromMorph->Get("resolRDH_mass992_catEBEE");
  RooDataHist *resolRDH_mass1004_catEBEE_morph = (RooDataHist*)filePointFromMorph->Get("resolRDH_mass1004_catEBEE");
  RooDataHist *resolRDH_mass1008_catEBEE_morph = (RooDataHist*)filePointFromMorph->Get("resolRDH_mass1008_catEBEE");
  RooDataHist *resolRDH_mass1500_catEBEE_morph = (RooDataHist*)filePointFromMorph->Get("resolRDH_mass1500_catEBEE");

  RooPlot *frameA = deltaM->frame(Range(-100,100),Bins(50));
  TCanvas *c1 = new TCanvas("c1","c1",1);
  resolRDH_mass1000_catEBEE_nom->plotOn(frameA, LineColor(kBlack), MarkerStyle(kSolid), MarkerSize(3),Rescale(1./resolRDH_mass1000_catEBEE_nom->sumEntries()));
  resolRDH_mass1000_catEBEE_morph->plotOn(frameA, LineColor(kRed), MarkerStyle(kDashed), MarkerSize(3),Rescale(1./resolRDH_mass1000_catEBEE_morph->sumEntries()));
  resolRDH_mass996_catEBEE_morph->plotOn(frameA, LineColor(kBlue), MarkerStyle(kDashed), MarkerSize(3),Rescale(1./resolRDH_mass996_catEBEE_morph->sumEntries()));
  resolRDH_mass992_catEBEE_morph->plotOn(frameA, LineColor(kYellow), MarkerStyle(kDashed), MarkerSize(3),Rescale(1./resolRDH_mass992_catEBEE_morph->sumEntries()));
  resolRDH_mass1004_catEBEE_morph->plotOn(frameA, LineColor(kOrange), MarkerStyle(kDashed), MarkerSize(3),Rescale(1./resolRDH_mass1004_catEBEE_morph->sumEntries()));
  resolRDH_mass1008_catEBEE_morph->plotOn(frameA, LineColor(kGreen), MarkerStyle(kDashed), MarkerSize(3),Rescale(1./resolRDH_mass1008_catEBEE_morph->sumEntries()));
  //resolRDH_mass1500_catEBEE_morph->plotOn(frameA, LineColor(kPink), MarkerStyle(kDashed), MarkerSize(3),Rescale(1./resolRDH_mass1500_catEBEE_morph->sumEntries()));

  c1->SetLogy();
  frameA->Draw();
  double max = frameA->GetMaximum();
  frameA->GetYaxis()->SetRangeUser(0.00001,1.);
  frameA->GetXaxis()->SetTitle("m_{#gamma#gamma}-m_{#gamma#gamma}^{gen}");
  frameA->SetTitle("Resolution");
  frameA->Draw();
  c1->SaveAs("test.png");

  return;
}

