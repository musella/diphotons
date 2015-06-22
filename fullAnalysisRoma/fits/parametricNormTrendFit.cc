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

void parametricNormTrendFit() {

  float lumi = 1.;      // pb

  float mass[4]  = {750, 1500, 3000, 5000};
  float massE[4] = {0, 0, 0, 0};
  
  float xsec[4] = { 5.07e-2, 0.133, 1.5e-3, 1.42e-7 };    // pb

  // yields
  float yield_all[4]    = { 0.023872,   0.0638639,  0.000772451,  7.98451e-08};
  float yield_class0[4] = { 0.00945885, 0.03377,    0.000572505,  6.82968e-08};
  float yield_class1[4] = { 0.00493643, 0.010515,   8.95818e-05,  7.30566e-09};
  float yield_class2[4] = { 0.0067995,  0.0155964,  9.38833e-05,  3.7328e-09 };
  float yield_class3[4] = { 0.00267723, 0.00398253, 1.64809e-05,  5.09893e-10 };

  // errors
  float yieldE_all[4], yieldE_class0[4], yieldE_class1[4], yieldE_class2[4], yieldE_class3[4];
  for (int ii=0; ii<4; ii++) {
    yieldE_all[ii]    = sqrt(yield_all[ii]);
    yieldE_class0[ii] = sqrt(yield_class0[ii]);
    yieldE_class1[ii] = sqrt(yield_class1[ii]);
    yieldE_class2[ii] = sqrt(yield_class2[ii]);
    yieldE_class3[ii] = sqrt(yield_class3[ii]);
  }

  // Efficiency x Acceptance
  float ExA_all[4], ExA_class0[4], ExA_class1[4], ExA_class2[4], ExA_class3[4];
  for (int ii=0; ii<4; ii++) {
    ExA_all[ii]    = yield_all[ii] / (xsec[ii]*lumi);
    ExA_class0[ii] = yield_class0[ii] / (xsec[ii]*lumi);
    ExA_class1[ii] = yield_class1[ii] / (xsec[ii]*lumi);
    ExA_class2[ii] = yield_class2[ii] / (xsec[ii]*lumi);
    ExA_class3[ii] = yield_class3[ii] / (xsec[ii]*lumi);
  }

  // Errors on ExA
  float ExAE_all[4], ExAE_class0[4], ExAE_class1[4], ExAE_class2[4], ExAE_class3[4];  
  /*
  for (int ii=0; ii<4; ii++) {
    //ExAE_all[ii]    = ExA_all[ii] * yieldE_all[ii] / yield_all[ii];
    ExAE_class0[ii] = ExA_class0[ii] * yieldE_class0[ii] / yield_class0[ii];
    ExAE_class1[ii] = ExA_class1[ii] * yieldE_class1[ii] / yield_class1[ii];
    ExAE_class2[ii] = ExA_class2[ii] * yieldE_class2[ii] / yield_class2[ii];
    ExAE_class3[ii] = ExA_class3[ii] * yieldE_class3[ii] / yield_class3[ii];
  }
  */
  // dummy
  for (int ii=0; ii<4; ii++) {
    ExAE_all[ii]    = 0.000000001;
    ExAE_class0[ii] = 0.000000001;
    ExAE_class1[ii] = 0.000000001;
    ExAE_class2[ii] = 0.000000001;
    ExAE_class3[ii] = 0.000000001;
  }
  
  // take graphs
  TGraphErrors *gExA_all  = new TGraphErrors(4,mass,ExA_all, massE, ExAE_all);
  TGraphErrors *gExA_cat0 = new TGraphErrors(4,mass,ExA_class0, massE, ExAE_class0);
  TGraphErrors *gExA_cat1 = new TGraphErrors(4,mass,ExA_class1, massE, ExAE_class1);
  TGraphErrors *gExA_cat2 = new TGraphErrors(4,mass,ExA_class2, massE, ExAE_class2);
  TGraphErrors *gExA_cat3 = new TGraphErrors(4,mass,ExA_class3, massE, ExAE_class3);

  // cosmetics
  gExA_all->SetMarkerStyle(20);    gExA_all->SetMarkerColor(5);
  gExA_cat0->SetMarkerStyle(21);   gExA_cat0->SetMarkerColor(1);
  gExA_cat1->SetMarkerStyle(21);   gExA_cat1->SetMarkerColor(2);
  gExA_cat2->SetMarkerStyle(21);   gExA_cat2->SetMarkerColor(3);
  gExA_cat3->SetMarkerStyle(21);   gExA_cat3->SetMarkerColor(4);

  // Now do the fits with a pol2
  TF1* f1_mass = new TF1("f1_mass","[0]+[1]*x+[2]*x*x", 500., 5100.);

  gExA_all->Fit("f1_mass");
  gExA_cat0->Fit("f1_mass");
  gExA_cat1->Fit("f1_mass");
  gExA_cat2->Fit("f1_mass");
  gExA_cat3->Fit("f1_mass");
  TF1 *exa_all  = gExA_all->GetFunction("f1_mass");
  TF1 *exa_cat0 = gExA_cat0->GetFunction("f1_mass");
  TF1 *exa_cat1 = gExA_cat1->GetFunction("f1_mass");
  TF1 *exa_cat2 = gExA_cat2->GetFunction("f1_mass");
  TF1 *exa_cat3 = gExA_cat3->GetFunction("f1_mass");
  exa_all->SetLineColor(5);
  exa_cat0->SetLineColor(1);
  exa_cat1->SetLineColor(2);
  exa_cat2->SetLineColor(3);
  exa_cat3->SetLineColor(4);

  // and finally plots
  gStyle->SetOptStat(0);

  TLegend *leg;
  leg = new TLegend(0.10,0.65,0.35,0.90);
  leg->SetFillStyle(0);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.05);
  leg->SetFillColor(0);
  leg->AddEntry(gExA_all,  "all events", "lp");
  leg->AddEntry(gExA_cat0, "class 0", "lp");
  leg->AddEntry(gExA_cat1, "class 1", "lp");
  leg->AddEntry(gExA_cat2, "class 2", "lp");
  leg->AddEntry(gExA_cat3, "class 3", "lp");
  
  TH2F *myH2 = new TH2F("myH2","", 100, 0., 5100, 100, 0.000001, 1.);

  TCanvas myCanvas0("myCanvas0","",1);
  myH2->Draw();
  myH2->GetXaxis()->SetTitle("diphoton mass");
  myH2->GetYaxis()->SetTitle("efficiency x Acceptance");
  gExA_all->Draw("PEsame");
  gExA_cat0->Draw("PEsame");
  gExA_cat1->Draw("PEsame");
  gExA_cat2->Draw("PEsame");
  gExA_cat3->Draw("PEsame");
  leg->Draw();
  myCanvas0.SaveAs("effTimesAccTrend.png");


  // Save parameterizations
  TFile myParaFile("normalizationEvolution.root","RECREATE");
  exa_cat0->Write("exa_cat0");
  exa_cat1->Write("exa_cat1");
  exa_cat2->Write("exa_cat2");
  exa_cat3->Write("exa_cat3");

  
}
