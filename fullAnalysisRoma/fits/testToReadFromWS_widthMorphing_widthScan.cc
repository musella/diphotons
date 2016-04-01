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

  TFile *fileResNominal     = new TFile("primiTestScanWidth/IntrinsicWidthHistos.root"); 
  TFile *filePointFromMorph = new TFile("primiTestScanWidth/WidthHistosGenOnlyScan.root");
  
  RooRealVar* deltaMgen = new RooRealVar("deltaMgen", "", -80, 80,   "GeV");

  cout << "========== nominal" << endl;
  RooDataHist *widthRDH_001_catEBEB_nom   = (RooDataHist*)fileResNominal->Get("intWidthRDH_mass750_catEBEB_kpl001"); 
  widthRDH_001_catEBEB_nom->Print();

  cout << "========== fit" << endl;
  RooDataHist *widthRDH_025_catEBEB_morph = (RooDataHist*)filePointFromMorph->Get("widthRDH_mass750_catEBEB_kpl0.250000");
  widthRDH_025_catEBEB_morph->Print();
  RooDataHist *widthRDH_026_catEBEB_morph = (RooDataHist*)filePointFromMorph->Get("widthRDH_mass750_catEBEB_kpl0.260000");
  widthRDH_026_catEBEB_morph->Print();
  RooDataHist *widthRDH_027_catEBEB_morph = (RooDataHist*)filePointFromMorph->Get("widthRDH_mass750_catEBEB_kpl0.270000");
  widthRDH_027_catEBEB_morph->Print();
  RooDataHist *widthRDH_028_catEBEB_morph = (RooDataHist*)filePointFromMorph->Get("widthRDH_mass750_catEBEB_kpl0.280000");
  widthRDH_028_catEBEB_morph->Print();
  RooDataHist *widthRDH_029_catEBEB_morph = (RooDataHist*)filePointFromMorph->Get("widthRDH_mass750_catEBEB_kpl0.290000");
  widthRDH_029_catEBEB_morph->Print();
  RooDataHist *widthRDH_030_catEBEB_morph = (RooDataHist*)filePointFromMorph->Get("widthRDH_mass750_catEBEB_kpl0.300000");
  widthRDH_030_catEBEB_morph->Print();


  RooPlot *frameA = deltaMgen->frame(Range(-80,80),Bins(100));
  //widthRDH_001_catEBEB_nom->plotOn(frameA, LineColor(kBlack), LineStyle(kSolid), Rescale(1./widthRDH_001_catEBEB_nom->sumEntries()));
  //widthRDH_001_catEBEB_morph->plotOn(frameA, MarkerColor(kGreen), LineStyle(kSolid), Rescale(1./widthRDH_001_catEBEB_morph->sumEntries()));

  widthRDH_025_catEBEB_morph->plotOn(frameA, MarkerColor(kGreen), LineStyle(kDashed));
  widthRDH_026_catEBEB_morph->plotOn(frameA, MarkerColor(kBlue), LineStyle(kDashed));
  widthRDH_027_catEBEB_morph->plotOn(frameA, MarkerColor(kViolet), LineStyle(kDashed));
  widthRDH_028_catEBEB_morph->plotOn(frameA, MarkerColor(kOrange), LineStyle(kDashed));
  widthRDH_029_catEBEB_morph->plotOn(frameA, MarkerColor(kRed), LineStyle(kDashed));
  widthRDH_030_catEBEB_morph->plotOn(frameA, MarkerColor(kBlack), LineStyle(kDashed));

  // legend  
  //TLegend* legmc = new TLegend(-4.5, 2500, -1.5, 3500, "", "");
  //TLegend* legmc = new TLegend(-18., 400, -10., 600, "", "");
  //TLegend* legmc = new TLegend(-28., 400, -13., 600, "", "");
  //TLegend* legmc = new TLegend(-48., 180, -15., 260, "", "");
  TLegend* legmc = new TLegend(-58., 140, -30., 220, "", "");
  legmc->SetTextSize(0.0286044);
  legmc->SetTextFont(42);
  legmc->SetBorderSize(0);
  legmc->SetFillStyle(0);
  legmc->AddEntry(frameA->getObject(0),"k=0.25","LP");
  legmc->AddEntry(frameA->getObject(1),"k=0.26","LP");
  legmc->AddEntry(frameA->getObject(2),"k=0.27","LP");
  legmc->AddEntry(frameA->getObject(3),"k=0.28","LP");
  legmc->AddEntry(frameA->getObject(4),"k=0.29","LP");
  legmc->AddEntry(frameA->getObject(5),"k=0.30","LP");

  //frameA->SetMaximum(0.1);
  frameA->SetMinimum(0.);
  frameA->Draw();
  frameA->GetXaxis()->SetTitle("m_{#gamma#gamma}-m_{G}");
  frameA->SetTitle("Width");
  TCanvas *c1 = new TCanvas("c1","c1",1);
  frameA->Draw();
  legmc->Draw();
  c1->SaveAs("checkWithNominalWidth_EBEB_025-030.png");

  return;
}


//  LocalWords:  GeV
