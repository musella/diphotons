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

  // files
  TFile *pas  = new TFile("JamboreeFiles/daTenereVersioneFinale_smearings/ResolutionHistos_nominalSmearings.root");
  TFile *rr4x = new TFile("newFiles/rereco74WithFix/ResHistosGenOnlyScan.root");
  TFile *pfix = new TFile("newFiles/promptWithFixes/ResHistosGenOnlyScan.root");
  RooRealVar* deltaM = new RooRealVar("deltaM", "",   -10000, 10000,   "GeV");

  // different recos
  RooDataHist *resolRDH_mass750_catEBEE_pas  = (RooDataHist*)pas->Get("resolRDH_mass750_catEBEE");
  RooDataHist *resolRDH_mass750_catEBEE_rr4x = (RooDataHist*)rr4x->Get("resolRDH_mass750_catEBEE");
  RooDataHist *resolRDH_mass750_catEBEE_pfix = (RooDataHist*)pfix->Get("resolRDH_mass750_catEBEE");
  
  // plot
  RooPlot *frameA = deltaM->frame(Range(-40,40),Bins(40));
  TCanvas *c1 = new TCanvas("c1","c1",1);
  resolRDH_mass750_catEBEE_pas->plotOn(frameA, MarkerColor(kYellow), MarkerStyle(kDashed), MarkerSize(3), Rescale(1./resolRDH_mass750_catEBEE_pas->sumEntries()));
  resolRDH_mass750_catEBEE_rr4x->plotOn(frameA, MarkerColor(kRed), MarkerStyle(kDashed), MarkerSize(3), Rescale(1./resolRDH_mass750_catEBEE_rr4x->sumEntries()));
  //resolRDH_mass750_catEBEE_pfix->plotOn(frameA, MarkerColor(kViolet), MarkerStyle(kDashed), MarkerSize(3), Rescale(1./resolRDH_mass750_catEBEE_pfix->sumEntries()));
  frameA->Draw();
  double max = frameA->GetMaximum();
  frameA->GetYaxis()->SetRangeUser(0.0, 0.1);
  frameA->GetXaxis()->SetTitle("m_{#gamma#gamma}-m_{#gamma#gamma}^{gen}");
  frameA->SetTitle("Resolution");

  // legend
  TLegend* leg = new TLegend(15, 0.06, 30, 0.09, "", "");
  leg->SetTextSize(0.0286044);
  leg->SetTextFont(42);
  leg->SetBorderSize(0);
  leg->SetFillStyle(0);
  leg->AddEntry(frameA->getObject(0),"pas","LP");
  leg->AddEntry(frameA->getObject(1),"74x rereco","LP");
  //leg->AddEntry(frameA->getObject(2),"check","LP");

  frameA->Draw();
  leg->Draw();
  c1->SaveAs("test.png");

  return;
}

