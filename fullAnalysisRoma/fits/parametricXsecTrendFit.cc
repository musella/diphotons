#include <TFile.h>
#include <TTree.h>
#include <TH2.h>
#include <TF1.h>
#include <TGraphErrors.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TLegend.h>
#include <iostream>

using namespace std;

void parametricXsecTrendFit() {

  float lumi = 1.;      // pb

  float mass[27]  = { 500., 750., 1000., 1250., 1500., 1750., 2000., 2250., 2500., 2750., 3000., 3250., 3500., 3750., 4000., 4250., 4500., 4750., 5000., 5250., 5500., 5750., 6000., 6250., 6500., 6750., 7000.};
  float massE[27] = { 0.00001, 0.00001, 0.00001, 0.00001, 0.00001, 0.00001, 0.00001, 0.00001, 0.00001, 0.00001, 0.00001, 0.00001, 0.00001, 0.00001, 0.00001, 0.00001, 0.00001, 0.00001, 0.00001, 0.00001, 0.00001, 0.00001, 0.00001, 0.00001, 0.00001, 0.00001, 0.00001 };
  float xsec[27]  = { 3.3e-01, 5.07e-02, 1.21e-02, 3.692e-03, 1.34e-03, 5.523e-04, 2.43e-04, 1.137e-04, 5.464e-05, 2.831e-05, 1.5e-05, 8.098e-06, 4.475e-06, 2.460e-06, 1.28e-06, 7.749e-07, 4.352e-07, 2.505e-07, 1.42e-07, 8.346e-08, 4.840e-08, 2.652e-08, 1.55e-08, 9.205e-09, 5.211e-09, 3.091e-09, 1.72e-09 };
  float xsecE[27] = { 10e-2, 10e-2, 10e-2, 10e-2, 10e-2, 10e-2, 10e-2, 10e-2, 10e-2, 10e-2, 10e-2, 10e-2, 10e-2, 10e-2, 10e-2, 10e-2, 10e-2, 10e-2, 10e-2, 10e-2, 10e-2, 10e-2, 10e-2, 10e-2, 10e-2, 10e-2, 10e-2 };
  //for (int ii=0; ii<27; ii++) {
  //xsecE[ii] = xsec[ii]/100000.;
  //}

  // graphs
  TGraphErrors *gXsec = new TGraphErrors(27,mass,xsec, massE, xsecE);

  // cosmetics
  gXsec->SetMarkerStyle(20);    gXsec->SetMarkerColor(1);

  // Now do the fits with a pol2
  TF1* f1_mass = new TF1("f1_mass", "pow(10,[0]*x+[1])", 490., 7100.);
  // TF1* f1_mass = new TF1("f1_mass", "[0]*x+[1]", 490, 7100.);
  // TF1* f1_mass = new TF1("f1_mass", "expo", 490, 7100.);
  f1_mass->SetParameter(1,0.4);
  f1_mass->SetParameter(0,-0.002);
    
  gXsec->Fit("f1_mass","","",500,2000);
  TF1 *fxsec  = gXsec->GetFunction("f1_mass");
  fxsec->SetLineColor(5);

  // and finally plots
  gStyle->SetOptStat(0);
  
  TH2F *myH2 = new TH2F("myH2","", 100, 0., 7500, 100, 0.0000000001, 100.);

  TCanvas myCanvas0("myCanvas0","",1);
  myCanvas0.SetLogy();
  myH2->Draw();
  myH2->GetXaxis()->SetTitle("diphoton mass");
  myH2->GetYaxis()->SetTitle("x-sec");
  gXsec->Draw("PEsame");
  myCanvas0.SaveAs("xsecTrend.png");

  // Save parameterizations
  TFile myParaFile("xsecEvolution.root","RECREATE");
  fxsec->Write("fxsec");
}
