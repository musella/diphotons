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
#include "./DiphotonPlot.C"

#include <iostream>

#define NSPECIES 4
#define NVARIABLES 1
#define NCUTS 5

void makeMCPlots(float lumi, int signalFactor=1)
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
  if(signalFactor==1) species[0]="G1500";
  else {
    char scaleF[10];
    sprintf(scaleF,"%dxG1500",signalFactor);
    species[0]=TString(scaleF);
  }
  species[1]="qcd";
  species[2]="gjets";
  species[3]="gg";

  // chiara
  TString files[NSPECIES];
  files[0]="data/newSelection/mergedFinal/RSGravToGG_kMpl-01_M-1500.root";
  files[1]="data/newSelection/mergedFinal/QCD.root";
  files[2]="data/newSelection/mergedFinal/GJets.root";
  files[3]="data/newSelection/mergedFinal/GGJets.root";

  TString plotsDir="./diphotPlots/";

  TFile* fOut=new TFile("diPhotHistos_"+suffix+".root","RECREATE");
  
  char icut[NCUTS][100];
  TH1F* histos[NSPECIES][NCUTS][NVARIABLES];    
  
  // chiara
  TString variables[NVARIABLES];
  variables[0]="mgg";
  /*
  variables[1]="ptgg";
  variables[2]="pt1";
  variables[3]="pt2";
  variables[4]="eta1";
  variables[5]="eta2";
  variables[6]="r91";
  variables[7]="r92";
  */

  // chiara
  TString units[NVARIABLES];
  units[0]="GeV/c^{2}";
  /*
  units[1]="GeV/c";
  units[2]="GeV/c";
  units[3]="GeV/c";
  units[4]="";
  units[5]="";
  units[6]="";
  units[7]="";
  */

  // chiara
  int nbins[NVARIABLES];
  nbins[0]=60;
  /*
  nbins[1]=50;
  nbins[2]=50;
  nbins[3]=50;
  nbins[4]=50;
  nbins[5]=50;
  nbins[6]=20;
  nbins[7]=20;
  */

  // chiara
  float range[NVARIABLES][2]; // N variables, min, max
  // mgg
  range[0][0]=0.;
  range[0][1]=6000.;
  /*
  // ptgg
  range[1][0]=0.;
  range[1][1]=3000.;
  // pt1
  range[2][0]=0.;
  range[2][1]=2000.;
  // pt2
  range[3][0]=0.;
  range[3][1]=1000.;
  // eta1
  range[4][0]=-3.;
  range[4][1]=3.;
  // eta2
  range[5][0]=-3.;
  range[5][1]=3.;
  // r91
  range[6][0]=0.8;
  range[6][1]=1.;
  // r91
  range[7][0]=0.8;
  range[7][1]=1.;
  */

  // chiara
  TString xaxisLabel[NVARIABLES];
  xaxisLabel[0]="m(#gamma#gamma)";
  /*
  xaxisLabel[1]="p_{T}(#gamma #gamma)";
  xaxisLabel[2]="p_{T}(#gamma1)";
  xaxisLabel[3]="p_{T}(#gamma2)";
  xaxisLabel[4]="#eta(#gamma1)";
  xaxisLabel[5]="#eta(#gamma2)";
  xaxisLabel[6]="R9(#gamma1)";
  xaxisLabel[7]="R9(#gamma2)";
  */

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
  cut[0]="(pt1>200 && pt2>200 && mgg>500)*";
  cut[1]="(pt1>200 && pt2>200 && mgg>500 && eventClass==0)*";
  cut[2]="(pt1>200 && pt2>200 && mgg>500 && eventClass==1)*";
  cut[3]="(pt1>200 && pt2>200 && mgg>500 && eventClass==2)*";
  cut[4]="(pt1>200 && pt2>200 && mgg>500 && eventClass==3)*";

  char lumistr[5];
  sprintf(lumistr,"%.1f",lumi);
  TString intLumi=TString(lumistr);     
  TFile *_file[NSPECIES];
  TTree *T1[NSPECIES];
  
  for (int i=0;i<NSPECIES;++i) {
    _file[i]=TFile::Open(files[i]);
    T1[i] = (TTree*)_file[i]->Get("DiPhotonTree");
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
          
      DiphotonPlot myPlot;
      myPlot.setLumi(lumi);
      myPlot.addLabel("");
      myPlot.setLabel((xaxisLabel[z]).Data());
      myPlot.setUnits((units[z]).Data());
      myPlot.setMCHist(iQCD,   histos[1][j][z]);
      myPlot.setMCHist(iGJets, histos[2][j][z]);
      myPlot.setMCHist(iGG,    histos[3][j][z]);
      myPlot.setMCHist(iRS,    histos[0][j][z]);

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
