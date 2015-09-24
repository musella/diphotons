#include "TMath.h"
#include "TTree.h"
#include "TStyle.h"
#include "TROOT.h"
#include "TH1F.h"
#include "TFile.h"
#include "THStack.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TPaveText.h"
#include "./TnPPlot.C"

#include <iostream>

#define NSPECIES 6
#define NVARIABLES 1
#define NCUTS 1

void makeMCPlots(float lumi)
{
  gROOT->SetStyle("Plain");
  gROOT->ProcessLine(".x ./DiphotonStyle.C");
  gStyle->SetPalette(1);
  gStyle->SetOptStat(0); 
  gStyle->SetOptFit(111110); 
  gStyle->SetOptFile(1); 
  
  gStyle->SetMarkerStyle(20);
  gStyle->SetMarkerSize(1.0);
  gStyle->SetMarkerColor(1);

  TString suffix="";

  // chiara
  TString species[NSPECIES];
  species[5]="DY";
  species[0]="Wjets";
  species[1]="WW";
  species[2]="WZ";
  species[3]="ZZ";
  species[4]="ttjets";

  // chiara
  TString files[NSPECIES];
  files[5]="/afs/cern.ch/work/c/crovelli/public/spring15_v3v8/formatted/Formatted_DYLL_all.root";
  files[0]="/afs/cern.ch/work/c/crovelli/public/spring15_v3v8/formatted/Formatted_WJetsToLNu_HT-600ToInf_ALL.root";
  files[1]="/afs/cern.ch/work/c/crovelli/public/spring15_v3v8/formatted/Formatted_WW2L2nu_ALL.root";
  files[2]="/afs/cern.ch/work/c/crovelli/public/spring15_v3v8/formatted/Formatted_WZjets_ALL.root";
  files[3]="/afs/cern.ch/work/c/crovelli/public/spring15_v3v8/formatted/Formatted_ZZ4l_ALL.root";
  files[4]="/afs/cern.ch/work/c/crovelli/public/spring15_v3v8/formatted/Formatted_TTjets_ALL.root";

  TString plotsDir="./tnpPlots/";

  TFile* fOut=new TFile("tnpHistos_"+suffix+".root","RECREATE");
  
  char icut[NCUTS][100];
  TH1F* histos[NSPECIES][NCUTS][NVARIABLES];    
  
  // chiara
  TString variables[NVARIABLES];
  variables[0]="mass";

  // chiara
  TString units[NVARIABLES];
  units[0]="GeV/c^{2}";

  // chiara
  int nbins[NVARIABLES];
  nbins[0]=65;

  // chiara
  float range[NVARIABLES][2]; // N variables, min, max
  // mgg
  range[0][0]=200.;
  range[0][1]=1500.;

  // chiara
  TString xaxisLabel[NVARIABLES];
  xaxisLabel[0]="mee";

  TString binSize[NVARIABLES];

  for (int z=0;z<NVARIABLES;++z) {
    for (int j=0;j<NCUTS;++j) {
      sprintf(icut[j],"icut%d",j);
      for (int i=0;i<NSPECIES;++i) {
	histos[i][j][z]=new TH1F(variables[z]+"_"+species[i]+"_"+TString(icut[j]),variables[z]+"_"+species[i]+"_"+TString(icut[j]),nbins[z],range[z][0],range[z][1]);
	if(i==0)
	  histos[i][j][z]->Sumw2();
	char binsiz[10];
	sprintf(binsiz,"%2.0f",(range[z][1]-range[z][0])/nbins[z]);
	binSize[z]=TString(binsiz);
      }
    }
  }

  // chiara
  TString cut[NCUTS];
  cut[0]="(mass>200 && mass<40000)*";

  char lumistr[5];
  sprintf(lumistr,"%.1f",lumi);
  TString intLumi=TString(lumistr);     
  TFile *_file[NSPECIES];
  TTree *T1[NSPECIES];
  
  for (int i=0;i<NSPECIES;++i) {
    _file[i]=TFile::Open(files[i]);
    T1[i] = (TTree*)_file[i]->Get("tnpAna/TaPTree");
   }

  int nspeciesToRun=NSPECIES;
  for (int z=0;z<NVARIABLES;++z) {
    for (int j=0;j<NCUTS;++j) {
      int firstSpecie = 0;
      for (int i=firstSpecie;i<nspeciesToRun;++i) {
	fOut->cd();
	TString histoName=variables[z]+"_"+species[i]+"_"+TString(icut[j]);
	std::cout << "Producing " << histoName << std::endl;
	if (T1[i]==0) {
	  std::cout << "Species " << i << " Tree not found" << std::endl;
	  return;
	}
	T1[i]->Project(histoName,variables[z],cut[j]+"weight*"+intLumi);
	std::cout << "Done " << histoName << std::endl;
      }
          
      TnPPlot myPlot;
      myPlot.setLumi(lumi);
      myPlot.addLabel("");
      myPlot.setLabel((xaxisLabel[z]).Data());
      myPlot.setUnits((units[z]).Data());
      myPlot.setMCHist(iDY,    histos[5][j][z]);
      myPlot.setMCHist(iWJets, histos[0][j][z]);
      myPlot.setMCHist(iWW,    histos[1][j][z]);
      myPlot.setMCHist(iWZ,    histos[2][j][z]);
      myPlot.setMCHist(iZZ,    histos[3][j][z]);
      myPlot.setMCHist(iTT,    histos[4][j][z]);

      // Draw
      //--------------------------------------------------------------------
      TCanvas* c1 = new TCanvas(Form("test_%d_%d_lin", z, j),
				Form("test_%d_%d_lin", z, j));
      
      c1->SetLogy(0);
      myPlot.Draw();
      c1->GetFrame()->DrawClone();
      c1->SaveAs(plotsDir+variables[z]+"MCOnly_"+TString(icut[j])+"_"+suffix+".png");

      TCanvas* c2 = new TCanvas(Form("test_%d_%d_log", z, j),
				Form("test_%d_%d_log", z, j));
      
      c2->SetLogy(1);
      myPlot.Draw();
      c2->GetFrame()->DrawClone();
      c2->SaveAs(plotsDir+variables[z]+"MCOnly_"+TString(icut[j])+"_"+suffix+"_log.png");
    }
  }
  
  fOut->Write();
  fOut->Close();
}
