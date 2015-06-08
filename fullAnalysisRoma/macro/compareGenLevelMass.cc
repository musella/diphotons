#include <TFile.h>
#include <TTree.h>
#include <TH1.h>
#include <TH2.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TLegend.h>
#include <iostream>

using namespace std;

void compareGenLevelMass() {

  // files
  TFile *myFile500 = new TFile("data/genLevelDistrib/genOnlyPhotons__K02_m500.root");
  TTree *myTree500 = (TTree*)myFile500->Get("genOnlyAna/DiphotonTree");
  //
  TFile *myFile750 = new TFile("data/genLevelDistrib/genOnlyPhotons__K02_m750.root");
  TTree *myTree750 = (TTree*)myFile750->Get("genOnlyAna/DiphotonTree");
  //
  TFile *myFile1000 = new TFile("data/genLevelDistrib/genOnlyPhotons__K02_m1000.root");
  TTree *myTree1000 = (TTree*)myFile1000->Get("genOnlyAna/DiphotonTree");
  //
  TFile *myFile1250 = new TFile("data/genLevelDistrib/genOnlyPhotons__K02_m1250_chiara.root");
  TTree *myTree1250 = (TTree*)myFile1250->Get("genOnlyAna/DiphotonTree");
  //
  TFile *myFile1500 = new TFile("data/genLevelDistrib/genOnlyPhotons__K02_m1500.root");
  TTree *myTree1500 = (TTree*)myFile1500->Get("genOnlyAna/DiphotonTree");
  //
  TFile *myFile1750 = new TFile("data/genLevelDistrib/genOnlyPhotons__K02_m1750_chiara.root");
  TTree *myTree1750 = (TTree*)myFile1750->Get("genOnlyAna/DiphotonTree");
  //
  TFile *myFile2000 = new TFile("data/genLevelDistrib/genOnlyPhotons__K02_m2000_chiara.root");
  TTree *myTree2000 = (TTree*)myFile2000->Get("genOnlyAna/DiphotonTree");
  //
  TFile *myFile2250 = new TFile("data/genLevelDistrib/genOnlyPhotons__K02_m2250_chiara.root");
  TTree *myTree2250 = (TTree*)myFile2250->Get("genOnlyAna/DiphotonTree");
  //
  TFile *myFile2500 = new TFile("data/genLevelDistrib/genOnlyPhotons__K02_m2500_chiara.root");
  TTree *myTree2500 = (TTree*)myFile2500->Get("genOnlyAna/DiphotonTree");
  //
  TFile *myFile2750 = new TFile("data/genLevelDistrib/genOnlyPhotons__K02_m2750_chiara.root");
  TTree *myTree2750 = (TTree*)myFile2750->Get("genOnlyAna/DiphotonTree");
  //
  TFile *myFile3000 = new TFile("data/genLevelDistrib/genOnlyPhotons__K02_m3000.root");
  TTree *myTree3000 = (TTree*)myFile3000->Get("genOnlyAna/DiphotonTree");
  //
  TFile *myFile3000c = new TFile("data/genLevelDistrib/genOnlyPhotons__K02_m3000_chiara.root");
  TTree *myTree3000c = (TTree*)myFile3000c->Get("genOnlyAna/DiphotonTree");
  //
  TFile *myFile4000 = new TFile("data/genLevelDistrib/genOnlyPhotons__K02_m4000.root");
  TTree *myTree4000 = (TTree*)myFile4000->Get("genOnlyAna/DiphotonTree");
  //
  TFile *myFile5000 = new TFile("data/genLevelDistrib/genOnlyPhotons__K02_m5000.root");
  TTree *myTree5000 = (TTree*)myFile5000->Get("genOnlyAna/DiphotonTree");

  // -------------------------------------------------------
  // Plots
  gStyle->SetOptStat(0);

  TH1F *myH500  = new TH1F("myH500","myH500",  55,500.,6000.);
  TH1F *myH750  = new TH1F("myH750","myH750",  55,500.,6000.);
  TH1F *myH1000 = new TH1F("myH1000","myH1000",55,500.,6000.);
  TH1F *myH1250 = new TH1F("myH1250","myH1250",55,500.,6000.);
  TH1F *myH1500 = new TH1F("myH1500","myH1500",55,500.,6000.);
  TH1F *myH1750 = new TH1F("myH1750","myH1750",55,500.,6000.);
  TH1F *myH2000 = new TH1F("myH2000","myH2000",55,500.,6000.);
  TH1F *myH2250 = new TH1F("myH2250","myH2250",55,500.,6000.);
  TH1F *myH2500 = new TH1F("myH2500","myH2500",55,500.,6000.);
  TH1F *myH2750 = new TH1F("myH2750","myH2750",55,500.,6000.);
  TH1F *myH3000 = new TH1F("myH3000","myH3000",55,500.,6000.);
  TH1F *myH3000c = new TH1F("myH3000c","myH3000c",55,500.,6000.);
  TH1F *myH4000 = new TH1F("myH4000","myH4000",55,500.,6000.);
  TH1F *myH5000 = new TH1F("myH5000","myH5000",55,500.,6000.);
  //
  myTree500 ->Project("myH500","mggGen","mggGen>=0");
  myTree750 ->Project("myH750","mggGen","mggGen>=0");
  myTree1000->Project("myH1000","mggGen","mggGen>=0");
  myTree1250->Project("myH1250","mggGen","mggGen>=0");
  myTree1500->Project("myH1500","mggGen","mggGen>=0");
  myTree1750->Project("myH1750","mggGen","mggGen>=0");
  myTree2000->Project("myH2000","mggGen","mggGen>=0");
  myTree2250->Project("myH2250","mggGen","mggGen>=0");
  myTree2500->Project("myH2500","mggGen","mggGen>=0");
  myTree2750->Project("myH2750","mggGen","mggGen>=0");
  myTree3000->Project("myH3000","mggGen","mggGen>=0");
  myTree3000c->Project("myH3000c","mggGen","mggGen>=0");
  myTree4000->Project("myH4000","mggGen","mggGen>=0");
  myTree5000->Project("myH5000","mggGen","mggGen>=0");
  //
  myH500->SetLineColor(3);
  myH750->SetLineColor(3);
  myH1000->SetLineColor(3);
  myH1250->SetLineColor(2);
  myH1500->SetLineColor(3);
  myH1750->SetLineColor(2);
  myH2000->SetLineColor(2);
  myH2250->SetLineColor(2);
  myH2500->SetLineColor(2);
  myH2750->SetLineColor(2);
  myH3000->SetLineColor(3);
  myH3000c->SetLineColor(2);
  myH4000->SetLineColor(3);
  myH5000->SetLineColor(3);
  //
  myH500->SetLineWidth(1);
  myH750->SetLineWidth(1);
  myH1000->SetLineWidth(1);
  myH1250->SetLineWidth(1);
  myH1500->SetLineWidth(1);
  myH1750->SetLineWidth(1);
  myH2000->SetLineWidth(1);
  myH2250->SetLineWidth(1);
  myH2500->SetLineWidth(1);
  myH2750->SetLineWidth(1);
  myH3000->SetLineWidth(1);
  myH3000c->SetLineWidth(1);
  myH4000->SetLineWidth(1);
  myH5000->SetLineWidth(1);

  TH2F *myH = new TH2F("myH","",100,500,6000,100,0,1);
  myH->GetXaxis()->SetTitle("gen level mgg");

  TCanvas c1("c1","c1",1);
  c1.cd();
  myH->Draw();
  myH500 ->DrawNormalized("same");
  myH750 ->DrawNormalized("same");
  myH1000->DrawNormalized("same");
  myH1250->DrawNormalized("same");
  myH1500->DrawNormalized("same");
  myH1750->DrawNormalized("same");
  myH2000->DrawNormalized("same");
  myH2250->DrawNormalized("same");
  myH2500->DrawNormalized("same");
  myH2750->DrawNormalized("same");
  myH3000->DrawNormalized("same");
  myH3000c->DrawNormalized("same");
  myH4000->DrawNormalized("same");
  myH5000->DrawNormalized("same");
  c1.SaveAs("test.png");
}
