#include <TFile.h>
#include <TTree.h>
#include <TH1.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TLegend.h>
#include <iostream>

using namespace std;

void parametricPdfTrendFit() {

  // input files
  // TFile *fileIn = new TFile("outTrends_allFree.root");
  TFile *fileIn = new TFile("outTrends_nFix.root");

  // take graphs
  TGraph *gMean_cat0   = (TGraph*)fileIn->Get("gMean_cat0");
  TGraph *gSigma_cat0  = (TGraph*)fileIn->Get("gSigma_cat0");
  TGraph *gAlpha1_cat0 = (TGraph*)fileIn->Get("gAlpha1_cat0");
  TGraph *gAlpha2_cat0 = (TGraph*)fileIn->Get("gAlpha2_cat0");
  TGraph *gN1_cat0     = (TGraph*)fileIn->Get("gN1_cat0");
  TGraph *gN2_cat0     = (TGraph*)fileIn->Get("gN2_cat0");
  //
  TGraph *gMean_cat1   = (TGraph*)fileIn->Get("gMean_cat1");
  TGraph *gSigma_cat1  = (TGraph*)fileIn->Get("gSigma_cat1");
  TGraph *gAlpha1_cat1 = (TGraph*)fileIn->Get("gAlpha1_cat1");
  TGraph *gAlpha2_cat1 = (TGraph*)fileIn->Get("gAlpha2_cat1");
  TGraph *gN1_cat1     = (TGraph*)fileIn->Get("gN1_cat1");
  TGraph *gN2_cat1     = (TGraph*)fileIn->Get("gN2_cat1");
  //
  TGraph *gMean_cat2   = (TGraph*)fileIn->Get("gMean_cat2");
  TGraph *gSigma_cat2  = (TGraph*)fileIn->Get("gSigma_cat2");
  TGraph *gAlpha1_cat2 = (TGraph*)fileIn->Get("gAlpha1_cat2");
  TGraph *gAlpha2_cat2 = (TGraph*)fileIn->Get("gAlpha2_cat2");
  TGraph *gN1_cat2     = (TGraph*)fileIn->Get("gN1_cat2");
  TGraph *gN2_cat2     = (TGraph*)fileIn->Get("gN2_cat2");
  //
  TGraph *gMean_cat3   = (TGraph*)fileIn->Get("gMean_cat3");
  TGraph *gSigma_cat3  = (TGraph*)fileIn->Get("gSigma_cat3");
  TGraph *gAlpha1_cat3 = (TGraph*)fileIn->Get("gAlpha1_cat3");
  TGraph *gAlpha2_cat3 = (TGraph*)fileIn->Get("gAlpha2_cat3");
  TGraph *gN1_cat3     = (TGraph*)fileIn->Get("gN1_cat3");
  TGraph *gN2_cat3     = (TGraph*)fileIn->Get("gN2_cat3");


  // cosmetics
  gMean_cat0->SetMarkerStyle(21);   gMean_cat0->SetMarkerColor(1);
  gSigma_cat0->SetMarkerStyle(21);  gSigma_cat0->SetMarkerColor(1);
  gAlpha1_cat0->SetMarkerStyle(21); gAlpha1_cat0->SetMarkerColor(1);
  gAlpha2_cat0->SetMarkerStyle(21); gAlpha2_cat0->SetMarkerColor(1);
  gN1_cat0->SetMarkerStyle(21);     gN1_cat0->SetMarkerColor(1);
  gN2_cat0->SetMarkerStyle(21);     gN2_cat0->SetMarkerColor(1);

  gMean_cat1->SetMarkerStyle(21);   gMean_cat1->SetMarkerColor(2);
  gSigma_cat1->SetMarkerStyle(21);  gSigma_cat1->SetMarkerColor(2);
  gAlpha1_cat1->SetMarkerStyle(21); gAlpha1_cat1->SetMarkerColor(2);
  gAlpha2_cat1->SetMarkerStyle(21); gAlpha2_cat1->SetMarkerColor(2);
  gN1_cat1->SetMarkerStyle(21);     gN1_cat1->SetMarkerColor(2);
  gN2_cat1->SetMarkerStyle(21);     gN2_cat1->SetMarkerColor(2);

  gMean_cat2->SetMarkerStyle(21);   gMean_cat2->SetMarkerColor(3);
  gSigma_cat2->SetMarkerStyle(21);  gSigma_cat2->SetMarkerColor(3);
  gAlpha1_cat2->SetMarkerStyle(21); gAlpha1_cat2->SetMarkerColor(3);
  gAlpha2_cat2->SetMarkerStyle(21); gAlpha2_cat2->SetMarkerColor(3);
  gN1_cat2->SetMarkerStyle(21);     gN1_cat2->SetMarkerColor(3);
  gN2_cat2->SetMarkerStyle(21);     gN2_cat2->SetMarkerColor(3);

  gMean_cat3->SetMarkerStyle(21);   gMean_cat3->SetMarkerColor(4);
  gSigma_cat3->SetMarkerStyle(21);  gSigma_cat3->SetMarkerColor(4);
  gAlpha1_cat3->SetMarkerStyle(21); gAlpha1_cat3->SetMarkerColor(4);
  gAlpha2_cat3->SetMarkerStyle(21); gAlpha2_cat3->SetMarkerColor(4);
  gN1_cat3->SetMarkerStyle(21);     gN1_cat3->SetMarkerColor(4);
  gN2_cat3->SetMarkerStyle(21);     gN2_cat3->SetMarkerColor(4);


  // Now do the fits with a pol2
  TF1* f1_mass = new TF1("f1_mass","[0]+[1]*x+[2]*x*x", 300., 6000.);

  gMean_cat0->Fit("f1_mass");
  gMean_cat1->Fit("f1_mass");
  gMean_cat2->Fit("f1_mass");
  gMean_cat3->Fit("f1_mass");
  TF1 *mean_cat0 = gMean_cat0->GetFunction("f1_mass");
  TF1 *mean_cat1 = gMean_cat1->GetFunction("f1_mass");
  TF1 *mean_cat2 = gMean_cat2->GetFunction("f1_mass");
  TF1 *mean_cat3 = gMean_cat3->GetFunction("f1_mass");
  mean_cat0->SetLineColor(1);
  mean_cat1->SetLineColor(2);
  mean_cat2->SetLineColor(3);
  mean_cat3->SetLineColor(4);

  gSigma_cat0->Fit("f1_mass");
  gSigma_cat1->Fit("f1_mass");
  gSigma_cat2->Fit("f1_mass");
  gSigma_cat3->Fit("f1_mass");
  TF1 *sigma_cat0 = gSigma_cat0->GetFunction("f1_mass");
  TF1 *sigma_cat1 = gSigma_cat1->GetFunction("f1_mass");
  TF1 *sigma_cat2 = gSigma_cat2->GetFunction("f1_mass");
  TF1 *sigma_cat3 = gSigma_cat3->GetFunction("f1_mass");
  sigma_cat0->SetLineColor(1);
  sigma_cat1->SetLineColor(2);
  sigma_cat2->SetLineColor(3);
  sigma_cat3->SetLineColor(4);

  gAlpha1_cat0->Fit("f1_mass");
  gAlpha1_cat1->Fit("f1_mass");
  gAlpha1_cat2->Fit("f1_mass");
  gAlpha1_cat3->Fit("f1_mass");
  TF1 *alpha1_cat0 = gAlpha1_cat0->GetFunction("f1_mass");
  TF1 *alpha1_cat1 = gAlpha1_cat1->GetFunction("f1_mass");
  TF1 *alpha1_cat2 = gAlpha1_cat2->GetFunction("f1_mass");
  TF1 *alpha1_cat3 = gAlpha1_cat3->GetFunction("f1_mass");
  alpha1_cat0->SetLineColor(1);
  alpha1_cat1->SetLineColor(2);
  alpha1_cat2->SetLineColor(3);
  alpha1_cat3->SetLineColor(4);

  gAlpha2_cat0->Fit("f1_mass");
  gAlpha2_cat1->Fit("f1_mass");
  gAlpha2_cat2->Fit("f1_mass");
  gAlpha2_cat3->Fit("f1_mass");
  TF1 *alpha2_cat0 = gAlpha2_cat0->GetFunction("f1_mass");
  TF1 *alpha2_cat1 = gAlpha2_cat1->GetFunction("f1_mass");
  TF1 *alpha2_cat2 = gAlpha2_cat2->GetFunction("f1_mass");
  TF1 *alpha2_cat3 = gAlpha2_cat3->GetFunction("f1_mass");
  alpha2_cat0->SetLineColor(1);
  alpha2_cat1->SetLineColor(2);
  alpha2_cat2->SetLineColor(3);
  alpha2_cat3->SetLineColor(4);
  
  gN1_cat0->Fit("f1_mass");
  gN1_cat1->Fit("f1_mass");
  gN1_cat2->Fit("f1_mass");
  gN1_cat3->Fit("f1_mass");
  TF1 *n1_cat0 = gN1_cat0->GetFunction("f1_mass");
  TF1 *n1_cat1 = gN1_cat1->GetFunction("f1_mass");
  TF1 *n1_cat2 = gN1_cat2->GetFunction("f1_mass");
  TF1 *n1_cat3 = gN1_cat3->GetFunction("f1_mass");
  n1_cat0->SetLineColor(1);
  n1_cat1->SetLineColor(2);
  n1_cat2->SetLineColor(3);
  n1_cat3->SetLineColor(4);

  gN2_cat0->Fit("f1_mass");
  gN2_cat1->Fit("f1_mass");
  gN2_cat2->Fit("f1_mass");
  gN2_cat3->Fit("f1_mass");
  TF1 *n2_cat0 = gN2_cat0->GetFunction("f1_mass");
  TF1 *n2_cat1 = gN2_cat1->GetFunction("f1_mass");
  TF1 *n2_cat2 = gN2_cat2->GetFunction("f1_mass");
  TF1 *n2_cat3 = gN2_cat3->GetFunction("f1_mass");
  n2_cat0->SetLineColor(1);
  n2_cat1->SetLineColor(2);
  n2_cat2->SetLineColor(3);
  n2_cat3->SetLineColor(4);

  // and finally plots
  gStyle->SetOptStat(0);

  TLegend *leg;
  leg = new TLegend(0.10,0.65,0.35,0.90);
  leg->SetFillStyle(0);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.05);
  leg->SetFillColor(0);
  leg->AddEntry(gMean_cat0, "class 0", "lp");
  leg->AddEntry(gMean_cat1, "class 1", "lp");
  leg->AddEntry(gMean_cat2, "class 2", "lp");
  leg->AddEntry(gMean_cat3, "class 3", "lp");
  
  TH2F *myH20  = new TH2F("myH20", "",100,0.,5100,100,-65.,5.);
  TH2F *myH21  = new TH2F("myH21", "",100,0.,5100,100,0.,100.);
  TH2F *myH223 = new TH2F("myH223","",100,0.,5100,100,0.,4.);
  TH2F *myH245 = new TH2F("myH245","",100,0.,5100,100,0.,20.);

  TCanvas myCanvas0("myCanvas0","",1);
  myH20->Draw();
  myH20->GetXaxis()->SetTitle("diphoton mass");
  myH20->GetYaxis()->SetTitle("CB mean");
  gMean_cat0->Draw("PEsame");
  gMean_cat1->Draw("PEsame");
  gMean_cat2->Draw("PEsame");
  gMean_cat3->Draw("PEsame");
  leg->Draw();
  myCanvas0.SaveAs("massTrend.png");

  TCanvas myCanvas1("myCanvas1","",1);
  myH21->Draw();
  myH21->GetXaxis()->SetTitle("diphoton mass");
  myH21->GetYaxis()->SetTitle("CB #sigma");
  gSigma_cat0->Draw("PEsame");
  gSigma_cat1->Draw("PEsame");
  gSigma_cat2->Draw("PEsame");
  gSigma_cat3->Draw("PEsame");
  leg->Draw();
  myCanvas1.SaveAs("sigmaTrend.png");

  TCanvas myCanvas2("myCanvas2","",1);
  myH223->Draw();
  myH223->GetXaxis()->SetTitle("diphoton mass");
  myH223->GetYaxis()->SetTitle("CB #alpha left");
  gAlpha1_cat0->Draw("PEsame");
  gAlpha1_cat1->Draw("PEsame");
  gAlpha1_cat2->Draw("PEsame");
  gAlpha1_cat3->Draw("PEsame");
  leg->Draw();
  myCanvas2.SaveAs("alpha1Trend.png");

  TCanvas myCanvas3("myCanvas3","",1);
  myH223->Draw();
  myH223->GetXaxis()->SetTitle("diphoton mass");
  myH223->GetYaxis()->SetTitle("CB #alpha right");
  gAlpha2_cat0->Draw("PEsame");
  gAlpha2_cat1->Draw("PEsame");
  gAlpha2_cat2->Draw("PEsame");
  gAlpha2_cat3->Draw("PEsame");
  leg->Draw();
  myCanvas3.SaveAs("alpha2Trend.png");

  TCanvas myCanvas4("myCanvas4","",1);
  myH245->Draw();
  myH245->GetXaxis()->SetTitle("diphoton mass");
  myH245->GetYaxis()->SetTitle("CB N left");
  gN1_cat0->Draw("PEsame");
  gN1_cat1->Draw("PEsame");
  gN1_cat2->Draw("PEsame");
  gN1_cat3->Draw("PEsame");
  leg->Draw();
  myCanvas4.SaveAs("n1Trend.png");

  TCanvas myCanvas5("myCanvas5","",1);
  myH245->Draw();
  myH245->GetXaxis()->SetTitle("diphoton mass");
  myH245->GetYaxis()->SetTitle("CB N right");
  gN2_cat0->Draw("PEsame");
  gN2_cat1->Draw("PEsame");
  gN2_cat2->Draw("PEsame");
  gN2_cat3->Draw("PEsame");
  leg->Draw();
  myCanvas5.SaveAs("n2Trend.png");


  // Save parameterizations
  TFile myParaFile("detectorResponseEvolutionAfterConv.root","RECREATE");

  mean_cat0->Write("mean_cat0");
  mean_cat1->Write("mean_cat1");
  mean_cat2->Write("mean_cat2");
  mean_cat3->Write("mean_cat3");

  sigma_cat0->Write("sigma_cat0");
  sigma_cat1->Write("sigma_cat1");
  sigma_cat2->Write("sigma_cat2");
  sigma_cat3->Write("sigma_cat3");

  alpha1_cat0->Write("alpha1_cat0");
  alpha1_cat1->Write("alpha1_cat1");
  alpha1_cat2->Write("alpha1_cat2");
  alpha1_cat3->Write("alpha1_cat3");

  alpha2_cat0->Write("alpha2_cat0");
  alpha2_cat1->Write("alpha2_cat1");
  alpha2_cat2->Write("alpha2_cat2");
  alpha2_cat3->Write("alpha2_cat3");

  n1_cat0->Write("n1_cat0");
  n1_cat1->Write("n1_cat1");
  n1_cat2->Write("n1_cat2");
  n1_cat3->Write("n1_cat3");

  n2_cat0->Write("n2_cat0");
  n2_cat1->Write("n2_cat1");
  n2_cat2->Write("n2_cat2");
  n2_cat3->Write("n2_cat3");
  
}
