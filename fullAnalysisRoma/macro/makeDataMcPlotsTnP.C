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
#include "TnPPlot.C"
#include "CMS_lumi.C"

#include <iostream>

#define NSPECIES 2
#define NVARIABLES 3
#define NCUTS 2

void makeDataMcPlotsTnP(float lumi, bool blindData=false)
{
  gROOT->SetStyle("Plain");
  gROOT->ProcessLine(".x ./DiphotonStyle.C");
  gStyle->SetPalette(1);
  gStyle->SetOptStat(0); 
  gStyle->SetOptFit(111110); 
  gStyle->SetOptFile(1); 
  TGaxis::SetExponentOffset(-0.1,-0.008);
  TGaxis::SetMaxDigits(4);
  
  gStyle->SetMarkerStyle(20);
  gStyle->SetMarkerSize(1.0);
  gStyle->SetMarkerColor(1);

  TString suffix="";

  // chiara
  TString species[NSPECIES];
  species[0]="Data";
  species[1]="DY";

  // chiara
  TString files[NSPECIES];
  files[0]="/afs/cern.ch/user/c/crovelli/myWorkspace/public/TaP_fall15_Moriond16_v1/formattedZ/Formatted_singleEle2015Dv1.root";
  files[1]="/afs/cern.ch/user/c/crovelli/myWorkspace/public/TaP_fall15_Moriond16_v1/formattedZ/Formatted_DYLL_largeAndExtension__all1pb.root";

  TString plotsDir="./tnpPlots/";
  TFile* fOut=new TFile("tnpHistos_"+suffix+".root","RECREATE");
  
  char icut[NCUTS][100];
  TH1F* histos[NSPECIES][NCUTS][NVARIABLES];    
  
  // chiara
  TString variables[NVARIABLES];
  variables[0]="mass";
  variables[1]="probe_pt";
  variables[2]="nvtx";

  // chiara
  TString units[NVARIABLES];
  units[0]="GeV";
  units[1]="GeV";
  units[2]="";
  
  // chiara
  int nbins[NVARIABLES];
  nbins[0]=40;
  nbins[1]=75;
  nbins[2]=30;

  // chiara
  float range[NVARIABLES][2]; // N variables, min, max
  range[0][0]=70.;
  range[0][1]=110.;
  range[1][0]=0.;
  range[1][1]=500.;
  range[2][0]=0.;
  range[2][1]=30.;

  // chiara
  TString xaxisLabel[NVARIABLES];
  xaxisLabel[0]="m_{ee}";
  xaxisLabel[1]="p_{T} probe";
  xaxisLabel[2]="number of vertices";

  TString binSize[NVARIABLES];

  for (int z=0;z<NVARIABLES;++z) {
    for (int j=0;j<NCUTS;++j) {
      sprintf(icut[j],"icut%d",j);
      for (int i=0;i<NSPECIES;++i) {
	histos[i][j][z]=new TH1F(variables[z]+"_"+species[i]+"_"+TString(icut[j]),variables[z]+"_"+species[i]+"_"+TString(icut[j]),nbins[z],range[z][0],range[z][1]);
	histos[i][j][z]->Sumw2();
	char binsiz[10];
	sprintf(binsiz,"%2.0f",(range[z][1]-range[z][0])/nbins[z]);
	binSize[z]=TString(binsiz);
      }
    }
  }

  // chiara
  TString cut[NCUTS];
  cut[0]="(mass>70 && mass<110 && abs(tag_absEta)<1.5 && abs(probe_absEta)<1.5)*";
  cut[1]="(mass>70 && mass<110 && (abs(tag_absEta)<1.5 && abs(probe_absEta)>1.5) || (abs(tag_absEta)>1.5 && abs(probe_absEta)<1.5) )*";
  //cut[0]="(probe_fullsel && mass>70 && mass<110 && abs(tag_absEta)<1.5 && abs(probe_absEta)<1.5)*";
  //cut[1]="(probe_fullsel && mass>70 && mass<110 && (abs(tag_absEta)<1.5 && abs(probe_absEta)>1.5) || (abs(tag_absEta)>1.5 && abs(probe_absEta)<1.5) )*";

  char lumistr[100];
  sprintf(lumistr,"%.2f",lumi);
  TString intLumi=TString(lumistr);     
  TFile *_file[NSPECIES];
  TTree *T1[NSPECIES];

  if(!blindData) {
    _file[0]=TFile::Open(files[0]);
    T1[0] = (TTree*)_file[0]->Get("tnpAna/TaPTree");
  } else T1[0] = 0;
  
  for (int i=1;i<NSPECIES;++i) {
    _file[i]=TFile::Open(files[i]);
    T1[i] = (TTree*)_file[i]->Get("tnpAna/TaPTree");
   }

  int nspeciesToRun=NSPECIES;
  for (int z=0;z<NVARIABLES;++z) {
    for (int j=0;j<NCUTS;++j) {
      int firstSpecie = 0;
      if(blindData) firstSpecie = 1;
      for (int i=firstSpecie;i<nspeciesToRun;++i) {
	fOut->cd();
	TString histoName=variables[z]+"_"+species[i]+"_"+TString(icut[j]);
	std::cout << "Producing " << histoName << std::endl;
	if (T1[i]==0) {
	  std::cout << "Species " << i << " Tree not found" << std::endl;
	  return;
	}
	if (i>0) {
	  T1[i]->Project(histoName,variables[z],cut[j]+"weight*"+intLumi);
	  cout << "MC: " << cut[j]+"weight*"+intLumi << endl;
	} else {
	  T1[i]->Project(histoName,variables[z],cut[j]+TString("1"));
	  cout << "data: " << cut[j]+TString("1") << endl;
	}
	std::cout << "Done " << histoName << std::endl;
      }

      // chiara: ad hoc to have correctly normalized
      /*
      for (int z=0;z<NVARIABLES;++z) {
	for (int j=0;j<NCUTS;++j) {
	  for (int i=1;i<NSPECIES;++i) 
	    histos[i][j][z]->Scale(histos[0][j][z]->Integral()/histos[i][j][z]->Integral());
	}
      }	    
      */    
          
      TnPPlot myPlot;
      myPlot.setLumi(lumi);
      myPlot.addLabel("");
      myPlot.setLabel((xaxisLabel[z]).Data());
      myPlot.setUnits((units[z]).Data());
      myPlot.setMCHist(iDY,    histos[1][j][z]);
      if(!blindData) myPlot.setDataHist(histos[0][j][z]);

      // Draw
      //--------------------------------------------------------------------
      TCanvas* c1 = new TCanvas(Form("test_%d_%d_lin", z, j),
				Form("test_%d_%d_lin", z, j));
      
      c1->SetLogy(0);
      myPlot.Draw();
      CMS_lumi(c1,4,1);
      c1->GetFrame()->DrawClone();
      c1->SaveAs(plotsDir+variables[z]+"DataMc_"+TString(icut[j])+"_"+suffix+".png");
      c1->SaveAs(plotsDir+variables[z]+"DataMc_"+TString(icut[j])+"_"+suffix+".pdf");
      c1->SaveAs(plotsDir+variables[z]+"DataMc_"+TString(icut[j])+"_"+suffix+".root");

      TCanvas* c2 = new TCanvas(Form("test_%d_%d_log", z, j),
				Form("test_%d_%d_log", z, j));
      
      c2->SetLogy(1);
      myPlot.Draw();
      c2->GetFrame()->DrawClone();
      c2->SaveAs(plotsDir+variables[z]+"DataMc_"+TString(icut[j])+"_"+suffix+"_log.png");
      c2->SaveAs(plotsDir+variables[z]+"DataMc_"+TString(icut[j])+"_"+suffix+"_log.root");
    }
  }
  
  fOut->Write();
  fOut->Close();
}
