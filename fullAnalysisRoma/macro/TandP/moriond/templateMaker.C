#define templateMaker_cxx
#include "templateMaker.h"
#include <iostream>
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>

using namespace std;

#define ptBinsEB 11
#define ptBinsEE 9            // nominal
//#define ptBinsEE 10         // fake rate
#define etaBinsEB 1
#define etaBinsEE 1


void templateMaker::Loop()
{
  if (fChain == 0) return;

  float ptInfEB[ptBinsEB] = {20.,30.,40.,50.,60., 80.,110.,150.,200.,270.,350.};
  float ptSupEB[ptBinsEB] = {30.,40.,50.,60.,80.,110.,150.,200.,270.,350.,500.};
  float ptInfEE[ptBinsEE] = {20.,30.,40.,50.,60.,80., 110.,150.,200.};                 // nominal
  float ptSupEE[ptBinsEE] = {30.,40.,50.,60.,80.,110.,150.,200.,500.};                 // nominal
  //float ptInfEE[ptBinsEB] = {20.,30.,40.,50.,60., 80.,105.,140.,180.,250.};          // fake rate
  //float ptSupEE[ptBinsEB] = {30.,40.,50.,60.,80.,105.,140.,180.,250.,500.};          // fake rate

  float etaInfEB[1] = {0.0};
  float etaSupEB[1] = {1.5};
  float etaInfEE[1] = {1.5};
  float etaSupEE[1] = {2.5};


  // histos for passing and failing probes     
  TH1F *hMassEBpass[ptBinsEB][etaBinsEB], *hMassEEpass[ptBinsEE][etaBinsEE];
  TH1F *hMassEBfail[ptBinsEB][etaBinsEB], *hMassEEfail[ptBinsEE][etaBinsEE];
  TH1F *hMassEBall[ptBinsEB][etaBinsEB],  *hMassEEall[ptBinsEE][etaBinsEE];

  for (int ii=0; ii<ptBinsEB; ii++) {
    for (int jj=0; jj<etaBinsEB; jj++) {
      hMassEBpass[ii][jj] = new TH1F("hMassEBpass[ii][jj]","hMassEBpass[ii][jj]",80,70,110);
      hMassEBpass[ii][jj]->Sumw2();
      hMassEBfail[ii][jj] = new TH1F("hMassEBfail[ii][jj]","hMassEBfail[ii][jj]",80,70,110);
      hMassEBfail[ii][jj]->Sumw2();
      hMassEBall[ii][jj]  = new TH1F("hMassEBall[ii][jj]","hMassEBall[ii][jj]",80,70,110);
      hMassEBall[ii][jj]->Sumw2();
    }
  }
  for (int ii=0; ii<ptBinsEE; ii++) {
    for (int jj=0; jj<etaBinsEE; jj++) {
      hMassEEpass[ii][jj] = new TH1F("hMassEEpass[ii][jj]","hMassEEpass[ii][jj]",80,70,110);
      hMassEEpass[ii][jj]->Sumw2();
      hMassEEfail[ii][jj] = new TH1F("hMassEEfail[ii][jj]","hMassEEfail[ii][jj]",80,70,110);
      hMassEEfail[ii][jj]->Sumw2();
      hMassEEall[ii][jj] = new TH1F("hMassEEall[ii][jj]","hMassEEall[ii][jj]",80,70,110);
      hMassEEall[ii][jj]->Sumw2();
    }
  }


  // Loop over entries    
  Long64_t nentries = fChain->GetEntriesFast();
  Long64_t nbytes = 0, nb = 0;
  cout << "Running over " << nentries << " events" << endl;

  for (Long64_t jentry=0; jentry<nentries;jentry++) {
    Long64_t ientry = LoadTree(jentry);
    if (ientry < 0) break;
    nb = fChain->GetEntry(jentry);   nbytes += nb;
    if(jentry%10000==0) cout << jentry << endl;

    // passed or not 
    bool passed = false;
    if (probe_fullsel) passed = true;             // nominal
    //if (probe_eleveto) passed = true;           // fake rate

    for (int ii=0; ii<ptBinsEB; ii++) {
      for (int jj=0; jj<etaBinsEB; jj++) {
	if (probe_absEta>=etaInfEB[jj] && probe_absEta<=etaSupEB[jj]){ 
	  if (probe_pt>=ptInfEB[ii] && probe_pt<=ptSupEB[ii]) { 
	    hMassEBall[ii][jj]->Fill(mass,pu_weight);
	    if (passed) hMassEBpass[ii][jj]->Fill(mass,pu_weight);            // chiara: DY sample has 1 weight only - use pu not to deal with too small numbers
	    else hMassEBfail[ii][jj]->Fill(mass,pu_weight); 
	  }
	}
      }
    }

    for (int ii=0; ii<ptBinsEE; ii++) {
      for (int jj=0; jj<etaBinsEE; jj++) {
	if (probe_absEta>=etaInfEE[jj] && probe_absEta<=etaSupEE[jj]){ 
	  if (probe_pt>=ptInfEE[ii] && probe_pt<=ptSupEE[ii]) { 
	    hMassEEall[ii][jj]->Fill(mass,pu_weight);
	    if (passed) hMassEEpass[ii][jj]->Fill(mass,pu_weight);
	    else hMassEEfail[ii][jj]->Fill(mass,pu_weight); 
	  }
	}
      }
    }

  }  // Loop over entries


  // saving histos
  TFile myFileTemp("MCtemplates.root","RECREATE");
  
  for (int ii=0; ii<ptBinsEB; ii++) {
    for (int jj=0; jj<etaBinsEB; jj++) {
      TString thisName;
      thisName = TString::Format("hMass_%f",ptInfEB[ii])+TString::Format("To%f",ptSupEB[ii])+TString::Format("_%f",etaInfEB[jj])+TString::Format("To%f",etaSupEB[jj])+"_Pass";
      hMassEBpass[ii][jj] ->Write(thisName);
      thisName = TString::Format("hMass_%f",ptInfEB[ii])+TString::Format("To%f",ptSupEB[ii])+TString::Format("_%f",etaInfEB[jj])+TString::Format("To%f",etaSupEB[jj])+"_Fail";
      hMassEBfail[ii][jj] ->Write(thisName);
      thisName = TString::Format("hMass_%f",ptInfEB[ii])+TString::Format("To%f",ptSupEB[ii])+TString::Format("_%f",etaInfEB[jj])+TString::Format("To%f",etaSupEB[jj])+"_All";
      hMassEBall[ii][jj] ->Write(thisName);
    }
  }
  
  for (int ii=0; ii<ptBinsEE; ii++) {
    for (int jj=0; jj<etaBinsEE; jj++) {
      TString thisName;
      thisName = TString::Format("hMass_%f",ptInfEE[ii])+TString::Format("To%f",ptSupEE[ii])+TString::Format("_%f",etaInfEE[jj])+TString::Format("To%f",etaSupEE[jj])+"_Pass";
      hMassEEpass[ii][jj] ->Write(thisName);
      thisName = TString::Format("hMass_%f",ptInfEE[ii])+TString::Format("To%f",ptSupEE[ii])+TString::Format("_%f",etaInfEE[jj])+TString::Format("To%f",etaSupEE[jj])+"_Fail";
      hMassEEfail[ii][jj] ->Write(thisName);
      thisName = TString::Format("hMass_%f",ptInfEE[ii])+TString::Format("To%f",ptSupEE[ii])+TString::Format("_%f",etaInfEE[jj])+TString::Format("To%f",etaSupEE[jj])+"_All";
      hMassEEall[ii][jj] ->Write(thisName);
    }
  }
}
