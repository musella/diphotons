#include <TFile.h>
#include <TTree.h>
#include <TH1.h>
#include <TF1.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TLegend.h>
#include <iostream>

using namespace std;

void compareTrueMassDistribution() {

  // to be changed
  int massA = 1500;
  int massB = 3000;
  int massC = 5000;
  int rangePlot = 500;        // 100
  int bins      = 100;        // 70  
  string coupling = "02";     // 001

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
  TH1D *H_A_cat0 = new TH1D("H_A_cat0","H_A_cat0", bins,massA-rangePlot,massA+rangePlot);
  TH1D *H_B_cat0 = new TH1D("H_B_cat0","H_B_cat0", bins,massB-rangePlot,massB+rangePlot);
  TH1D *H_C_cat0 = new TH1D("H_C_cat0","H_C_cat0", bins,massC-rangePlot,massC+rangePlot);
  H_A_cat0->Sumw2();
  H_B_cat0->Sumw2();
  H_C_cat0->Sumw2();
  tA -> Project("H_A_cat0", "mggGen","mgg>500 && mgg<6000 && mggGen>=0 && eventClass==0");
  tB -> Project("H_B_cat0", "mggGen","mgg>500 && mgg<6000 && mggGen>=0 && eventClass==0");
  tC -> Project("H_C_cat0", "mggGen","mgg>500 && mgg<6000 && mggGen>=0 && eventClass==0");

  // projecting - cat1
  TH1D *H_A_cat1 = new TH1D("H_A_cat1","H_A_cat1", bins,massA-rangePlot,massA+rangePlot);
  TH1D *H_B_cat1 = new TH1D("H_B_cat1","H_B_cat1", bins,massB-rangePlot,massB+rangePlot);
  TH1D *H_C_cat1 = new TH1D("H_C_cat1","H_C_cat1", bins,massC-rangePlot,massC+rangePlot);
  H_A_cat1->Sumw2();
  H_B_cat1->Sumw2();
  H_C_cat1->Sumw2();
  tA -> Project("H_A_cat1", "mggGen","mgg>500 && mgg<6000 && mggGen>=0 && eventClass==1");
  tB -> Project("H_B_cat1", "mggGen","mgg>500 && mgg<6000 && mggGen>=0 && eventClass==1");
  tC -> Project("H_C_cat1", "mggGen","mgg>500 && mgg<6000 && mggGen>=0 && eventClass==1");

  // projecting - cat2
  TH1D *H_A_cat2 = new TH1D("H_A_cat2","H_A_cat2", bins,massA-rangePlot,massA+rangePlot);
  TH1D *H_B_cat2 = new TH1D("H_B_cat2","H_B_cat2", bins,massB-rangePlot,massB+rangePlot);
  TH1D *H_C_cat2 = new TH1D("H_C_cat2","H_C_cat2", bins,massC-rangePlot,massC+rangePlot);
  H_A_cat2->Sumw2();
  H_B_cat2->Sumw2();
  H_C_cat2->Sumw2();
  tA -> Project("H_A_cat2", "mggGen","mgg>500 && mgg<6000 && mggGen>=0 && eventClass==2");
  tB -> Project("H_B_cat2", "mggGen","mgg>500 && mgg<6000 && mggGen>=0 && eventClass==2");
  tC -> Project("H_C_cat2", "mggGen","mgg>500 && mgg<6000 && mggGen>=0 && eventClass==2");

  // projecting - cat3
  TH1D *H_A_cat3 = new TH1D("H_A_cat3","H_A_cat3", bins,massA-rangePlot,massA+rangePlot);
  TH1D *H_B_cat3 = new TH1D("H_B_cat3","H_B_cat3", bins,massB-rangePlot,massB+rangePlot);
  TH1D *H_C_cat3 = new TH1D("H_C_cat3","H_C_cat3", bins,massC-rangePlot,massC+rangePlot);
  H_A_cat3->Sumw2();
  H_B_cat3->Sumw2();
  H_C_cat3->Sumw2();
  tA -> Project("H_A_cat3", "mggGen","mgg>500 && mgg<6000 && mggGen>=0 && eventClass==3");
  tB -> Project("H_B_cat3", "mggGen","mgg>500 && mgg<6000 && mggGen>=0 && eventClass==3");
  tC -> Project("H_C_cat3", "mggGen","mgg>500 && mgg<6000 && mggGen>=0 && eventClass==3");

  // projecting - all classes
  TH1D *H_A = new TH1D("H_A","H_A", bins,massA-rangePlot,massA+rangePlot);
  TH1D *H_B = new TH1D("H_B","H_B", bins,massB-rangePlot,massB+rangePlot);
  TH1D *H_C = new TH1D("H_C","H_C", bins,massC-rangePlot,massC+rangePlot);
  H_A->Sumw2();
  H_B->Sumw2();
  H_C->Sumw2();
  tA -> Project("H_A", "mggGen","mgg>500 && mgg<6000 && mggGen>=0");
  tB -> Project("H_B", "mggGen","mgg>500 && mgg<6000 && mggGen>=0");
  tC -> Project("H_C", "mggGen","mgg>500 && mgg<6000 && mggGen>=0");

  // cosmetics
  H_A_cat0->SetLineColor(2);
  H_B_cat0->SetLineColor(2);
  H_C_cat0->SetLineColor(2);
  H_A_cat0->SetLineWidth(2);
  H_B_cat0->SetLineWidth(2);
  H_C_cat0->SetLineWidth(2);
  H_A_cat0->SetTitle("");
  H_B_cat0->SetTitle("");
  H_C_cat0->SetTitle("");
  H_A_cat0->GetXaxis()->SetTitle("m_{GEN}");
  H_B_cat0->GetXaxis()->SetTitle("m_{GEN}");
  H_C_cat0->GetXaxis()->SetTitle("m_{GEN}");
  //
  H_A_cat1->SetLineColor(3);
  H_B_cat1->SetLineColor(3);
  H_C_cat1->SetLineColor(3);
  H_A_cat1->SetLineWidth(2);
  H_B_cat1->SetLineWidth(2);
  H_C_cat1->SetLineWidth(2);
  H_A_cat1->SetTitle("");
  H_B_cat1->SetTitle("");
  H_C_cat1->SetTitle("");
  H_A_cat1->GetXaxis()->SetTitle("m_{GEN}");
  H_B_cat1->GetXaxis()->SetTitle("m_{GEN}");
  H_C_cat1->GetXaxis()->SetTitle("m_{GEN}");
  //
  H_A_cat2->SetLineColor(4);
  H_B_cat2->SetLineColor(4);
  H_C_cat2->SetLineColor(4);
  H_A_cat2->SetLineWidth(2);
  H_B_cat2->SetLineWidth(2);
  H_C_cat2->SetLineWidth(2);
  H_A_cat2->SetTitle("");
  H_B_cat2->SetTitle("");
  H_C_cat2->SetTitle("");
  H_A_cat2->GetXaxis()->SetTitle("m_{GEN}");
  H_B_cat2->GetXaxis()->SetTitle("m_{GEN}");
  H_C_cat2->GetXaxis()->SetTitle("m_{GEN}");
  //
  H_A_cat3->SetLineColor(7);
  H_B_cat3->SetLineColor(7);
  H_C_cat3->SetLineColor(7);
  H_A_cat3 ->SetLineWidth(2);
  H_B_cat3->SetLineWidth(2);
  H_C_cat3->SetLineWidth(2);
  H_A_cat3->SetTitle("");
  H_B_cat3->SetTitle("");
  H_C_cat3->SetTitle("");
  H_A_cat3->GetXaxis()->SetTitle("m_{GEN}");
  H_B_cat3->GetXaxis()->SetTitle("m_{GEN}");
  H_C_cat3->GetXaxis()->SetTitle("m_{GEN}");
  //
  H_A->SetLineColor(1);
  H_B->SetLineColor(1);
  H_C->SetLineColor(1);
  H_A ->SetLineWidth(2);
  H_B->SetLineWidth(2);
  H_C->SetLineWidth(2);
  H_A->SetTitle("");
  H_B->SetTitle("");
  H_C->SetTitle("");
  H_A->GetXaxis()->SetTitle("m_{GEN}");
  H_B->GetXaxis()->SetTitle("m_{GEN}");
  H_C->GetXaxis()->SetTitle("m_{GEN}");
  
  // plots
  gStyle->SetOptStat(0);

  TLegend *leg;
  leg = new TLegend(0.10,0.65,0.35,0.90);
  leg->SetFillStyle(0);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.05);
  leg->SetFillColor(0);
  leg->AddEntry(H_A_cat0, "cat0", "l");
  leg->AddEntry(H_A_cat1, "cat1", "l");
  leg->AddEntry(H_A_cat2, "cat2", "l");
  leg->AddEntry(H_A_cat3, "cat3", "l");
  
  TCanvas myCanvas0("myCanvas0","",1);
  H_A_cat0->DrawNormalized("hist");
  H_A_cat1->DrawNormalized("samehist");
  H_A_cat2->DrawNormalized("samehist");
  H_A_cat3->DrawNormalized("samehist");
  leg->Draw();
  myCanvas0.SetLogy();
  myCanvas0.SaveAs(Form("genMass_mass%d_",massA)+TString(coupling)+TString("_LOG.png"));

  TCanvas myCanvas1("myCanvas1","",1);
  H_B_cat0->DrawNormalized("hist");
  H_B_cat1->DrawNormalized("samehist");
  H_B_cat2->DrawNormalized("samehist");
  H_B_cat3->DrawNormalized("samehist");
  leg->Draw();
  myCanvas1.SetLogy();
  myCanvas1.SaveAs(Form("genMass_mass%d_",massB)+TString(coupling)+TString("_LOG.png"));

  TCanvas myCanvas2("myCanvas2","",1);
  H_C_cat0->DrawNormalized("hist");
  H_C_cat1->DrawNormalized("samehist");
  H_C_cat2->DrawNormalized("samehist");
  H_C_cat3->DrawNormalized("samehist");
  leg->Draw();
  myCanvas2.SetLogy();
  myCanvas2.SaveAs(Form("genMass_mass%d_",massC)+TString(coupling)+TString("_LOG.png"));

  TCanvas myCanvas3a("myCanvas3a","",1);
  H_A->DrawNormalized("hist");
  myCanvas3a.SaveAs(Form("genMass_mass%d_",massA)+TString(coupling)+TString("_all.png"));
  myCanvas3a.SetLogy();
  myCanvas3a.SaveAs(Form("genMass_mass%d_",massA)+TString(coupling)+TString("_all_LOG.png"));

  TCanvas myCanvas3b("myCanvas3b","",1);
  H_B->DrawNormalized("hist");
  myCanvas3b.SaveAs(Form("genMass_mass%d_",massB)+TString(coupling)+TString("_all.png"));
  myCanvas3b.SetLogy();
  myCanvas3b.SaveAs(Form("genMass_mass%d_",massB)+TString(coupling)+TString("_all_LOG.png"));

  TCanvas myCanvas3c("myCanvas3c","",1);
  H_C->DrawNormalized("hist");
  myCanvas3c.SaveAs(Form("genMass_mass%d_",massC)+TString(coupling)+TString("_all.png"));
  myCanvas3c.SetLogy();
  myCanvas3c.SaveAs(Form("genMass_mass%d_",massC)+TString(coupling)+TString("_all_LOG.png"));
}
