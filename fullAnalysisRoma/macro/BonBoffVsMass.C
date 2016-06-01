#include "TString.h"
#include "TGraphErrors.h"
#include "TCanvas.h"
#include "TH2F.h"
#include "TLegend.h"
#include "TLatex.h"
#include <iostream>

using namespace std;

void BonBoffVsMass(){

  float mass[6]    = { 500, 750, 1000, 1500, 2000, 2500 };
  float massErr[6] = { 0, 0, 0, 0, 0, 0 };
  
  float meanEBEB_Bon[6]    = { -2.0033e-03, -3.0439e-03, -4.4436e-03, -6.5142e-03, -7.6966e-03, -8.7202e-03 };
  float meanErrEBEB_Bon[6] = { 8.15e-05, 7.57e-05, 7.42e-05, 7.12e-05, 7.03e-05, 6.88e-05 };
  float meanEBEE_Bon[6]    = { -1.9477e-03, -2.3147e-03, -3.6632e-03, -5.0304e-03, -6.1710e-03, -7.3437e-03 };
  float meanErrEBEE_Bon[6] = { 1.36e-04, 1.32e-04, 1.32e-04, 1.44e-04, 1.56e-04, 1.70e-04 };
  
  float meanEBEB_Boff[6]    = { 8.4111e-03, 7.9081e-03, 6.8028e-03, 4.5533e-03, 3.9257e-03, 3.0992e-03 };
  float meanErrEBEB_Boff[6] = { 1.07e-04, 1.02e-04, 1.00e-04, 9.20e-05, 9.26e-05, 8.54e-05 };
  float meanEBEE_Boff[6]    = { 2.3808e-03, 2.7055e-03, 1.5437e-03, -3.5864e-04, -2.0564e-03, -3.2928e-03};
  float meanErrEBEE_Boff[6] = { 1.80e-04, 1.78e-04, 1.72e-04, 1.79e-04, 1.97e-04, 2.19e-04 };  

  float meanEBEB_Diff[6], meanErrEBEB_Diff[6];
  float meanEBEE_Diff[6], meanErrEBEE_Diff[6];
  for (int ii=0; ii<6; ii++) {
    meanEBEB_Diff[ii] = meanEBEB_Bon[ii] - meanEBEB_Boff[ii];
    meanErrEBEB_Diff[ii] = sqrt( meanErrEBEB_Bon[ii]*meanErrEBEB_Bon[ii] + meanErrEBEB_Boff[ii]*meanErrEBEB_Boff[ii] );
    meanEBEE_Diff[ii] = meanEBEE_Bon[ii] - meanEBEE_Boff[ii];
    meanErrEBEE_Diff[ii] = sqrt( meanErrEBEE_Bon[ii]*meanErrEBEE_Bon[ii] + meanErrEBEE_Boff[ii]*meanErrEBEE_Boff[ii] );
  }

  float sigmaEBEB_Bon[6]    = { 9.9355e-03, 9.8133e-03, 9.8827e-03, 9.9490e-03, 9.9833e-03, 1.0058e-02 };
  float sigmaErrEBEB_Bon[6] = { 7.14e-05, 6.61e-05, 6.51e-05, 6.21e-05, 6.10e-05, 5.90e-05 };
  float sigmaEBEE_Bon[6]    = { 1.5190e-02, 1.4617e-02, 1.5019e-02, 1.4855e-02, 1.4666e-02, 1.4795e-02 };
  float sigmaErrEBEE_Bon[6] = { 1.18e-04, 1.14e-04, 1.15e-04, 1.26e-04, 1.35e-04, 1.48e-04 };

  float sigmaEBEB_Boff[6]    = { 1.2200e-02, 1.1693e-02, 1.1652e-02, 1.1704e-02, 1.1590e-02, 1.1682e-02 };
  float sigmaErrEBEB_Boff[6] = { 1.13e-04, 1.22e-04, 1.07e-04, 1.00e-04, 9.82e-05, 8.85e-05 }; 
  float sigmaEBEE_Boff[6]    = { 1.7648e-02, 1.6803e-02, 1.6175e-02, 1.6221e-02, 1.5958e-02, 1.5445e-02 };
  float sigmaErrEBEE_Boff[6] = { 1.60e-04, 1.57e-04, 1.51e-04, 1.57e-04, 1.72e-04, 1.91e-04};
  
  float sigmaEBEB_Diff[6], sigmaErrEBEB_Diff[6];
  float sigmaEBEE_Diff[6], sigmaErrEBEE_Diff[6];
  for (int ii=0; ii<6; ii++) {
    sigmaEBEB_Diff[ii] = fabs( (sigmaEBEB_Bon[ii] - sigmaEBEB_Boff[ii])/sigmaEBEB_Bon[ii]);
    sigmaErrEBEB_Diff[ii] = sqrt( sigmaErrEBEB_Bon[ii]*sigmaErrEBEB_Bon[ii] + sigmaErrEBEB_Boff[ii]*sigmaErrEBEB_Boff[ii] );
    sigmaEBEE_Diff[ii] = fabs( (sigmaEBEE_Bon[ii] - sigmaEBEE_Boff[ii])/sigmaEBEE_Bon[ii]);
    sigmaErrEBEE_Diff[ii] = sqrt( sigmaErrEBEE_Bon[ii]*sigmaErrEBEE_Bon[ii] + sigmaErrEBEE_Boff[ii]*sigmaErrEBEE_Boff[ii] );
  }

  TGraphErrors *grMeanEBEB_Bon = new TGraphErrors(6, mass, meanEBEB_Bon, massErr, meanErrEBEB_Bon );
  grMeanEBEB_Bon->SetMarkerColor(kRed);
  grMeanEBEB_Bon->SetMarkerStyle(20);
  grMeanEBEB_Bon->SetMarkerSize(1.);

  TGraphErrors *grMeanEBEB_Boff = new TGraphErrors(6, mass, meanEBEB_Boff, massErr, meanErrEBEB_Boff );
  grMeanEBEB_Boff->SetMarkerColor(kBlue);
  grMeanEBEB_Boff->SetMarkerStyle(20);
  grMeanEBEB_Boff->SetMarkerSize(1.);

  TGraphErrors *grMeanEBEB_Diff = new TGraphErrors(6, mass, meanEBEB_Diff, massErr, meanErrEBEB_Diff);
  grMeanEBEB_Diff->SetMarkerColor(kBlue);
  grMeanEBEB_Diff->SetMarkerStyle(20);
  grMeanEBEB_Diff->SetMarkerSize(1.);

  TGraphErrors *grMeanEBEE_Bon = new TGraphErrors(6, mass, meanEBEE_Bon, massErr, meanErrEBEE_Bon );
  grMeanEBEE_Bon->SetMarkerColor(kRed);
  grMeanEBEE_Bon->SetMarkerStyle(20);
  grMeanEBEE_Bon->SetMarkerSize(1.);

  TGraphErrors *grMeanEBEE_Boff = new TGraphErrors(6, mass, meanEBEE_Boff, massErr, meanErrEBEE_Boff );
  grMeanEBEE_Boff->SetMarkerColor(kBlue);
  grMeanEBEE_Boff->SetMarkerStyle(20);
  grMeanEBEE_Boff->SetMarkerSize(1.);

  TGraphErrors *grMeanEBEE_Diff = new TGraphErrors(6, mass, meanEBEE_Diff, massErr, meanErrEBEE_Diff);
  grMeanEBEE_Diff->SetMarkerColor(kBlue);
  grMeanEBEE_Diff->SetMarkerStyle(20);
  grMeanEBEE_Diff->SetMarkerSize(1.);



  TGraphErrors *grSigmaEBEB_Bon = new TGraphErrors(6, mass, sigmaEBEB_Bon, massErr, sigmaErrEBEB_Bon );
  grSigmaEBEB_Bon->SetMarkerColor(kRed);
  grSigmaEBEB_Bon->SetMarkerStyle(20);
  grSigmaEBEB_Bon->SetMarkerSize(1.);

  TGraphErrors *grSigmaEBEB_Boff = new TGraphErrors(6, mass, sigmaEBEB_Boff, massErr, sigmaErrEBEB_Boff );
  grSigmaEBEB_Boff->SetMarkerColor(kBlue);
  grSigmaEBEB_Boff->SetMarkerStyle(20);
  grSigmaEBEB_Boff->SetMarkerSize(1.);

  TGraphErrors *grSigmaEBEB_Diff = new TGraphErrors(6, mass, sigmaEBEB_Diff, massErr, sigmaErrEBEB_Diff);
  grSigmaEBEB_Diff->SetMarkerColor(kBlue);
  grSigmaEBEB_Diff->SetMarkerStyle(20);
  grSigmaEBEB_Diff->SetMarkerSize(1.);

  TGraphErrors *grSigmaEBEE_Bon = new TGraphErrors(6, mass, sigmaEBEE_Bon, massErr, sigmaErrEBEE_Bon );
  grSigmaEBEE_Bon->SetMarkerColor(kRed);
  grSigmaEBEE_Bon->SetMarkerStyle(20);
  grSigmaEBEE_Bon->SetMarkerSize(1.);

  TGraphErrors *grSigmaEBEE_Boff = new TGraphErrors(6, mass, sigmaEBEE_Boff, massErr, sigmaErrEBEE_Boff );
  grSigmaEBEE_Boff->SetMarkerColor(kBlue);
  grSigmaEBEE_Boff->SetMarkerStyle(20);
  grSigmaEBEE_Boff->SetMarkerSize(1.);

  TGraphErrors *grSigmaEBEE_Diff = new TGraphErrors(6, mass, sigmaEBEE_Diff, massErr, sigmaErrEBEE_Diff);
  grSigmaEBEE_Diff->SetMarkerColor(kBlue);
  grSigmaEBEE_Diff->SetMarkerStyle(20);
  grSigmaEBEE_Diff->SetMarkerSize(1.);


  TH2F *hMeanEBEB      = new TH2F("hMeanEBEB",    "EBEB",     100,400,2600,100,-0.011, 0.011);
  TH2F *hMeanEBEBDiff  = new TH2F("hMeanEBEBDiff","EBEB",100,400,2600,100,-0.015, -0.005);
  TH2F *hMeanEBEE      = new TH2F("hMeanEBEE",    "EBEE",     100,400,2600,100,-0.011, 0.011);
  TH2F *hMeanEBEEDiff  = new TH2F("hMeanEBEEDiff","EBEE",100,400,2600,100,-0.01, 0.005);
  hMeanEBEB->GetXaxis()->SetTitle("m_{X}");
  hMeanEBEE->GetXaxis()->SetTitle("m_{X}");
  hMeanEBEB->GetYaxis()->SetTitle("#Deltam/m");
  hMeanEBEE->GetYaxis()->SetTitle("#Deltam/m");
  hMeanEBEBDiff->GetXaxis()->SetTitle("m_{X}");
  hMeanEBEEDiff->GetXaxis()->SetTitle("m_{X}");
  hMeanEBEBDiff->GetYaxis()->SetTitle("(#Deltam/m)_{ON} - (#Deltam/m)_{OFF}");
  hMeanEBEEDiff->GetYaxis()->SetTitle("(#Deltam/m)_{ON} - (#Deltam/m)_{OFF}");
  hMeanEBEBDiff->GetYaxis()->SetTitleOffset(1.35);
  hMeanEBEEDiff->GetYaxis()->SetTitleOffset(1.35);

  TH2F *hSigmaEBEB      = new TH2F("hSigmaEBEB",    "EBEB",     100,400,2600,100,0.0085, 0.0135);
  TH2F *hSigmaEBEBDiff  = new TH2F("hSigmaEBEBDiff","EBEB",100,400,2600,100,0.13, 0.26);
  TH2F *hSigmaEBEE      = new TH2F("hSigmaEBEE",    "EBEE",     100,400,2600,100,0.0135, 0.0185);
  TH2F *hSigmaEBEEDiff  = new TH2F("hSigmaEBEEDiff","EBEE",100,400,2600,100,0.04, 0.17);
  hSigmaEBEB->GetXaxis()->SetTitle("m_{X}");
  hSigmaEBEE->GetXaxis()->SetTitle("m_{X}");
  hSigmaEBEB->GetYaxis()->SetTitle("#sigma m");
  hSigmaEBEE->GetYaxis()->SetTitle("#sigma m");
  hSigmaEBEBDiff->GetXaxis()->SetTitle("m_{X}");
  hSigmaEBEEDiff->GetXaxis()->SetTitle("m_{X}");
  hSigmaEBEBDiff->GetYaxis()->SetTitle("| (#sigma m)_{ON} - (#sigma m)_{OFF} | / (#sigma m)_{ON}");
  hSigmaEBEEDiff->GetYaxis()->SetTitle("| (#sigma m)_{ON} - (#sigma m)_{OFF} | / (#sigma m)_{ON}");
  hSigmaEBEBDiff->GetYaxis()->SetTitleOffset(1.35);
  hSigmaEBEEDiff->GetYaxis()->SetTitleOffset(1.35);

  TCanvas c1("meanEBEB","EBEB",1);
  c1.cd();
  hMeanEBEB->Draw();
  grMeanEBEB_Bon->Draw("P");
  grMeanEBEB_Boff->Draw("P");
  c1.SaveAs("meanEBEB.png");

  TCanvas c2("meanEBEBDiff","EBEB",1);
  c2.cd();
  hMeanEBEBDiff->Draw();
  grMeanEBEB_Diff->Draw("P");
  grMeanEBEB_Diff->Fit("pol1");
  c2.SaveAs("meanEBEBDiff.png");
  c2.SaveAs("meanEBEBDiff.root");

  TCanvas c1b("meanEBEE","EBEE",1);
  c1b.cd();
  hMeanEBEE->Draw();
  grMeanEBEE_Bon->Draw("P");
  grMeanEBEE_Boff->Draw("P");
  c1b.SaveAs("meanEBEE.png");

  TCanvas c2b("meanEBEEDiff","EBEE",1);
  c2b.cd();
  hMeanEBEEDiff->Draw();
  grMeanEBEE_Diff->Draw("P");
  grMeanEBEE_Diff->Fit("pol1");
  c2b.SaveAs("meanEBEEDiff.png");

  TCanvas c11("sigmaEBEB","EBEB",1);
  c11.cd();
  hSigmaEBEB->Draw();
  grSigmaEBEB_Bon->Draw("P");
  grSigmaEBEB_Boff->Draw("P");
  c11.SaveAs("sigmaEBEB.png");

  TCanvas c12("sigmaEBEBDiff","EBEB",1);
  c12.cd();
  hSigmaEBEBDiff->Draw();
  grSigmaEBEB_Diff->Draw("P");
  c12.SaveAs("sigmaEBEBDiff.png");

  TCanvas c11b("sigmaEBEE","EBEE",1);
  c11b.cd();
  hSigmaEBEE->Draw();
  grSigmaEBEE_Bon->Draw("P");
  grSigmaEBEE_Boff->Draw("P");
  c11b.SaveAs("sigmaEBEE.png");

  TCanvas c12b("sigmaEBEEDiff","EBEE",1);
  c12b.cd();
  hSigmaEBEEDiff->Draw();
  grSigmaEBEE_Diff->Draw("P");
  c12b.SaveAs("sigmaEBEEDiff.png");
  
}
