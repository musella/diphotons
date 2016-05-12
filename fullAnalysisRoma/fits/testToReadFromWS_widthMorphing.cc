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

  TFile *fileResNominal     = new TFile("daTenere/width/01/IntrinsicWidthHistos_1000to5000.root");
  TFile *filePointFromMorph = new TFile("daTenere/width/01/WidthHistosGenOnlyScan_1000to5000.root");

  RooRealVar* deltaMgen = new RooRealVar("deltaMgen", "",   -10000, 10000,   "GeV");

  RooDataHist *widthRDH_mass1000_catEBEB_nom   = (RooDataHist*)fileResNominal->Get("intWidthRDH_mass1000_catEBEB_kpl01");
  RooDataHist *widthRDH_mass1000_catEBEB_morph = (RooDataHist*)filePointFromMorph->Get("widthRDH_mass1000_catEBEB_kpl01");
  RooDataHist *widthRDH_mass1004_catEBEB_morph = (RooDataHist*)filePointFromMorph->Get("widthRDH_mass1004_catEBEB_kpl01");
  RooDataHist *widthRDH_mass1008_catEBEB_morph = (RooDataHist*)filePointFromMorph->Get("widthRDH_mass1008_catEBEB_kpl01");
  RooDataHist *widthRDH_mass754_catEBEB_morph = (RooDataHist*)filePointFromMorph->Get("widthRDH_mass754_catEBEB_kpl01");
  RooDataHist *widthRDH_mass758_catEBEB_morph = (RooDataHist*)filePointFromMorph->Get("widthRDH_mass758_catEBEB_kpl01");

  RooPlot *frameA = deltaMgen->frame(Range(-25,25),Bins(30));
  widthRDH_mass1000_catEBEB_nom->plotOn(frameA, LineColor(kBlack), LineStyle(kSolid), Rescale(1./widthRDH_mass1000_catEBEB_nom->sumEntries()));
  widthRDH_mass1000_catEBEB_morph->plotOn(frameA, LineColor(kRed), LineStyle(kDashed), Rescale(1./widthRDH_mass1000_catEBEB_morph->sumEntries()));
  widthRDH_mass1004_catEBEB_morph->plotOn(frameA, LineColor(kBlue), LineStyle(kDashed), Rescale(1./widthRDH_mass1004_catEBEB_morph->sumEntries()));
  widthRDH_mass1008_catEBEB_morph->plotOn(frameA, LineColor(kYellow), LineStyle(kDashed), Rescale(1./widthRDH_mass1008_catEBEB_morph->sumEntries()));
  //widthRDH_mass754_catEBEB_morph->plotOn(frameA, LineColor(kOrange), LineStyle(kDashed), Rescale(1./widthRDH_mass754_catEBEB_morph->sumEntries()));
  //widthRDH_mass758_catEBEB_morph->plotOn(frameA, LineColor(kGreen), LineStyle(kDashed), Rescale(1./widthRDH_mass758_catEBEB_morph->sumEntries()));
  frameA->Draw();
  //frameA->GetYaxis()->SetRangeUser(0.00001, 1.);
  frameA->GetYaxis()->SetRangeUser(0., 0.15);
  frameA->GetXaxis()->SetTitle("m_{#gamma#gamma}-m_{G}");
  frameA->SetTitle("Width");

  TCanvas *c1 = new TCanvas("c1","c1",1);
  //c1->SetLogy();
  frameA->Draw();
  c1->SaveAs("test.png");








  return;
}


//  LocalWords:  GeV
