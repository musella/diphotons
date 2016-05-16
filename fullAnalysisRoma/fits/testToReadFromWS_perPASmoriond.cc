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
#include "TLegend.h"
#include "TVectorD.h"
#include <iostream>
#include "TCanvas.h"
#include "CMS_lumi.C"   

using namespace RooFit;
using namespace std;

void runfits() {

  TFile *fileWSdec = new TFile("/afs/cern.ch/user/m/musella/public/workspace/exo/dec_pdfs_4plotting/full_analysis_spring15_7415v2_sync_v5_data_ecorr_cic2_default_shapes_approval_lumi_2.4_grav_001_500.root");
  RooWorkspace *wDec = (RooWorkspace*) fileWSdec->Get("wtemplates");

  TFile *fileWSmor = new TFile("/afs/cern.ch/user/m/musella/public/workspace/exo/full_analysis_moriond16v1_sync_v4_data_cic2_default_shapes_spin2_wnuis_lumi_2.69/full_analysis_moriond16v1_sync_v4_data_cic2_default_shapes_spin2_wnuis_lumi_2.69_grav_001_500.root");
  RooWorkspace *wMor = (RooWorkspace*) fileWSmor->Get("wtemplates");
 
  // RooRealvar
  RooRealVar *mggEBEB = wDec->var("mggEBEB");
  RooRealVar *mggEBEE = wDec->var("mggEBEE");

  // Convolution
  RooHistPdf *convEBEB_dec = (RooHistPdf*)wDec->pdf("model_signal_grav_001_500_EBEB");
  RooHistPdf *convEBEE_dec = (RooHistPdf*)wDec->pdf("model_signal_grav_001_500_EBEE");
  RooHistPdf *convEBEB_mor = (RooHistPdf*)wMor->pdf("model_signal_grav_001_500_EBEB");
  RooHistPdf *convEBEE_mor = (RooHistPdf*)wMor->pdf("model_signal_grav_001_500_EBEE");

  TLatex *lat = new TLatex();
  lat->SetTextSize(0.06);
  lat->SetTextFont(42);

  // Plot
  mggEBEB->setRange(435,565);                                  // to normalize the pdf in the range of the plot
  mggEBEB->setRange("normRange",435,565);
  RooPlot *frameCat0 = mggEBEB->frame(Range(435,565));
  TCanvas *c1 = new TCanvas("c1","c1",1);
  convEBEB_mor->plotOn(frameCat0, LineColor(kRed), LineStyle(kSolid), NormRange("normRange"));
  convEBEB_dec->plotOn(frameCat0, LineColor(kBlue), LineStyle(kSolid), NormRange("normRange"));
  frameCat0->GetXaxis()->SetTitleSize(0.045);
  frameCat0->GetYaxis()->SetTitleSize(0.045);
  frameCat0->GetXaxis()->SetLabelSize(0.045);
  frameCat0->GetYaxis()->SetLabelSize(0.045);
  frameCat0->GetXaxis()->SetTitle("m_{#gamma#gamma} (GeV)");
  frameCat0->GetYaxis()->SetTitle("Fraction of events");
  frameCat0->SetTitle("");
  // legend                                          
  TLegend* legmcEBEB = new TLegend(440, 0.15, 470, 0.25, "", "");
  legmcEBEB->SetTextSize(0.06);
  legmcEBEB->SetTextFont(42);
  legmcEBEB->SetBorderSize(0);
  legmcEBEB->SetFillStyle(0);
  legmcEBEB->AddEntry(frameCat0->getObject(1),"Prompt-reco","LP");
  legmcEBEB->AddEntry(frameCat0->getObject(0),"Re-reco","LP");
  frameCat0->Draw();
  lat->DrawLatex(440,0.32,"#splitline{m = 500 GeV}{#frac{#Gamma}{m} = 1.4 #times 10^{-4}}");
  legmcEBEB->Draw("same");
  CMS_lumi(c1,4,1); 
  c1->SaveAs("decVsMorEBEB.png");
  c1->SaveAs("decVsMorEBEB.pdf");
  c1->SaveAs("decVsMorEBEB.root");

  mggEBEE->setRange(435,565);
  mggEBEE->setRange("normRange",435,565);
  RooPlot *frameCat1 = mggEBEE->frame(Range(435,565));
  TCanvas *c2 = new TCanvas("c2","c2",1);
  convEBEE_mor->plotOn(frameCat1, LineColor(kRed), LineStyle(kSolid), NormRange("normRange"));
  convEBEE_dec->plotOn(frameCat1, LineColor(kBlue), LineStyle(kSolid), NormRange("normRange"));
  frameCat1->GetXaxis()->SetTitle("m_{#gamma#gamma} (GeV)");
  frameCat1->GetXaxis()->SetTitleSize(0.045);
  frameCat1->GetYaxis()->SetTitleSize(0.045);
  frameCat1->GetXaxis()->SetLabelSize(0.045);
  frameCat1->GetYaxis()->SetLabelSize(0.045);
  frameCat1->GetYaxis()->SetTitle("Fraction of events");
  frameCat1->SetTitle("");
  TLegend* legmcEBEE = new TLegend(440, 0.13, 470, 0.19, "","");
  legmcEBEE->SetTextSize(0.06);
  legmcEBEE->SetTextFont(42);
  legmcEBEE->SetBorderSize(0);
  legmcEBEE->SetFillStyle(0);
  legmcEBEE->AddEntry(frameCat1->getObject(1),"Prompt-reco","LP");
  legmcEBEE->AddEntry(frameCat1->getObject(0),"Re-reco","LP");
  frameCat1->Draw();
  lat->DrawLatex(440,0.22,"#splitline{m = 500 GeV}{#frac{#Gamma}{m} = 1.4 #times 10^{-4}}");
  legmcEBEE->Draw("same");
  CMS_lumi(c2,4,1); 
  c2->SaveAs("decVsMorEBEE.png");
  c2->SaveAs("decVsMorEBEE.pdf");
  c2->SaveAs("decVsMorEBEE.root");

  return;
}

