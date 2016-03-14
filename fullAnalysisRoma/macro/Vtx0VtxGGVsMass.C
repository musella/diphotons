#include "TString.h"
#include "TGraphErrors.h"
#include "TCanvas.h"
#include "TH2F.h"
#include "TLegend.h"
#include "TLatex.h"
#include <iostream>

using namespace std;

void Vtx0VtxGGVsMass(){

  float mass[6]    = { 500, 750, 1000, 1500, 2000, 2500 };
  float massErr[6] = { 0, 0, 0, 0, 0, 0 };
  
  float meanEBEB_VtxGG[6]    = { -1.9410e-03, -2.9600e-03, -4.3510e-03, -6.4133e-03, -7.5669e-03, -8.5902e-03 };
  float meanErrEBEB_VtxGG[6] = { 7.88e-05, 7.30e-05, 7.24e-05, 6.94e-05, 6.81e-05, 6.66e-05 };
  float meanEBEE_VtxGG[6]    = { -1.9224e-03, -2.3328e-03, -3.6587e-03, -5.0002e-03, -6.1919e-03, -7.3895e-03};
  float meanErrEBEE_VtxGG[6] = { 1.38e-04, 1.32e-04, 1.33e-04, 1.45e-04, 1.61e-04, 1.72e-04 };
  
  float meanEBEB_Vtx0[6]    = { -1.9826e-03, -3.0319e-03, -4.3947e-03, -6.4182e-03, -7.6221e-03, -8.6255e-03 };
  float meanErrEBEB_Vtx0[6] = { 8.09e-05, 7.39e-05, 7.48e-05, 7.00e-05, 6.81e-05, 6.74e-05 };
  float meanEBEE_Vtx0[6]    = { -1.9474e-03, -2.4553e-03, -3.7178e-03, -5.1301e-03, -6.2364e-03, -7.3990e-03 };
  float meanErrEBEE_Vtx0[6] = { 1.42e-04, 1.36e-04, 1.37e-04, 1.49e-04, 1.61e-04, 1.76e-04 };

  float meanEBEB_Diff[6], meanErrEBEB_Diff[6];
  float meanEBEE_Diff[6], meanErrEBEE_Diff[6];
  for (int ii=0; ii<6; ii++) {
    meanEBEB_Diff[ii] = meanEBEB_VtxGG[ii] - meanEBEB_Vtx0[ii];
    meanErrEBEB_Diff[ii] = sqrt( meanErrEBEB_VtxGG[ii]*meanErrEBEB_VtxGG[ii] + meanErrEBEB_Vtx0[ii]*meanErrEBEB_Vtx0[ii] );
    meanEBEE_Diff[ii] = meanEBEE_VtxGG[ii] - meanEBEE_Vtx0[ii];
    meanErrEBEE_Diff[ii] = sqrt( meanErrEBEE_VtxGG[ii]*meanErrEBEE_VtxGG[ii] + meanErrEBEE_Vtx0[ii]*meanErrEBEE_Vtx0[ii] );
  }

  float sigmaEBEB_VtxGG[6]    = { 9.4509e-03, 9.3427e-03, 9.4044e-03, 9.4735e-03, 9.5000e-03, 9.5935e-03 };
  float sigmaErrEBEB_VtxGG[6] = { 8.01e-05, 7.23e-05, 8.89e-05, 6.05e-05, 5.90e-05, 5.70e-05 };
  float sigmaEBEE_VtxGG[6]    = { 1.5204e-02, 1.4702e-02, 1.5114e-02, 1.4972e-02, 1.4874e-02, 1.5020e-02 };
  float sigmaErrEBEE_VtxGG[6] = { 1.21e-04, 1.15e-04, 1.17e-04, 1.28e-04, 1.40e-04, 1.49e-04 };

  float sigmaEBEB_Vtx0[6]    = { 9.5863e-03, 9.4784e-03, 9.5320e-03, 9.5724e-03, 9.6434e-03, 9.6668e-03 };
  float sigmaErrEBEB_Vtx0[6] = { 8.31e-05, 7.53e-05, 7.81e-05, 6.13e-05, 5.91e-05, 5.77e-05 };
  float sigmaEBEE_Vtx0[6]    = { 1.5411e-02, 1.4945e-02, 1.5347e-02, 1.5214e-02, 1.4925e-02, 1.5073e-02 };
  float sigmaErrEBEE_Vtx0[6] = { 1.26e-04, 1.19e-04, 1.20e-04, 1.32e-04, 1.39e-04, 1.54e-04 };
  
  float sigmaEBEB_Diff[6], sigmaErrEBEB_Diff[6];
  float sigmaEBEE_Diff[6], sigmaErrEBEE_Diff[6];
  for (int ii=0; ii<6; ii++) {
    sigmaEBEB_Diff[ii] = fabs( (sigmaEBEB_VtxGG[ii] - sigmaEBEB_Vtx0[ii])/sigmaEBEB_VtxGG[ii] );
    sigmaErrEBEB_Diff[ii] = sqrt( sigmaErrEBEB_VtxGG[ii]*sigmaErrEBEB_VtxGG[ii] + sigmaErrEBEB_Vtx0[ii]*sigmaErrEBEB_Vtx0[ii] );
    sigmaEBEE_Diff[ii] = fabs( (sigmaEBEE_VtxGG[ii] - sigmaEBEE_Vtx0[ii])/sigmaEBEE_VtxGG[ii] );
    sigmaErrEBEE_Diff[ii] = sqrt( sigmaErrEBEE_VtxGG[ii]*sigmaErrEBEE_VtxGG[ii] + sigmaErrEBEE_Vtx0[ii]*sigmaErrEBEE_Vtx0[ii] );
  }

  TGraphErrors *grMeanEBEB_VtxGG = new TGraphErrors(6, mass, meanEBEB_VtxGG, massErr, meanErrEBEB_VtxGG );
  grMeanEBEB_VtxGG->SetMarkerColor(kRed);
  grMeanEBEB_VtxGG->SetMarkerStyle(20);
  grMeanEBEB_VtxGG->SetMarkerSize(1.);

  TGraphErrors *grMeanEBEB_Vtx0 = new TGraphErrors(6, mass, meanEBEB_Vtx0, massErr, meanErrEBEB_Vtx0 );
  grMeanEBEB_Vtx0->SetMarkerColor(kBlue);
  grMeanEBEB_Vtx0->SetMarkerStyle(20);
  grMeanEBEB_Vtx0->SetMarkerSize(1.);

  TGraphErrors *grMeanEBEB_Diff = new TGraphErrors(6, mass, meanEBEB_Diff, massErr, meanErrEBEB_Diff);
  grMeanEBEB_Diff->SetMarkerColor(kBlue);
  grMeanEBEB_Diff->SetMarkerStyle(20);
  grMeanEBEB_Diff->SetMarkerSize(1.);

  TGraphErrors *grMeanEBEE_VtxGG = new TGraphErrors(6, mass, meanEBEE_VtxGG, massErr, meanErrEBEE_VtxGG );
  grMeanEBEE_VtxGG->SetMarkerColor(kRed);
  grMeanEBEE_VtxGG->SetMarkerStyle(20);
  grMeanEBEE_VtxGG->SetMarkerSize(1.);

  TGraphErrors *grMeanEBEE_Vtx0 = new TGraphErrors(6, mass, meanEBEE_Vtx0, massErr, meanErrEBEE_Vtx0 );
  grMeanEBEE_Vtx0->SetMarkerColor(kBlue);
  grMeanEBEE_Vtx0->SetMarkerStyle(20);
  grMeanEBEE_Vtx0->SetMarkerSize(1.);

  TGraphErrors *grMeanEBEE_Diff = new TGraphErrors(6, mass, meanEBEE_Diff, massErr, meanErrEBEE_Diff);
  grMeanEBEE_Diff->SetMarkerColor(kBlue);
  grMeanEBEE_Diff->SetMarkerStyle(20);
  grMeanEBEE_Diff->SetMarkerSize(1.);



  TGraphErrors *grSigmaEBEB_VtxGG = new TGraphErrors(6, mass, sigmaEBEB_VtxGG, massErr, sigmaErrEBEB_VtxGG );
  grSigmaEBEB_VtxGG->SetMarkerColor(kRed);
  grSigmaEBEB_VtxGG->SetMarkerStyle(20);
  grSigmaEBEB_VtxGG->SetMarkerSize(1.);

  TGraphErrors *grSigmaEBEB_Vtx0 = new TGraphErrors(6, mass, sigmaEBEB_Vtx0, massErr, sigmaErrEBEB_Vtx0 );
  grSigmaEBEB_Vtx0->SetMarkerColor(kBlue);
  grSigmaEBEB_Vtx0->SetMarkerStyle(20);
  grSigmaEBEB_Vtx0->SetMarkerSize(1.);

  TGraphErrors *grSigmaEBEB_Diff = new TGraphErrors(6, mass, sigmaEBEB_Diff, massErr, sigmaErrEBEB_Diff);
  grSigmaEBEB_Diff->SetMarkerColor(kBlue);
  grSigmaEBEB_Diff->SetMarkerStyle(20);
  grSigmaEBEB_Diff->SetMarkerSize(1.);

  TGraphErrors *grSigmaEBEE_VtxGG = new TGraphErrors(6, mass, sigmaEBEE_VtxGG, massErr, sigmaErrEBEE_VtxGG );
  grSigmaEBEE_VtxGG->SetMarkerColor(kRed);
  grSigmaEBEE_VtxGG->SetMarkerStyle(20);
  grSigmaEBEE_VtxGG->SetMarkerSize(1.);

  TGraphErrors *grSigmaEBEE_Vtx0 = new TGraphErrors(6, mass, sigmaEBEE_Vtx0, massErr, sigmaErrEBEE_Vtx0 );
  grSigmaEBEE_Vtx0->SetMarkerColor(kBlue);
  grSigmaEBEE_Vtx0->SetMarkerStyle(20);
  grSigmaEBEE_Vtx0->SetMarkerSize(1.);

  TGraphErrors *grSigmaEBEE_Diff = new TGraphErrors(6, mass, sigmaEBEE_Diff, massErr, sigmaErrEBEE_Diff);
  grSigmaEBEE_Diff->SetMarkerColor(kBlue);
  grSigmaEBEE_Diff->SetMarkerStyle(20);
  grSigmaEBEE_Diff->SetMarkerSize(1.);


  TH2F *hMeanEBEB      = new TH2F("hMeanEBEB",    "EBEB",     100,400,2600,100,-0.011, 0.011);
  TH2F *hMeanEBEBDiff  = new TH2F("hMeanEBEBDiff","EBEB",     100,400,2600,100,-0.005, 0.005);
  TH2F *hMeanEBEE      = new TH2F("hMeanEBEE",    "EBEE",     100,400,2600,100,-0.011, 0.011);
  TH2F *hMeanEBEEDiff  = new TH2F("hMeanEBEEDiff","EBEE",     100,400,2600,100,-0.005, 0.005);
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

  TH2F *hSigmaEBEB      = new TH2F("hSigmaEBEB",    "EBEB",     100,400,2600,100,0.0085, 0.0115);
  TH2F *hSigmaEBEBDiff  = new TH2F("hSigmaEBEBDiff","EBEB",     100,400,2600,100,0.002, 0.022);
  TH2F *hSigmaEBEE      = new TH2F("hSigmaEBEE",    "EBEE",     100,400,2600,100,0.0135, 0.0165);
  TH2F *hSigmaEBEEDiff  = new TH2F("hSigmaEBEEDiff","EBEE",     100,400,2600,100,0.002, 0.022);
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
  grMeanEBEB_VtxGG->Draw("P");
  grMeanEBEB_Vtx0->Draw("P");
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
  grMeanEBEE_VtxGG->Draw("P");
  grMeanEBEE_Vtx0->Draw("P");
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
  grSigmaEBEB_VtxGG->Draw("P");
  grSigmaEBEB_Vtx0->Draw("P");
  c11.SaveAs("sigmaEBEB.png");

  TCanvas c12("sigmaEBEBDiff","EBEB",1);
  c12.cd();
  hSigmaEBEBDiff->Draw();
  grSigmaEBEB_Diff->Draw("P");
  c12.SaveAs("sigmaEBEBDiff.png");

  TCanvas c11b("sigmaEBEE","EBEE",1);
  c11b.cd();
  hSigmaEBEE->Draw();
  grSigmaEBEE_VtxGG->Draw("P");
  grSigmaEBEE_Vtx0->Draw("P");
  c11b.SaveAs("sigmaEBEE.png");

  TCanvas c12b("sigmaEBEEDiff","EBEE",1);
  c12b.cd();
  hSigmaEBEEDiff->Draw();
  grSigmaEBEE_Diff->Draw("P");
  c12b.SaveAs("sigmaEBEEDiff.png");
  
}
