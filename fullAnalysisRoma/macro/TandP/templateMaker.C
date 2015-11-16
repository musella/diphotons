#define templateMaker_cxx
#include "templateMaker.h"
#include <iostream>
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>

using namespace std;

#define ptBinsEB 9
#define ptBinsEE 7
#define etaBinsEB 1
#define etaBinsEE 1


void templateMaker::Loop()
{
  if (fChain == 0) return;

  float ptInfEB[ptBinsEB] = {20.,30.,40.,50.,60.,80.,110.,150.,250.};
  float ptSupEB[ptBinsEB] = {30.,40.,50.,60.,80.,110.,150.,250.,500.};
  float ptInfEE[ptBinsEE] = {20.,30.,40.,50.,60.,80.,110.};
  float ptSupEE[ptBinsEE] = {30.,40.,50.,60.,80.,110.,500.};

  float etaInfEB[1] = {0.0};
  float etaSupEB[1] = {1.5};
  float etaInfEE[1] = {1.5};
  float etaSupEE[1] = {2.5};


  // histos for passing and failing probes     
  TH1F *hMassEBpass[ptBinsEB][etaBinsEB], *hMassEEpass[ptBinsEE][etaBinsEE];
  TH1F *hMassEBfail[ptBinsEB][etaBinsEB], *hMassEEfail[ptBinsEE][etaBinsEE];

  for (int ii=0; ii<ptBinsEB; ii++) {
    for (int jj=0; jj<etaBinsEB; jj++) {
      hMassEBpass[ii][jj] = new TH1F("hMassEBpass[ii][jj]","hMassEBpass[ii][jj]",80,70,110);
      hMassEBpass[ii][jj]->Sumw2();
      hMassEBfail[ii][jj] = new TH1F("hMassEBfail[ii][jj]","hMassEBfail[ii][jj]",80,70,110);
      hMassEBfail[ii][jj]->Sumw2();
    }
  }
  for (int ii=0; ii<ptBinsEE; ii++) {
    for (int jj=0; jj<etaBinsEE; jj++) {
      hMassEEpass[ii][jj] = new TH1F("hMassEEpass[ii][jj]","hMassEEpass[ii][jj]",80,70,110);
      hMassEEpass[ii][jj]->Sumw2();
      hMassEEfail[ii][jj] = new TH1F("hMassEEfail[ii][jj]","hMassEEfail[ii][jj]",80,70,110);
      hMassEEfail[ii][jj]->Sumw2();
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
    if (probe_fullsel) passed = true;

    for (int ii=0; ii<ptBinsEB; ii++) {
      for (int jj=0; jj<etaBinsEB; jj++) {
	if (probe_absEta>=etaInfEB[jj] && probe_absEta<=etaSupEB[jj]){ 
	  if (probe_pt>=ptInfEB[ii] && probe_pt<=ptSupEB[ii]) { 
	    if (passed) hMassEBpass[ii][jj]->Fill(mass);
	    else hMassEBfail[ii][jj]->Fill(mass); 
	  }
	}
      }
    }

    for (int ii=0; ii<ptBinsEE; ii++) {
      for (int jj=0; jj<etaBinsEE; jj++) {
	if (probe_absEta>=etaInfEE[jj] && probe_absEta<=etaSupEE[jj]){ 
	  if (probe_pt>=ptInfEE[ii] && probe_pt<=ptSupEE[ii]) { 
	    if (passed) hMassEEpass[ii][jj]->Fill(mass);
	    else hMassEEfail[ii][jj]->Fill(mass); 
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
    }
  }
  
  for (int ii=0; ii<ptBinsEE; ii++) {
    for (int jj=0; jj<etaBinsEE; jj++) {
      TString thisName;
      thisName = TString::Format("hMass_%f",ptInfEE[ii])+TString::Format("To%f",ptSupEE[ii])+TString::Format("_%f",etaInfEE[jj])+TString::Format("To%f",etaSupEE[jj])+"_Pass";
      hMassEEpass[ii][jj] ->Write(thisName);
      thisName = TString::Format("hMass_%f",ptInfEE[ii])+TString::Format("To%f",ptSupEE[ii])+TString::Format("_%f",etaInfEE[jj])+TString::Format("To%f",etaSupEE[jj])+"_Fail";
      hMassEEfail[ii][jj] ->Write(thisName);
    }
  }
}
