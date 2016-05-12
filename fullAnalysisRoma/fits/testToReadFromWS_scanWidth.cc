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

  // with dataset, obtained running with genOnly=0    
  TFile *fileWS001 = new TFile("primiTestScanWidth/conRoodatasetFattoConGenOnlyFalse_0T/keepingRoodataset_0T__750_001.root");
  TFile *fileWS01  = new TFile("primiTestScanWidth/conRoodatasetFattoConGenOnlyFalse_0T/keepingRoodataset_0T__750_01.root");
  TFile *fileWS02  = new TFile("primiTestScanWidth/conRoodatasetFattoConGenOnlyFalse_0T/keepingRoodataset_0T__750_02.root");
  RooWorkspace *wfullsim001 = (RooWorkspace*) fileWS001->Get("w");
  RooWorkspace *wfullsim01  = (RooWorkspace*) fileWS01->Get("w");
  RooWorkspace *wfullsim02  = (RooWorkspace*) fileWS02->Get("w");

  // the new fit results, from the width scan
  TFile *fileWSfineScan_widthScan = new TFile("primiTestScanWidth/myWSwithMorphing_13TeV_0_v6.root");    
  RooWorkspace *wfineScan_widthScan = (RooWorkspace*) fileWSfineScan_widthScan->Get("w");

  // the old fit results, from the mass scan
  TFile *fileWSfineScan_massScan001 = new TFile("signalModel76x_001_500to998_0T_v6.root");
  TFile *fileWSfineScan_massScan01  = new TFile("signalModel76x_01_500to998_0T_v6.root");
  TFile *fileWSfineScan_massScan02  = new TFile("signalModel76x_02_500to998_0T_v6.root");
  RooWorkspace *wfineScan_massScan001 = (RooWorkspace*) fileWSfineScan_massScan001->Get("w");
  RooWorkspace *wfineScan_massScan01  = (RooWorkspace*) fileWSfineScan_massScan01->Get("w");
  RooWorkspace *wfineScan_massScan02  = (RooWorkspace*) fileWSfineScan_massScan02->Get("w");

  // RooRealvar
  RooRealVar *mgg = wfineScan_widthScan->var("mgg");
  mgg->Print();
  cout << endl;



  // Convolution from width scan
  RooAbsPdf *conv001EBEB  = wfineScan_widthScan->pdf("Convolution_catEBEB_mass750_kpl0.010000");
  RooAbsPdf *conv001EBEE  = wfineScan_widthScan->pdf("Convolution_catEBEE_mass750_kpl0.010000");
  RooAbsPdf *conv01EBEB   = wfineScan_widthScan->pdf("Convolution_catEBEB_mass750_kpl0.100000");
  RooAbsPdf *conv01EBEE   = wfineScan_widthScan->pdf("Convolution_catEBEE_mass750_kpl0.100000");
  RooAbsPdf *conv02EBEB   = wfineScan_widthScan->pdf("Convolution_catEBEB_mass750_kpl0.200000");
  RooAbsPdf *conv02EBEE   = wfineScan_widthScan->pdf("Convolution_catEBEE_mass750_kpl0.200000");

  // To check the fit evolution
  RooAbsPdf *conv002EBEB  = wfineScan_widthScan->pdf("Convolution_catEBEB_mass750_kpl0.020000");
  RooAbsPdf *conv002EBEE  = wfineScan_widthScan->pdf("Convolution_catEBEE_mass750_kpl0.020000");
  RooAbsPdf *conv003EBEB  = wfineScan_widthScan->pdf("Convolution_catEBEB_mass750_kpl0.030000");
  RooAbsPdf *conv003EBEE  = wfineScan_widthScan->pdf("Convolution_catEBEE_mass750_kpl0.030000");
  RooAbsPdf *conv004EBEB  = wfineScan_widthScan->pdf("Convolution_catEBEB_mass750_kpl0.040000");
  RooAbsPdf *conv004EBEE  = wfineScan_widthScan->pdf("Convolution_catEBEE_mass750_kpl0.040000");
  RooAbsPdf *conv005EBEB  = wfineScan_widthScan->pdf("Convolution_catEBEB_mass750_kpl0.050000");
  RooAbsPdf *conv005EBEE  = wfineScan_widthScan->pdf("Convolution_catEBEE_mass750_kpl0.050000");
  RooAbsPdf *conv006EBEB  = wfineScan_widthScan->pdf("Convolution_catEBEB_mass750_kpl0.060000");
  RooAbsPdf *conv006EBEE  = wfineScan_widthScan->pdf("Convolution_catEBEE_mass750_kpl0.060000");
  RooAbsPdf *conv007EBEB  = wfineScan_widthScan->pdf("Convolution_catEBEB_mass750_kpl0.070000");
  RooAbsPdf *conv007EBEE  = wfineScan_widthScan->pdf("Convolution_catEBEE_mass750_kpl0.070000");
  RooAbsPdf *conv008EBEB  = wfineScan_widthScan->pdf("Convolution_catEBEB_mass750_kpl0.080000");
  RooAbsPdf *conv008EBEE  = wfineScan_widthScan->pdf("Convolution_catEBEE_mass750_kpl0.080000");
  RooAbsPdf *conv009EBEB  = wfineScan_widthScan->pdf("Convolution_catEBEB_mass750_kpl0.090000");
  RooAbsPdf *conv009EBEE  = wfineScan_widthScan->pdf("Convolution_catEBEE_mass750_kpl0.090000");
  RooAbsPdf *conv010EBEB  = wfineScan_widthScan->pdf("Convolution_catEBEB_mass750_kpl0.100000");
  RooAbsPdf *conv010EBEE  = wfineScan_widthScan->pdf("Convolution_catEBEE_mass750_kpl0.100000");
  RooAbsPdf *conv011EBEB  = wfineScan_widthScan->pdf("Convolution_catEBEB_mass750_kpl0.110000");
  RooAbsPdf *conv011EBEE  = wfineScan_widthScan->pdf("Convolution_catEBEE_mass750_kpl0.110000");
  RooAbsPdf *conv012EBEB  = wfineScan_widthScan->pdf("Convolution_catEBEB_mass750_kpl0.120000");
  RooAbsPdf *conv012EBEE  = wfineScan_widthScan->pdf("Convolution_catEBEE_mass750_kpl0.120000");
  RooAbsPdf *conv013EBEB  = wfineScan_widthScan->pdf("Convolution_catEBEB_mass750_kpl0.130000");
  RooAbsPdf *conv013EBEE  = wfineScan_widthScan->pdf("Convolution_catEBEE_mass750_kpl0.130000");
  RooAbsPdf *conv014EBEB  = wfineScan_widthScan->pdf("Convolution_catEBEB_mass750_kpl0.140000");
  RooAbsPdf *conv014EBEE  = wfineScan_widthScan->pdf("Convolution_catEBEE_mass750_kpl0.140000");
  RooAbsPdf *conv015EBEB  = wfineScan_widthScan->pdf("Convolution_catEBEB_mass750_kpl0.150000");
  RooAbsPdf *conv015EBEE  = wfineScan_widthScan->pdf("Convolution_catEBEE_mass750_kpl0.150000");
  RooAbsPdf *conv016EBEB  = wfineScan_widthScan->pdf("Convolution_catEBEB_mass750_kpl0.160000");
  RooAbsPdf *conv016EBEE  = wfineScan_widthScan->pdf("Convolution_catEBEE_mass750_kpl0.160000");
  RooAbsPdf *conv017EBEB  = wfineScan_widthScan->pdf("Convolution_catEBEB_mass750_kpl0.170000");
  RooAbsPdf *conv017EBEE  = wfineScan_widthScan->pdf("Convolution_catEBEE_mass750_kpl0.170000");
  RooAbsPdf *conv018EBEB  = wfineScan_widthScan->pdf("Convolution_catEBEB_mass750_kpl0.180000");
  RooAbsPdf *conv018EBEE  = wfineScan_widthScan->pdf("Convolution_catEBEE_mass750_kpl0.180000");
  RooAbsPdf *conv019EBEB  = wfineScan_widthScan->pdf("Convolution_catEBEB_mass750_kpl0.190000");
  RooAbsPdf *conv019EBEE  = wfineScan_widthScan->pdf("Convolution_catEBEE_mass750_kpl0.190000");
  RooAbsPdf *conv020EBEB  = wfineScan_widthScan->pdf("Convolution_catEBEB_mass750_kpl0.200000");
  RooAbsPdf *conv020EBEE  = wfineScan_widthScan->pdf("Convolution_catEBEE_mass750_kpl0.200000");
  RooAbsPdf *conv021EBEB  = wfineScan_widthScan->pdf("Convolution_catEBEB_mass750_kpl0.210000");
  RooAbsPdf *conv021EBEE  = wfineScan_widthScan->pdf("Convolution_catEBEE_mass750_kpl0.210000");
  RooAbsPdf *conv022EBEB  = wfineScan_widthScan->pdf("Convolution_catEBEB_mass750_kpl0.220000");
  RooAbsPdf *conv022EBEE  = wfineScan_widthScan->pdf("Convolution_catEBEE_mass750_kpl0.220000");
  RooAbsPdf *conv023EBEB  = wfineScan_widthScan->pdf("Convolution_catEBEB_mass750_kpl0.230000");
  RooAbsPdf *conv023EBEE  = wfineScan_widthScan->pdf("Convolution_catEBEE_mass750_kpl0.230000");
  RooAbsPdf *conv024EBEB  = wfineScan_widthScan->pdf("Convolution_catEBEB_mass750_kpl0.240000");
  RooAbsPdf *conv024EBEE  = wfineScan_widthScan->pdf("Convolution_catEBEE_mass750_kpl0.240000");
  RooAbsPdf *conv025EBEB  = wfineScan_widthScan->pdf("Convolution_catEBEB_mass750_kpl0.250000");
  RooAbsPdf *conv025EBEE  = wfineScan_widthScan->pdf("Convolution_catEBEE_mass750_kpl0.250000");
  RooAbsPdf *conv026EBEB  = wfineScan_widthScan->pdf("Convolution_catEBEB_mass750_kpl0.260000");
  RooAbsPdf *conv026EBEE  = wfineScan_widthScan->pdf("Convolution_catEBEE_mass750_kpl0.260000");
  RooAbsPdf *conv027EBEB  = wfineScan_widthScan->pdf("Convolution_catEBEB_mass750_kpl0.270000");
  RooAbsPdf *conv027EBEE  = wfineScan_widthScan->pdf("Convolution_catEBEE_mass750_kpl0.270000");
  RooAbsPdf *conv028EBEB  = wfineScan_widthScan->pdf("Convolution_catEBEB_mass750_kpl0.280000");
  RooAbsPdf *conv028EBEE  = wfineScan_widthScan->pdf("Convolution_catEBEE_mass750_kpl0.280000");
  RooAbsPdf *conv029EBEB  = wfineScan_widthScan->pdf("Convolution_catEBEB_mass750_kpl0.290000");
  RooAbsPdf *conv029EBEE  = wfineScan_widthScan->pdf("Convolution_catEBEE_mass750_kpl0.290000");

  // Dataset
  RooAbsData *dataset001EBEB = wfullsim001->data("SigWeight_catEBEB_mass750");
  RooAbsData *dataset001EBEE = wfullsim001->data("SigWeight_catEBEE_mass750");
  RooAbsData *dataset01EBEB  = wfullsim01->data("SigWeight_catEBEB_mass750");
  RooAbsData *dataset01EBEE  = wfullsim01->data("SigWeight_catEBEE_mass750");
  RooAbsData *dataset02EBEB  = wfullsim02->data("SigWeight_catEBEB_mass750");
  RooAbsData *dataset02EBEE  = wfullsim02->data("SigWeight_catEBEE_mass750");

  // Convolution from mass scan
  RooAbsPdf *conv001EBEB_massScan = wfineScan_massScan001->pdf("Convolution_catEBEB_mass750_kpl001");
  RooAbsPdf *conv01EBEB_massScan  = wfineScan_massScan01->pdf("Convolution_catEBEB_mass750_kpl01");
  RooAbsPdf *conv02EBEB_massScan  = wfineScan_massScan02->pdf("Convolution_catEBEB_mass750_kpl02");
  RooAbsPdf *conv001EBEE_massScan = wfineScan_massScan001->pdf("Convolution_catEBEE_mass750_kpl001");
  RooAbsPdf *conv01EBEE_massScan  = wfineScan_massScan01->pdf("Convolution_catEBEE_mass750_kpl01");
  RooAbsPdf *conv02EBEE_massScan  = wfineScan_massScan02->pdf("Convolution_catEBEE_mass750_kpl02");
  



  // Plots: data vs fit
  RooPlot *frameCatEBEB_001 = mgg->frame(Range(700,800),Bins(200));
  TCanvas *cEBEB_001 = new TCanvas("cEBEB_001","cEBEB_001",1);
  dataset001EBEB->plotOn(frameCatEBEB_001, LineColor(kBlack), LineStyle(kSolid));
  conv001EBEB->plotOn(frameCatEBEB_001, LineColor(kGreen), LineStyle(kDashed));
  frameCatEBEB_001->GetXaxis()->SetTitle("m_{#gamma#gamma}");
  frameCatEBEB_001->SetTitle("EBEB");
  frameCatEBEB_001->Draw();
  cEBEB_001->SaveAs("dataVsFit_001_EBEB.png");
  cEBEB_001->SaveAs("dataVsFit_001_EBEB.root");

  RooPlot *frameCatEBEE_001 = mgg->frame(Range(700,800),Bins(200));
  TCanvas *cEBEE_001 = new TCanvas("cEBEE_001","cEBEE_001",1);
  dataset001EBEE->plotOn(frameCatEBEE_001, LineColor(kBlack), LineStyle(kSolid));
  conv001EBEE->plotOn(frameCatEBEE_001, LineColor(kGreen), LineStyle(kDashed));
  frameCatEBEE_001->GetXaxis()->SetTitle("m_{#gamma#gamma}");
  frameCatEBEE_001->SetTitle("EBEE");
  frameCatEBEE_001->Draw();
  cEBEE_001->SaveAs("dataVsFit_001_EBEE.png");
  cEBEE_001->SaveAs("dataVsFit_001_EBEE.root");

  RooPlot *frameCatEBEB_01 = mgg->frame(Range(600,900),Bins(200));
  TCanvas *cEBEB_01 = new TCanvas("cEBEB_01","cEBEB_01",1);
  dataset01EBEB->plotOn(frameCatEBEB_01, LineColor(kBlack), LineStyle(kSolid));
  conv01EBEB->plotOn(frameCatEBEB_01, LineColor(kGreen), LineStyle(kDashed));
  frameCatEBEB_01->GetXaxis()->SetTitle("m_{#gamma#gamma}");
  frameCatEBEB_01->SetTitle("EBEB");
  frameCatEBEB_01->Draw();
  cEBEB_01->SaveAs("dataVsFit_01_EBEB.png");
  cEBEB_01->SaveAs("dataVsFit_01_EBEB.root");

  RooPlot *frameCatEBEE_01 = mgg->frame(Range(600,900),Bins(200));
  TCanvas *cEBEE_01 = new TCanvas("cEBEE_01","cEBEE_01",1);
  dataset01EBEE->plotOn(frameCatEBEE_01, LineColor(kBlack), LineStyle(kSolid));
  conv01EBEE->plotOn(frameCatEBEE_01, LineColor(kGreen), LineStyle(kDashed));
  frameCatEBEE_01->GetXaxis()->SetTitle("m_{#gamma#gamma}");
  frameCatEBEE_01->SetTitle("EBEE");
  frameCatEBEE_01->Draw();
  cEBEE_01->SaveAs("dataVsFit_01_EBEE.png");
  cEBEE_01->SaveAs("dataVsFit_01_EBEE.root");

  RooPlot *frameCatEBEB_02 = mgg->frame(Range(300,1200),Bins(200));
  TCanvas *cEBEB_02 = new TCanvas("cEBEB_02","cEBEB_02",1);
  dataset02EBEB->plotOn(frameCatEBEB_02, LineColor(kBlack), LineStyle(kSolid));
  conv02EBEB->plotOn(frameCatEBEB_02, LineColor(kGreen), LineStyle(kDashed));
  frameCatEBEB_02->GetXaxis()->SetTitle("m_{#gamma#gamma}");
  frameCatEBEB_02->SetTitle("EBEB");
  frameCatEBEB_02->Draw();
  cEBEB_02->SaveAs("dataVsFit_02_EBEB.png");
  cEBEB_02->SaveAs("dataVsFit_02_EBEB.root");

  RooPlot *frameCatEBEE_02 = mgg->frame(Range(300,1200),Bins(200));
  TCanvas *cEBEE_02 = new TCanvas("cEBEE_02","cEBEE_02",1);
  dataset02EBEE->plotOn(frameCatEBEE_02, LineColor(kBlack), LineStyle(kSolid));
  conv02EBEE->plotOn(frameCatEBEE_02, LineColor(kGreen), LineStyle(kDashed));
  frameCatEBEE_02->GetXaxis()->SetTitle("m_{#gamma#gamma}");
  frameCatEBEE_02->SetTitle("EBEE");
  frameCatEBEE_02->Draw();
  cEBEE_02->SaveAs("dataVsFit_02_EBEE.png");
  cEBEE_02->SaveAs("dataVsFit_02_EBEE.root");



  // Plots: mass scan vs width scan
  RooPlot *frameCatEBEB_001_massVsWidthFit = mgg->frame(Range(700,800),Bins(200));
  TCanvas *cEBEB_001_massVsWidthFit = new TCanvas("cEBEB_001_massVsWidthFit","cEBEB_001_massVsWidthFit",1);
  conv001EBEB->plotOn(frameCatEBEB_001_massVsWidthFit, LineColor(kGreen), LineStyle(kDashed));
  conv001EBEB_massScan->plotOn(frameCatEBEB_001_massVsWidthFit, LineColor(kRed), LineStyle(kDashed));
  frameCatEBEB_001_massVsWidthFit->GetXaxis()->SetTitle("m_{#gamma#gamma}");
  frameCatEBEB_001_massVsWidthFit->SetTitle("Convolution vs data");
  frameCatEBEB_001_massVsWidthFit->Draw();
  cEBEB_001_massVsWidthFit->SaveAs("massVsWidthFit_001_EBEB.png");
  cEBEB_001_massVsWidthFit->SaveAs("massVsWidthFit_001_EBEB.root");

  RooPlot *frameCatEBEE_001_massVsWidthFit = mgg->frame(Range(700,800),Bins(200));
  TCanvas *cEBEE_001_massVsWidthFit = new TCanvas("cEBEE_001_massVsWidthFit","cEBEE_001_massVsWidthFit",1);
  conv001EBEE->plotOn(frameCatEBEE_001_massVsWidthFit, LineColor(kGreen), LineStyle(kDashed));
  conv001EBEE_massScan->plotOn(frameCatEBEE_001_massVsWidthFit, LineColor(kRed), LineStyle(kDashed));
  frameCatEBEE_001_massVsWidthFit->GetXaxis()->SetTitle("m_{#gamma#gamma}");
  frameCatEBEE_001_massVsWidthFit->SetTitle("Convolution vs data");
  frameCatEBEE_001_massVsWidthFit->Draw();
  cEBEE_001_massVsWidthFit->SaveAs("massVsWidthFit_001_EBEE.png");
  cEBEE_001_massVsWidthFit->SaveAs("massVsWidthFit_001_EBEE.root");

  RooPlot *frameCatEBEB_01_massVsWidthFit = mgg->frame(Range(600,900),Bins(200));
  TCanvas *cEBEB_01_massVsWidthFit = new TCanvas("cEBEB_01_massVsWidthFit","cEBEB_01_massVsWidthFit",1);
  conv01EBEB->plotOn(frameCatEBEB_01_massVsWidthFit, LineColor(kGreen), LineStyle(kDashed));
  conv01EBEB_massScan->plotOn(frameCatEBEB_01_massVsWidthFit, LineColor(kRed), LineStyle(kDashed));
  frameCatEBEB_01_massVsWidthFit->GetXaxis()->SetTitle("m_{#gamma#gamma}");
  frameCatEBEB_01_massVsWidthFit->SetTitle("Convolution vs data");
  frameCatEBEB_01_massVsWidthFit->Draw();
  cEBEB_01_massVsWidthFit->SaveAs("massVsWidthFit_01_EBEB.png");
  cEBEB_01_massVsWidthFit->SaveAs("massVsWidthFit_01_EBEB.root");

  RooPlot *frameCatEBEE_01_massVsWidthFit = mgg->frame(Range(600,900),Bins(200));
  TCanvas *cEBEE_01_massVsWidthFit = new TCanvas("cEBEE_01_massVsWidthFit","cEBEE_01_massVsWidthFit",1);
  conv01EBEE->plotOn(frameCatEBEE_01_massVsWidthFit, LineColor(kGreen), LineStyle(kDashed));
  conv01EBEE_massScan->plotOn(frameCatEBEE_01_massVsWidthFit, LineColor(kRed), LineStyle(kDashed));
  frameCatEBEE_01_massVsWidthFit->GetXaxis()->SetTitle("m_{#gamma#gamma}");
  frameCatEBEE_01_massVsWidthFit->SetTitle("Convolution vs data");
  frameCatEBEE_01_massVsWidthFit->Draw();
  cEBEE_01_massVsWidthFit->SaveAs("massVsWidthFit_01_EBEE.png");
  cEBEE_01_massVsWidthFit->SaveAs("massVsWidthFit_01_EBEE.root");

  RooPlot *frameCatEBEB_02_massVsWidthFit = mgg->frame(Range(300,1200),Bins(200));
  TCanvas *cEBEB_02_massVsWidthFit = new TCanvas("cEBEB_02_massVsWidthFit","cEBEB_02_massVsWidthFit",1);
  conv02EBEB->plotOn(frameCatEBEB_02_massVsWidthFit, LineColor(kGreen), LineStyle(kDashed));
  conv02EBEB_massScan->plotOn(frameCatEBEB_02_massVsWidthFit, LineColor(kRed), LineStyle(kDashed));
  frameCatEBEB_02_massVsWidthFit->GetXaxis()->SetTitle("m_{#gamma#gamma}");
  frameCatEBEB_02_massVsWidthFit->SetTitle("Convolution vs data");
  frameCatEBEB_02_massVsWidthFit->Draw();
  cEBEB_02_massVsWidthFit->SaveAs("massVsWidthFit_02_EBEB.png");
  cEBEB_02_massVsWidthFit->SaveAs("massVsWidthFit_02_EBEB.root");

  RooPlot *frameCatEBEE_02_massVsWidthFit = mgg->frame(Range(300,1200),Bins(200));
  TCanvas *cEBEE_02_massVsWidthFit = new TCanvas("cEBEE_02_massVsWidthFit","cEBEE_02_massVsWidthFit",1);
  conv02EBEE->plotOn(frameCatEBEE_02_massVsWidthFit, LineColor(kGreen), LineStyle(kDashed));
  conv02EBEE_massScan->plotOn(frameCatEBEE_02_massVsWidthFit, LineColor(kRed), LineStyle(kDashed));
  frameCatEBEE_02_massVsWidthFit->GetXaxis()->SetTitle("m_{#gamma#gamma}");
  frameCatEBEE_02_massVsWidthFit->SetTitle("Convolution vs data");
  frameCatEBEE_02_massVsWidthFit->Draw();
  cEBEE_02_massVsWidthFit->SaveAs("massVsWidthFit_02_EBEE.png");
  cEBEE_02_massVsWidthFit->SaveAs("massVsWidthFit_02_EBEE.root");



  // Evolution with width
  RooPlot *frameEvolWidthEBEB = mgg->frame(Range(700,800),Bins(200));   // 001-005
  //RooPlot *frameEvolWidthEBEB = mgg->frame(Range(680,820),Bins(200));   // 005-009
  //RooPlot *frameEvolWidthEBEB = mgg->frame(Range(600,900),Bins(200));   // 009-017 
  //RooPlot *frameEvolWidthEBEB = mgg->frame(Range(300,1200),Bins(200));   
  TCanvas *cEvolWidthEBEB = new TCanvas("cEvolWidthEBEB","cEvolWidthEBEB",1);
  conv001EBEB->plotOn(frameEvolWidthEBEB, LineColor(kGreen), LineStyle(kDashed));
  conv002EBEB->plotOn(frameEvolWidthEBEB, LineColor(kBlue), LineStyle(kDashed));
  conv003EBEB->plotOn(frameEvolWidthEBEB, LineColor(kViolet), LineStyle(kDashed));
  conv004EBEB->plotOn(frameEvolWidthEBEB, LineColor(kOrange), LineStyle(kDashed));
  conv005EBEB->plotOn(frameEvolWidthEBEB, LineColor(kRed), LineStyle(kDashed));
  frameEvolWidthEBEB->GetXaxis()->SetTitle("m_{#gamma#gamma}");
  frameEvolWidthEBEB->SetTitle("Convolution vs data");

  TLegend* legmcEBEB = new TLegend(685., 0.01, 750., 0.022, "", "");
  //TLegend* legmcEBEB = new TLegend(685., 0.015, 750., 0.030, "", "");
  //TLegend* legmcEBEB = new TLegend(400., 0.006, 550., 0.06, "", "");
  //TLegend* legmcEBEB = new TLegend(400., 0.01, 550., 0.03, "", "");
  legmcEBEB->SetTextSize(0.0286044);
  legmcEBEB->SetTextFont(42);
  legmcEBEB->SetFillStyle(0);
  legmcEBEB->SetBorderSize(0);
  legmcEBEB->AddEntry(frameEvolWidthEBEB->getObject(0),"k=0.01","LP");
  legmcEBEB->AddEntry(frameEvolWidthEBEB->getObject(1),"k=0.02","LP");
  legmcEBEB->AddEntry(frameEvolWidthEBEB->getObject(2),"k=0.03","LP");
  legmcEBEB->AddEntry(frameEvolWidthEBEB->getObject(3),"k=0.04","LP");
  legmcEBEB->AddEntry(frameEvolWidthEBEB->getObject(4),"k=0.05","LP");

  frameEvolWidthEBEB->Draw();
  legmcEBEB->Draw();  
  //cEvolWidthEBEB->SetLogy();
  cEvolWidthEBEB->SaveAs("evolWidthEBEB.png");

  RooPlot *frameEvolWidthEBEE = mgg->frame(Range(680,830),Bins(200));   // 001-005  
  //RooPlot *frameEvolWidthEBEE = mgg->frame(Range(650,850),Bins(200));   // 005-009  
  //RooPlot *frameEvolWidthEBEE = mgg->frame(Range(600,900),Bins(200));   // 009-017 
  //RooPlot *frameEvolWidthEBEE = mgg->frame(Range(300,1200),Bins(200));
  TCanvas *cEvolWidthEBEE = new TCanvas("cEvolWidthEBEE","cEvolWidthEBEE",1);
  conv001EBEE->plotOn(frameEvolWidthEBEE, LineColor(kGreen), LineStyle(kDashed));
  conv002EBEE->plotOn(frameEvolWidthEBEE, LineColor(kBlue), LineStyle(kDashed));
  conv003EBEE->plotOn(frameEvolWidthEBEE, LineColor(kViolet), LineStyle(kDashed));
  conv004EBEE->plotOn(frameEvolWidthEBEE, LineColor(kOrange), LineStyle(kDashed));
  conv005EBEE->plotOn(frameEvolWidthEBEE, LineColor(kRed), LineStyle(kDashed));
  frameEvolWidthEBEE->GetXaxis()->SetTitle("m_{#gamma#gamma}");
  frameEvolWidthEBEE->SetTitle("Convolution vs data");

  TLegend* legmcEBEE = new TLegend(685., 0.01, 750., 0.018, "", "");
  //TLegend* legmcEBEE = new TLegend(665., 0.015, 725., 0.03, "", "");
  //TLegend* legmcEBEE = new TLegend(400., 0.006, 550., 0.06, "", "");
  //TLegend* legmcEBEE = new TLegend(400., 0.01, 550., 0.03, "", "");
  legmcEBEE->SetTextSize(0.0286044);
  legmcEBEE->SetTextFont(42);
  legmcEBEE->SetFillStyle(0);
  legmcEBEE->SetBorderSize(0);
  legmcEBEE->AddEntry(frameEvolWidthEBEE->getObject(0),"k=0.01","LP");
  legmcEBEE->AddEntry(frameEvolWidthEBEE->getObject(1),"k=0.02","LP");
  legmcEBEE->AddEntry(frameEvolWidthEBEE->getObject(2),"k=0.03","LP");
  legmcEBEE->AddEntry(frameEvolWidthEBEE->getObject(3),"k=0.04","LP");
  legmcEBEE->AddEntry(frameEvolWidthEBEE->getObject(4),"k=0.05","LP");

  frameEvolWidthEBEE->Draw();
  legmcEBEE->Draw(); 
  //cEvolWidthEBEE->SetLogy();
  cEvolWidthEBEE->SaveAs("evolWidthEBEE.png");

  return;
}

