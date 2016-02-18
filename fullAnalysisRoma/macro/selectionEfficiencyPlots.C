#include "TStyle.h"
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TTree.h"
#include <TGraphAsymmErrors.h>
#include "TCanvas.h"
#include "TLegend.h"

#include <iostream>

bool selectionEfficiencyPlots() {

  // taking inputs
  TFile *infile = new TFile("data/selezV1/perEfficienze/mergedFinal/GGJets.root");   
  if (!infile) {
    cout << "File " << infile << " not existing" << endl;
    return 0;
  }

  TTree *theTree = (TTree*)infile->Get("DiPhotonTree");
  if (!theTree) { 
    cout << "Tree DiPhotonTree not existing" << endl;
    return 0;
  }

  // selection breakdown
  TH1F *h_selection = (TH1F*)infile->Get("h_selection");

  // num & den histos
  TH1D *h_denomPlusPres0 = new TH1D("h_denomPlusPres0", "h_denomPlusPres0", 120, 0., 6000.);
  TH1D *h_denomPlusPres1 = new TH1D("h_denomPlusPres1", "h_denomPlusPres1", 120, 0., 6000.);
  TH1D *h_denomPlusPres2 = new TH1D("h_denomPlusPres2", "h_denomPlusPres2", 120, 0., 6000.);
  TH1D *h_denomPlusPres3 = new TH1D("h_denomPlusPres3", "h_denomPlusPres3", 120, 0., 6000.);

  TH1D *h_denomPlusPres01 = new TH1D("h_denomPlusPres01", "h_denomPlusPres01", 120, 0., 6000.);
  TH1D *h_denomPlusPres23 = new TH1D("h_denomPlusPres23", "h_denomPlusPres23", 120, 0., 6000.);

  TH1D *h_num0 = new TH1D("h_num0", "h_num0", 120, 0., 6000.);
  TH1D *h_num1 = new TH1D("h_num1", "h_num1", 120, 0., 6000.);
  TH1D *h_num2 = new TH1D("h_num2", "h_num2", 120, 0., 6000.);
  TH1D *h_num3 = new TH1D("h_num3", "h_num3", 120, 0., 6000.);

  TH1D *h_num01 = new TH1D("h_num01", "h_num01", 120, 0., 6000.);
  TH1D *h_num23 = new TH1D("h_num23", "h_num23", 120, 0., 6000.);

  h_denomPlusPres0->Sumw2();
  h_denomPlusPres1->Sumw2();
  h_denomPlusPres2->Sumw2();
  h_denomPlusPres3->Sumw2();

  h_denomPlusPres01->Sumw2();
  h_denomPlusPres23->Sumw2();

  h_num0->Sumw2();
  h_num1->Sumw2();
  h_num2->Sumw2();
  h_num3->Sumw2();

  h_num01->Sumw2();
  h_num23->Sumw2();


  // Filling histos
  TString cutBase = "(mgg>250 && pt1>100 && pt2>100 && genmatch1==1 && genmatch2==1 && geniso1<10 && geniso2<10 ";
  TString cutPresel = cutBase + " && presel1==1 && presel2==1";
  TString cutSel    = cutPresel + " && sel1==1 && sel2==1";
  cout << endl;
  cout << cutPresel << endl;
  cout << cutSel << endl;
  cout << endl;
  // 
  theTree->Project("h_denomPlusPres0","mgg",cutPresel + TString(" && eventClass==0)*weight"));
  theTree->Project("h_denomPlusPres1","mgg",cutPresel + TString(" && eventClass==1)*weight"));
  theTree->Project("h_denomPlusPres2","mgg",cutPresel + TString(" && eventClass==2)*weight"));
  theTree->Project("h_denomPlusPres3","mgg",cutPresel + TString(" && eventClass==3)*weight"));
  //
  theTree->Project("h_denomPlusPres01","mgg",cutPresel + TString(" && (eventClass==0 || eventClass==1))*weight"));
  theTree->Project("h_denomPlusPres23","mgg",cutPresel + TString(" && (eventClass==2 || eventClass==3))*weight"));
  //
  theTree->Project("h_num0","mgg",cutSel + TString(" && eventClass==0)*weight"));
  theTree->Project("h_num1","mgg",cutSel + TString(" && eventClass==1)*weight"));
  theTree->Project("h_num2","mgg",cutSel + TString(" && eventClass==2)*weight"));
  theTree->Project("h_num3","mgg",cutSel + TString(" && eventClass==3)*weight"));
  //
  theTree->Project("h_num01","mgg",cutSel + TString(" && (eventClass==0 || eventClass==1))*weight"));
  theTree->Project("h_num23","mgg",cutSel + TString(" && (eventClass==2 || eventClass==3))*weight"));

  // rebinning
  h_denomPlusPres0->Rebin(4);
  h_denomPlusPres1->Rebin(4);
  h_denomPlusPres2->Rebin(4);
  h_denomPlusPres3->Rebin(4);
  h_denomPlusPres01->Rebin(4);
  h_denomPlusPres23->Rebin(4);
  h_num0->Rebin(4);
  h_num1->Rebin(4);
  h_num2->Rebin(4);
  h_num3->Rebin(4);
  h_num01->Rebin(4);
  h_num23->Rebin(4);

  // efficiency distributions
  TGraphAsymmErrors *h_effVsPres0 = new TGraphAsymmErrors(h_num0,h_denomPlusPres0);
  TGraphAsymmErrors *h_effVsPres1 = new TGraphAsymmErrors(h_num1,h_denomPlusPres1);
  TGraphAsymmErrors *h_effVsPres2 = new TGraphAsymmErrors(h_num2,h_denomPlusPres2);
  TGraphAsymmErrors *h_effVsPres3 = new TGraphAsymmErrors(h_num3,h_denomPlusPres3);

  TGraphAsymmErrors *h_effVsPres01 = new TGraphAsymmErrors(h_num01,h_denomPlusPres01);
  TGraphAsymmErrors *h_effVsPres23 = new TGraphAsymmErrors(h_num23,h_denomPlusPres23);

  h_effVsPres0->SetTitle("EB, highR9");
  h_effVsPres1->SetTitle("EB, lowR9");
  h_effVsPres2->SetTitle("EE, highR9");
  h_effVsPres3->SetTitle("EE, lowR9");
  h_effVsPres01->SetTitle("EBEB");
  h_effVsPres23->SetTitle("EBEE");

  h_effVsPres0->GetXaxis()->SetTitle("m(#gamma#gamma)");
  h_effVsPres1->GetXaxis()->SetTitle("m(#gamma#gamma)");
  h_effVsPres2->GetXaxis()->SetTitle("m(#gamma#gamma)");
  h_effVsPres3->GetXaxis()->SetTitle("m(#gamma#gamma)");
  h_effVsPres01->GetXaxis()->SetTitle("m(#gamma#gamma)");
  h_effVsPres23->GetXaxis()->SetTitle("m(#gamma#gamma)");

  // cosmetics
  h_effVsPres0->SetMarkerStyle(20);
  h_effVsPres1->SetMarkerStyle(20);
  h_effVsPres2->SetMarkerStyle(20);
  h_effVsPres3->SetMarkerStyle(20);
  h_effVsPres01->SetMarkerStyle(20);
  h_effVsPres23->SetMarkerStyle(20);

  h_effVsPres0->SetMarkerColor(2);
  h_effVsPres1->SetMarkerColor(2);
  h_effVsPres2->SetMarkerColor(2);
  h_effVsPres3->SetMarkerColor(2);
  h_effVsPres01->SetMarkerColor(2);
  h_effVsPres23->SetMarkerColor(2);


  // breakdown
  h_selection->SetLineColor(4);
  h_selection->SetLineWidth(2);
  h_selection->GetXaxis()->SetBinLabel(1,"no cut");
  h_selection->GetXaxis()->SetBinLabel(2,"2 preselected #gamma");
  h_selection->GetXaxis()->SetBinLabel(3,"2 selected #gamma");
  h_selection->GetXaxis()->SetBinLabel(4,"pT cuts");
  h_selection->GetXaxis()->SetBinLabel(5,"m(#gamma#gamma) cut");
  h_selection->GetXaxis()->SetBinLabel(6,"good vertex");


  // plots
  gStyle->SetOptStat(0);

  TH2F *myH2  = new TH2F("myH2", "",100,0.,6000.,100,0.5,1.);
  myH2->GetXaxis()->SetTitle("m(#gamma#gamma)");
  myH2->GetYaxis()->SetTitle("efficiency");

  TCanvas ca("ca","",1);
  myH2->SetTitle("EB, high R9");
  myH2->Draw();
  h_effVsPres0->Draw("sameP");
  ca.SaveAs("effVsMass_class0.png");

  TCanvas cb("cb","",1);
  myH2->SetTitle("EB, low R9");
  myH2->Draw();
  h_effVsPres1->Draw("sameP");
  cb.SaveAs("effVsMass_class1.png");

  TCanvas cc("cc","",1);
  myH2->SetTitle("EE, high R9");
  myH2->Draw();
  h_effVsPres2->Draw("sameP");
  cc.SaveAs("effVsMass_class2.png");

  TCanvas cd("cd","",1);
  myH2->SetTitle("EE, low R9");
  myH2->Draw();
  h_effVsPres3->Draw("sameP");
  cd.SaveAs("effVsMass_class3.png");

  TCanvas ce("ce","",1);
  myH2->SetTitle("EBEB");
  myH2->Draw();
  h_effVsPres01->Draw("sameP");
  ce.SaveAs("effVsMass_EBEB.png");

  TCanvas cf("cf","",1);
  myH2->SetTitle("EBEB");
  myH2->Draw();
  h_effVsPres23->Draw("sameP");
  cf.SaveAs("effVsMass_EBEE.png");
  
  TCanvas cg("cg","",1);
  h_selection->Draw("hist");
  h_selection->SetTitle("");
  cg.SaveAs("breakdown.png");
  cg.SetLogy();
  cg.SaveAs("breakdown_log.png");

  return 1;
}
