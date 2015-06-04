#include <TFile.h>
#include <TTree.h>
#include <TH1.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TLegend.h>
#include <iostream>

using namespace std;

void compareMassDistribution() {

  // to be changed
  int massA = 750;
  int massB = 1500;
  int massC = 5000;
  string coupling = "001";

  // input files
  TFile *fA = new TFile(TString("data/newSelection/mergedFinal/RSGravToGG_kMpl-")+TString(coupling)+Form("_M-%d.root",massA));
  cout << "fileA = " << TString("data/newSelection/mergedFinal/RSGravToGG_kMpl-")+TString(coupling)+Form("_M-%d.root",massA) << endl;
  TFile *fB = new TFile(TString("data/newSelection/mergedFinal/RSGravToGG_kMpl-")+TString(coupling)+Form("_M-%d.root",massB));
  cout << "fileB = " << TString("data/newSelection/mergedFinal/RSGravToGG_kMpl-")+TString(coupling)+Form("_M-%d.root",massB) << endl;
  TFile *fC = new TFile(TString("data/newSelection/mergedFinal/RSGravToGG_kMpl-")+TString(coupling)+Form("_M-%d.root",massC));
  cout << "fileC = " << TString("data/newSelection/mergedFinal/RSGravToGG_kMpl-")+TString(coupling)+Form("_M-%d.root",massC) << endl;
  cout << "files taken" << endl;

  TTree* tA = (TTree*)fA->Get("DiPhotonTree");
  TTree* tB = (TTree*)fB->Get("DiPhotonTree");
  TTree* tC = (TTree*)fC->Get("DiPhotonTree");
  cout << "tree taken" << endl;

  // projecting - cat0
  TH1D *H_A_cat0 = new TH1D("H_A_cat0","H_A_cat0", 60,-0.12,0.12);
  TH1D *H_B_cat0 = new TH1D("H_B_cat0","H_B_cat0", 60,-0.12,0.12);
  TH1D *H_C_cat0 = new TH1D("H_C_cat0","H_C_cat0", 60,-0.12,0.12);
  H_A_cat0->Sumw2();
  H_B_cat0->Sumw2();
  H_C_cat0->Sumw2();
  tA -> Project("H_A_cat0", "mgg/mggGen-1","mgg>500 && mgg<6000 && mggGen>=0 && eventClass==0");
  tB -> Project("H_B_cat0", "mgg/mggGen-1","mgg>500 && mgg<6000 && mggGen>=0 && eventClass==0");
  tC -> Project("H_C_cat0", "mgg/mggGen-1","mgg>500 && mgg<6000 && mggGen>=0 && eventClass==0");

  // projecting - cat1
  TH1D *H_A_cat1 = new TH1D("H_A_cat1","H_A_cat1", 60,-0.12,0.12);
  TH1D *H_B_cat1 = new TH1D("H_B_cat1","H_B_cat1", 60,-0.12,0.12);
  TH1D *H_C_cat1 = new TH1D("H_C_cat1","H_C_cat1", 60,-0.12,0.12);
  H_A_cat1->Sumw2();
  H_B_cat1->Sumw2();
  H_C_cat1->Sumw2();
  tA -> Project("H_A_cat1", "mgg/mggGen-1","mgg>500 && mgg<6000 && mggGen>=0 && eventClass==1");
  tB -> Project("H_B_cat1", "mgg/mggGen-1","mgg>500 && mgg<6000 && mggGen>=0 && eventClass==1");
  tC -> Project("H_C_cat1", "mgg/mggGen-1","mgg>500 && mgg<6000 && mggGen>=0 && eventClass==1");

  // projecting - cat2
  TH1D *H_A_cat2 = new TH1D("H_A_cat2","H_A_cat2", 60,-0.12,0.12);
  TH1D *H_B_cat2 = new TH1D("H_B_cat2","H_B_cat2", 60,-0.12,0.12);
  TH1D *H_C_cat2 = new TH1D("H_C_cat2","H_C_cat2", 60,-0.12,0.12);
  H_A_cat2->Sumw2();
  H_B_cat2->Sumw2();
  H_C_cat2->Sumw2();
  tA -> Project("H_A_cat2", "mgg/mggGen-1","mgg>500 && mgg<6000 && mggGen>=0 && eventClass==2");
  tB -> Project("H_B_cat2", "mgg/mggGen-1","mgg>500 && mgg<6000 && mggGen>=0 && eventClass==2");
  tC -> Project("H_C_cat2", "mgg/mggGen-1","mgg>500 && mgg<6000 && mggGen>=0 && eventClass==2");

  // projecting - cat3
  TH1D *H_A_cat3 = new TH1D("H_A_cat3","H_A_cat3", 60,-0.12,0.12);
  TH1D *H_B_cat3 = new TH1D("H_B_cat3","H_B_cat3", 60,-0.12,0.12);
  TH1D *H_C_cat3 = new TH1D("H_C_cat3","H_C_cat3", 60,-0.12,0.12);
  H_A_cat3->Sumw2();
  H_B_cat3->Sumw2();
  H_C_cat3->Sumw2();
  tA -> Project("H_A_cat3", "mgg/mggGen-1","mgg>500 && mgg<6000 && mggGen>=0 && eventClass==3");
  tB -> Project("H_B_cat3", "mgg/mggGen-1","mgg>500 && mgg<6000 && mggGen>=0 && eventClass==3");
  tC -> Project("H_C_cat3", "mgg/mggGen-1","mgg>500 && mgg<6000 && mggGen>=0 && eventClass==3");

  // cosmetics
  H_A_cat0->SetLineColor(2);
  H_B_cat0->SetLineColor(3);
  H_C_cat0->SetLineColor(4);
  H_A_cat0->SetLineWidth(2);
  H_B_cat0->SetLineWidth(2);
  H_C_cat0->SetLineWidth(2);
  H_A_cat0->SetTitle("cat0");
  H_B_cat0->SetTitle("cat0");
  H_C_cat0->SetTitle("cat0");
  H_A_cat0->GetXaxis()->SetTitle("m_{RECO}/m_{TRUE}-1");
  H_B_cat0->GetXaxis()->SetTitle("m_{RECO}/m_{TRUE}-1");
  H_C_cat0->GetXaxis()->SetTitle("m_{RECO}/m_{TRUE}-1");
  //
  H_A_cat1->SetLineColor(2);
  H_B_cat1->SetLineColor(3);
  H_C_cat1->SetLineColor(4);
  H_A_cat1->SetLineWidth(2);
  H_B_cat1->SetLineWidth(2);
  H_C_cat1->SetLineWidth(2);
  H_A_cat1->SetTitle("cat1");
  H_B_cat1->SetTitle("cat1");
  H_C_cat1->SetTitle("cat1");
  H_A_cat1->GetXaxis()->SetTitle("m_{RECO}/m_{TRUE}-1");
  H_B_cat1->GetXaxis()->SetTitle("m_{RECO}/m_{TRUE}-1");
  H_C_cat1->GetXaxis()->SetTitle("m_{RECO}/m_{TRUE}-1");
  //
  H_A_cat2->SetLineColor(2);
  H_B_cat2->SetLineColor(3);
  H_C_cat2->SetLineColor(4);
  H_A_cat2->SetLineWidth(2);
  H_B_cat2->SetLineWidth(2);
  H_C_cat2->SetLineWidth(2);
  H_A_cat2->SetTitle("cat2");
  H_B_cat2->SetTitle("cat2");
  H_C_cat2->SetTitle("cat2");
  H_A_cat2->GetXaxis()->SetTitle("m_{RECO}/m_{TRUE}-1");
  H_B_cat2->GetXaxis()->SetTitle("m_{RECO}/m_{TRUE}-1");
  H_C_cat2->GetXaxis()->SetTitle("m_{RECO}/m_{TRUE}-1");
  //
  H_A_cat3->SetLineColor(2);
  H_B_cat3->SetLineColor(3);
  H_C_cat3->SetLineColor(4);
  H_A_cat3 ->SetLineWidth(2);
  H_B_cat3->SetLineWidth(2);
  H_C_cat3->SetLineWidth(2);
  H_A_cat3->SetTitle("cat3");
  H_B_cat3->SetTitle("cat3");
  H_C_cat3->SetTitle("cat3");
  H_A_cat3->GetXaxis()->SetTitle("m_{RECO}/m_{TRUE}-1");
  H_B_cat3->GetXaxis()->SetTitle("m_{RECO}/m_{TRUE}-1");
  H_C_cat3->GetXaxis()->SetTitle("m_{RECO}/m_{TRUE}-1");

  // plots
  gStyle->SetOptStat(0);

  TLegend *leg;
  leg = new TLegend(0.10,0.65,0.35,0.90);
  leg->SetFillStyle(0);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.05);
  leg->SetFillColor(0);
  leg->AddEntry(H_A_cat0, Form("m_{G}=%d GeV",massA), "l");
  leg->AddEntry(H_B_cat0, Form("m_{G}=%d GeV",massB), "l");
  leg->AddEntry(H_C_cat0, Form("m_{G}=%d GeV",massC), "l");
  
  TCanvas myCanvas0("myCanvas0","",1);
  H_A_cat0->DrawNormalized("hist");
  H_B_cat0->DrawNormalized("samehist");
  H_C_cat0->DrawNormalized("samehist");
  leg->Draw();
  myCanvas0.SaveAs(TString("massComparison_cat0_")+TString(coupling)+TString(".png"));
  myCanvas0.SetLogy();
  myCanvas0.SaveAs(TString("massComparison_cat0_")+TString(coupling)+TString("_LOG.png"));

  TCanvas myCanvas1("myCanvas1","",1);
  H_A_cat1->DrawNormalized("hist");
  H_B_cat1->DrawNormalized("samehist");
  H_C_cat1->DrawNormalized("samehist");
  leg->Draw();
  myCanvas1.SaveAs(TString("massComparison_cat1_")+TString(coupling)+TString(".png"));
  myCanvas1.SetLogy();
  myCanvas1.SaveAs(TString("massComparison_cat1_")+TString(coupling)+TString("_LOG.png"));

  TCanvas myCanvas2("myCanvas2","",1);
  H_A_cat2->DrawNormalized("hist");
  H_B_cat2->DrawNormalized("samehist");
  H_C_cat2->DrawNormalized("samehist");
  leg->Draw();
  myCanvas2.SaveAs(TString("massComparison_cat2_")+TString(coupling)+TString(".png"));
  myCanvas2.SetLogy();
  myCanvas2.SaveAs(TString("massComparison_cat2_")+TString(coupling)+TString("_LOG.png"));

  TCanvas myCanvas3("myCanvas3","",1);
  H_A_cat3->DrawNormalized("hist");
  H_B_cat3->DrawNormalized("samehist");
  H_C_cat3->DrawNormalized("samehist");
  leg->Draw();
  myCanvas3.SaveAs(TString("massComparison_cat3_")+TString(coupling)+TString(".png"));
  myCanvas3.SetLogy();
  myCanvas3.SaveAs(TString("massComparison_cat3_")+TString(coupling)+TString("_LOG.png"));
}
