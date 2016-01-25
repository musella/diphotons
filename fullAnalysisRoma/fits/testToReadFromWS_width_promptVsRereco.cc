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
  
  TFile *filePas = new TFile("JamboreeFiles/daTenereVersioneFinale_smearings/WidthHistosGenOnlyScan001.root");
  TFile *fileFix = new TFile("widthWithfix/001/WidthHistosGenOnlyScan.root");

  RooRealVar* deltaMgen = new RooRealVar("deltaMgen", "",   -10000, 10000,   "GeV");

  RooDataHist *widthRDH_mass750_catEBEB_pas = (RooDataHist*)filePas->Get("widthRDH_mass750_catEBEB_kpl001");
  RooDataHist *widthRDH_mass750_catEBEB_fix = (RooDataHist*)fileFix->Get("widthRDH_mass750_catEBEB_kpl001");

  RooPlot *frameA = deltaMgen->frame(Range(-10,10),Bins(30));
  widthRDH_mass750_catEBEB_pas->plotOn(frameA, MarkerColor(kYellow), MarkerStyle(kDashed), MarkerSize(3));
  widthRDH_mass750_catEBEB_fix->plotOn(frameA, MarkerColor(kRed),    MarkerStyle(kDashed), MarkerSize(3));
  frameA->Draw();
  frameA->GetXaxis()->SetTitle("m_{#gamma#gamma}-m_{G}");
  frameA->SetTitle("Width");

  TCanvas *c1 = new TCanvas("c1","c1",1);
  //c1->SetLogy();
  frameA->Draw();
  c1->SaveAs("test.png");








  return;
}


//  LocalWords:  GeV
