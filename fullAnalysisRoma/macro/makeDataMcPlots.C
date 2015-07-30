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

#define NSPECIES 5
#define NVARIABLES 18
#define NCUTS 1

void makeDataMcPlots(float lumi, bool blindData=false, int signalFactor=1)
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
  species[0]="Data";      
  if(signalFactor==1) species[1]="G1500";
  else {
    char scaleF[10];
    sprintf(scaleF,"%dxG1500",signalFactor);
    species[1]=TString(scaleF);
  }
  species[2]="qcd";
  species[3]="gjets";
  species[4]="gg";

  // chiara
  TString files[NSPECIES];
  files[0]="data/noHLT/mergedFinal/DoubleEG.root";
  files[1]="data/noHLT/mergedFinal/RSGravToGG_kMpl-01_M-1000.root";
  files[2]="data/noHLT/mergedFinal/QCD.root";
  files[3]="data/noHLT/mergedFinal/GJets.root";
  files[4]="data/noHLT/mergedFinal/GGJets.root";

  TString plotsDir="./diphotPlots/";

  TFile* fOut=new TFile("diPhotHistos_"+suffix+".root","RECREATE");
  
  char icut[NCUTS][100];
  TH1F* histos[NSPECIES][NCUTS][NVARIABLES];    
  
  // chiara
  TString variables[NVARIABLES];
  variables[0]="mgg";
  variables[1]="ptgg";
  variables[2]="pt1";
  variables[3]="pt2";
  variables[4]="eta1";
  variables[5]="eta2";
  variables[6]="r91";
  variables[7]="r92";
  variables[8]="nvtx";
  variables[9]="eventClass";
  variables[10]="chiso1";
  variables[11]="chiso2";
  variables[12]="phoiso1";
  variables[13]="phoiso2";
  variables[14]="sieie1";
  variables[15]="sieie2";
  variables[16]="hoe1";
  variables[17]="hoe2";

  // chiara
  TString units[NVARIABLES];
  units[0]="GeV";
  units[1]="GeV";
  units[2]="GeV";
  units[3]="GeV";
  units[4]="";
  units[5]="";
  units[6]="";
  units[7]="";
  units[8]="";
  units[9]="";
  units[10]="GeV";
  units[11]="GeV";
  units[12]="GeV";
  units[13]="GeV";
  units[14]="";
  units[15]="";
  units[16]="";
  units[17]="";

  // chiara
  int nbins[NVARIABLES];
  nbins[0]=30;
  //nbins[1]=50;
  nbins[1]=20;
  // nbins[2]=50;
  // nbins[3]=50;
  nbins[2]=20;
  nbins[3]=20;
  nbins[4]=20;
  nbins[5]=20;
  nbins[6]=10;
  nbins[7]=10;
  nbins[8]=20;
  nbins[9]=4;
  nbins[10]=10;
  nbins[11]=10;
  nbins[12]=10;
  nbins[13]=10;
  nbins[14]=50;
  nbins[15]=50;
  nbins[16]=20;
  nbins[17]=20;

  // chiara
  float range[NVARIABLES][2]; // N variables, min, max
  // mgg
  range[0][0]=150.;
  range[0][1]=1050.;
  // ptgg
  range[1][0]=0.;
  // range[1][1]=3000.;
  range[1][1]=1000.;
  // pt1
  range[2][0]=0.;
  // range[2][1]=2000.;
  range[2][1]=500.;
  // pt2
  range[3][0]=0.;
  //range[3][1]=1000.;
  range[3][1]=500.;
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
  // # vtx
  range[8][0]=0.;
  range[8][1]=40.;  
  // # event class
  range[9][0]=-0.5;
  range[9][1]=3.5;  
  // charged isolation 1
  range[10][0]=0.;
  range[10][1]=5.;  
  // charged isolation 2
  range[11][0]=0.;
  range[11][1]=5.;  
  // photon isolation 1
  range[12][0]=0.;
  range[12][1]=10.;  
  // photon isolation 2
  range[13][0]=0.;
  range[13][1]=10.;  
  // sIeIe 1
  range[14][0]=0.;
  range[14][1]=0.05;  
  // sIeIe 2
  range[15][0]=0.;
  range[15][1]=0.05;  
  // H/E 1
  range[16][0]=0.;
  range[16][1]=0.05;  
  // H/E 2
  range[17][0]=0.;
  range[17][1]=0.05;  

  // chiara
  TString xaxisLabel[NVARIABLES];
  xaxisLabel[0]="m(#gamma#gamma)";
  xaxisLabel[1]="p_{T}(#gamma #gamma)";
  xaxisLabel[2]="p_{T}(#gamma1)";
  xaxisLabel[3]="p_{T}(#gamma2)";
  xaxisLabel[4]="#eta(#gamma1)";
  xaxisLabel[5]="#eta(#gamma2)";
  xaxisLabel[6]="R9(#gamma1)";
  xaxisLabel[7]="R9(#gamma2)";
  xaxisLabel[8]="Vertices";
  xaxisLabel[9]="Class";
  xaxisLabel[10]="Charged isolation(#gamma1)";
  xaxisLabel[11]="Charged isolation(#gamma2)";
  xaxisLabel[12]="Photon isolation(#gamma1)";
  xaxisLabel[13]="Photon isolation(#gamma2)";
  xaxisLabel[14]="sigmaIetaIeta(#gamma1)";
  xaxisLabel[15]="sigmaIetaIeta(#gamma2)";
  xaxisLabel[16]="H/E(#gamma1)";
  xaxisLabel[17]="H/E(#gamma2)";


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
  /*
  cut[0]="(pt1>200 && pt2>200 && mgg>500)*";
  cut[1]="(pt1>200 && pt2>200 && mgg>500 && eventClass==0)*";
  cut[2]="(pt1>200 && pt2>200 && mgg>500 && eventClass==1)*";
  cut[3]="(pt1>200 && pt2>200 && mgg>500 && eventClass==2)*";
  cut[4]="(pt1>200 && pt2>200 && mgg>500 && eventClass==3)*";
  */
  // cut[0]="(pt1>50 && pt2>50 && mgg>50)*";
  cut[0]="(pt1>50 && pt2>50 && mgg>250)*";
  // cut[0]="(pt1>100 && pt2>100 && mgg>250)*";

  char lumistr[5];
  sprintf(lumistr,"%.1f",lumi);
  TString intLumi=TString(lumistr);     
  TFile *_file[NSPECIES];
  TTree *T1[NSPECIES];
  
  char lumiwgt[10];
  sprintf(lumiwgt,"%f*",lumi);
  cout << "lumiwgt = " << lumiwgt << endl;

  if(!blindData) {  
    _file[0]=TFile::Open(files[0]);   
    T1[0] = (TTree*)_file[0]->Get("DiPhotonTree");    
  } else T1[0] = 0;   

  for (int i=1;i<NSPECIES;++i) {
    _file[i]=TFile::Open(files[i]);
    T1[i] = (TTree*)_file[i]->Get("DiPhotonTree");
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
	  cout << "non dati: " << cut[j]+"weight*"+intLumi << endl;
	} else { 
	  T1[i]->Project(histoName,variables[z],cut[j]+TString("1"));
	  cout << "dati: " << cut[j]+TString("1") << endl;
	}
	std::cout << "Done " << histoName << std::endl;
      }
          
      DiphotonPlot myPlot;
      myPlot.setLumi(lumi);
      myPlot.addLabel("");
      myPlot.setLabel((xaxisLabel[z]).Data());
      myPlot.setUnits((units[z]).Data());
      myPlot.setMCHist(iQCD,   histos[2][j][z]);
      myPlot.setMCHist(iGJets, histos[3][j][z]);
      myPlot.setMCHist(iGG,    histos[4][j][z]);
      // myPlot.setMCHist(iRS,    histos[1][j][z]);

      if(!blindData) myPlot.setDataHist(histos[0][j][z]);                 
      cout << "data: " << histos[0][j][z]->GetEntries() << " " << histos[0][j][z]->Integral() << endl;
      cout << "qcd: "  << histos[2][j][z]->GetEntries() << " " << histos[2][j][z]->Integral() << endl;
      cout << "gj: "   << histos[3][j][z]->GetEntries() << " " << histos[3][j][z]->Integral() << endl;
      cout << "gg: "   << histos[4][j][z]->GetEntries() << " " << histos[4][j][z]->Integral() << endl;

      // Draw
      //--------------------------------------------------------------------
      TCanvas* c1 = new TCanvas(Form("test_%d_%d_lin", z, j),
				Form("test_%d_%d_lin", z, j));
      
      c1->SetLogy(0);
      myPlot.Draw();
      c1->GetFrame()->DrawClone();
      c1->SaveAs(plotsDir+variables[z]+"DataMc_"+TString(icut[j])+"_"+suffix+".png");
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
