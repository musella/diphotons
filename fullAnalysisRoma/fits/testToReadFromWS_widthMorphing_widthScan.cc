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

  TFile *filePointFromMorph = new TFile("daTenere_NuovoMetodo/widthScan/WidthHistosGenOnlyScan.root");
  
  RooRealVar* deltaMgen = new RooRealVar("deltaMgen", "", -395., 395., "GeV");

  for (int ii=4; ii<30; ii++) {
    
    float coupling = 0.01 + ii*0.01;
    TString couplingS = TString::Format("%1.2f",coupling);

    float rangeInf = - 4. * 1.4 * coupling * coupling * 750;
    float rangeSup = + 4. * 1.4 * coupling * coupling * 750;
    int bins = (int)( (rangeSup-rangeInf)/0.1 );

    TString stringEBEB = "widthRDH_mass750_catEBEB_kpl" + TString::Format("%1.6f",coupling);
    cout << "coupling " << coupling << ", EBEB: " << stringEBEB << endl;
    TString stringEBEE = "widthRDH_mass750_catEBEE_kpl" + TString::Format("%1.6f",coupling);
    cout << "coupling " << coupling << ", EBEE: " << stringEBEE << endl;

    RooDataHist *widthRDH_catEBEB_morph = (RooDataHist*)filePointFromMorph->Get(stringEBEB);
    widthRDH_catEBEB_morph->Print();
    RooDataHist *widthRDH_catEBEE_morph = (RooDataHist*)filePointFromMorph->Get(stringEBEE);
    widthRDH_catEBEE_morph->Print();

    RooPlot *frameA = deltaMgen->frame(Range(rangeInf,rangeSup));
    widthRDH_catEBEB_morph->plotOn(frameA, MarkerColor(kRed), LineStyle(kDashed), Rescale(1./widthRDH_catEBEB_morph->sumEntries()));
    widthRDH_catEBEE_morph->plotOn(frameA, MarkerColor(kGreen), LineStyle(kDashed), Rescale(1./widthRDH_catEBEE_morph->sumEntries()));
    float max = frameA->GetMaximum();
    frameA->SetMaximum(1.);
    frameA->SetMaximum(0.5);
    if (coupling>0.075) frameA->SetMaximum(0.25);
    if (coupling>0.095) frameA->SetMaximum(0.15);
    if (coupling>0.125) frameA->SetMaximum(0.1);
    if (coupling>0.175) frameA->SetMaximum(0.05);
    if (coupling>0.215) frameA->SetMaximum(0.03);
    frameA->SetMinimum(0.);
    frameA->Draw();
    frameA->GetXaxis()->SetTitle("m_{#gamma#gamma}-m_{G}");
    frameA->SetTitle("Width");
    
    TCanvas *c1 = new TCanvas("c1","c1",1);
    frameA->Draw();
    TString test = "TestK"+couplingS+".png";
    c1->SaveAs(test);
  }
  
  return;
}


//  LocalWords:  GeV
