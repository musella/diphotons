#include "TMath.h"
#include "TTree.h"
#include "TStyle.h"
#include "TROOT.h"
#include "TH1F.h"
#include "TFile.h"
#include "TFrame.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TPaveText.h"
#include <iostream>

//#include "../interface/DiphotonUtils.h"

#define NVARIABLES 16
#define NCUTS 4

using namespace std;

void FillHistos(TH1F* histos[NCUTS][NVARIABLES+1][2], vector<float>* vars[NVARIABLES+1], bool matched,
                bool e_EB, bool g_EB, int type, int e_idx, int g_idx)
{
    for(int iVar=0; iVar<NVARIABLES; ++iVar)
    {
        int icut=0;
        if(matched)
        {
            if((iVar<3 && e_EB) || (iVar>=3 && g_EB))
                icut = 0;
            if((iVar<3 && !e_EB) || (iVar>=3 && !g_EB))
                icut = 1;
        }
        else
        {
            if((iVar<3 && e_EB) || (iVar>=3 && g_EB))
                icut = 2;
            if((iVar<3 && !e_EB) || (iVar>=3 && !g_EB))
                icut = 3;
        }
        int idx=iVar<3 ? e_idx : g_idx;        
        histos[icut][iVar][type]->Fill(vars[iVar]->at(idx));
    }
}

int main(int argc, char* argv[])
{
    gROOT->SetStyle("Plain");
    gStyle->SetPalette(1);
    gStyle->SetOptStat(0); 
    gStyle->SetOptFit(111110); 
    gStyle->SetOptFile(1); 
  
    gStyle->SetMarkerStyle(20);
    gStyle->SetMarkerSize(1.0);
    gStyle->SetMarkerColor(1);
    gStyle->SetOptStat("");

    TString plotsDir="TaPplots/";
    system("mkdir -p TaPplots");

    TFile* fOut=new TFile("TaPplots/TaP_histos.root","RECREATE");
  
    char icut[NCUTS][100];
    sprintf(icut[0],"EB_matched");
    sprintf(icut[1],"EE_matched");
    sprintf(icut[2],"EB_unmatched");
    sprintf(icut[3],"EE_unmatched");
    TH1F* histos[NCUTS][NVARIABLES+1][2];    
  
    TString variables[NVARIABLES+1];
    variables[0]="electron_pt";
    variables[1]="electron_eta";
    variables[2]="electron_phi";
    variables[3]="gamma_pt";
    variables[4]="gamma_eta";
    variables[5]="gamma_phi";
    variables[6]="gamma_r9";
    variables[7]="gamma_sieie";
    variables[8]="gamma_scRawEne";    
    variables[9]="gamma_hoe";
    variables[10]="gamma_chiso";
    variables[11]="gamma_phoiso";
    variables[12]="gamma_neuiso";
    variables[13]="gamma_eleveto";
    variables[14]="gamma_presel";
    variables[15]="gamma_fullsel";
    variables[16]="invMass";

    TString units[NVARIABLES+1];
    units[0]="GeV/c^{2}";
    units[1]="";
    units[2]="";
    units[3]="GeV/c^{2}";
    units[4]="";
    units[5]="";
    units[6]="";
    units[7]="";
    units[8]="GeV/c^{2}";
    units[9]="";
    units[10]="";
    units[11]="";
    units[12]="";
    units[13]="";
    units[14]="";
    units[15]="";
    units[16]="GeV/c^{2}";
 
    int nbins[NVARIABLES+1];
    nbins[0]=100;
    nbins[1]=100;
    nbins[2]=50;
    nbins[3]=100;
    nbins[4]=100;
    nbins[5]=50;
    nbins[6]=100;
    nbins[7]=100;
    nbins[8]=100;
    nbins[9]=100;
    nbins[10]=100;
    nbins[11]=100;
    nbins[12]=100;
    nbins[13]=2;
    nbins[14]=2;
    nbins[15]=2;
    nbins[16]=150;
    
    float range[NVARIABLES+1][2]; // N variables, min, max
    //---ele_pt
    range[0][0]=0.;
    range[0][1]=200.;
    //---ele_eta
    range[1][0]=-3;
    range[1][1]=3.; 
    //---ele_phi
    range[2][0]=0.;
    range[2][1]=4.; 
    //---gamma_pt
    range[3][0]=0.;
    range[3][1]=200.;
    //---gamma_eta
    range[4][0]=-3.;
    range[4][1]=3.;
    //---gamma_phi
    range[5][0]=0.;
    range[5][1]=4.;
    //---gamma_r9
    range[6][0]=0.;
    range[6][1]=1.1;
    //---gamma_sieie
    range[7][0]=0.;
    range[7][1]=0.04;
    //---gammascRawEne
    range[8][0]=0.;
    range[8][1]=600.;
    //---gamma hoe
    range[9][0]=0.;
    range[9][1]=0.1;
    //---gamma_chiso
    range[10][0]=0.;
    range[10][1]=3;    
    //---gamma_phoiso
    range[11][0]=0.;
    range[11][1]=3;    
    //---gamma_neuiso
    range[12][0]=0.;
    range[12][1]=3;
    //---gamma eleveto
    range[13][0]=0;
    range[13][1]=2;
    //---gamma presel
    range[14][0]=0;
    range[14][1]=2;
    //---gamma fullsel
    range[15][0]=0;
    range[15][1]=2;
    //---invMass
    range[16][0]=70.;
    range[16][1]=110.;
       
    TString xaxisLabel[NVARIABLES+1];
    xaxisLabel[0]="p_{T}(e)";
    xaxisLabel[1]="#eta(e)";
    xaxisLabel[2]="#phi(e)";
    xaxisLabel[3]="p_{T}(#gamma)";
    xaxisLabel[4]="#eta(#gamma)";
    xaxisLabel[5]="#phi(#gamma)";
    xaxisLabel[6]="R9(#gamma)";
    xaxisLabel[7]="#sigma_{i#etai#eta}";
    xaxisLabel[8]="SCRawEnergy(#gamma)";
    xaxisLabel[9]="HCALoverECAL(#gamma)";
    xaxisLabel[10]="chiso(#gamma)";
    xaxisLabel[11]="phoiso(#gamma)";
    xaxisLabel[12]="neuiso(#gamma)";
    xaxisLabel[13]="eleveto(#gamma)";
    xaxisLabel[14]="presel(#gamma)";
    xaxisLabel[15]="fullsel(#gamma)";
    xaxisLabel[16]="M_{e#gamma}";

    TString binSize[NVARIABLES+1];

    for (int iCut=0;iCut<NCUTS;++iCut)
    {
        for (int iVar=0;iVar<NVARIABLES+1;++iVar)
        {      
            histos[iCut][iVar][0] = new TH1F(variables[iVar]+"_"+TString(icut[iCut])+"_mc", variables[iVar]+"_"+TString(icut[iCut]),
                                            nbins[iVar],range[iVar][0],range[iVar][1]);
            histos[iCut][iVar][1] = new TH1F(variables[iVar]+"_"+TString(icut[iCut])+"_data", variables[iVar]+"_"+TString(icut[iCut]),
                                            nbins[iVar],range[iVar][0],range[iVar][1]);
            for(int iSample=0; iSample<2; ++iSample)
            {
                histos[iCut][iVar][iSample]->SetTitle("");
                histos[iCut][iVar][iSample]->SetXTitle(xaxisLabel[iVar]+" ("+units[iVar]+")");
                histos[iCut][iVar][iSample]->SetYTitle("");
            }
            char binsiz[10];
            sprintf(binsiz,"%2.0f",(range[iVar][1]-range[iVar][0])/nbins[iVar]);
            binSize[iVar]=TString(binsiz);
        }
    }

    //---cuts
    TString cut[NCUTS];

    TString inputFileNameMC = "TaP_output_DY.root";
    TString inputFileNameData = "TaP_output_data.root";
    TFile* mcFile = TFile::Open(inputFileNameMC);
    TFile* dataFile = TFile::Open(inputFileNameData);
    TTree* mcTree = (TTree*)mcFile->Get("tnpAna/TaPtree");
    TTree* dataTree = (TTree*)dataFile->Get("tnpAna/TaPtree");

    vector<float>* vars_mc[NVARIABLES+1];
    vector<float>* vars_data[NVARIABLES+1];
    vector<bool>* mc_e_medium = new vector<bool>;
    vector<bool>* mc_e_match = new vector<bool>;
    vector<bool>* mc_g_match = new vector<bool>;
    vector<int>* mc_e_idx = new vector<int>;
    vector<int>* mc_g_idx = new vector<int>;
    vector<float>* mc_M = new vector<float>;
    vector<bool>* data_e_medium = new vector<bool>;
    vector<bool>* data_e_match = new vector<bool>;
    vector<bool>* data_g_match = new vector<bool>;
    vector<int>* data_e_idx = new vector<int>;
    vector<int>* data_g_idx = new vector<int>;
    vector<float>* data_M = new vector<float>;
    for(int iVar=0; iVar<NVARIABLES; ++iVar)
    {
        vars_mc[iVar] = new vector<float>;
        vars_data[iVar] = new vector<float>;
        mcTree->SetBranchAddress(variables[iVar].Data(), &vars_mc[iVar]);
        dataTree->SetBranchAddress(variables[iVar].Data(), &vars_data[iVar]);
    }
    mcTree->SetBranchAddress("isTagMediumEle", &mc_e_medium);
    mcTree->SetBranchAddress("electron_matchHLT", &mc_e_match);
    mcTree->SetBranchAddress("gamma_matchHLT", &mc_g_match);
    mcTree->SetBranchAddress("eleIndex", &mc_e_idx);
    mcTree->SetBranchAddress("gammaIndex", &mc_g_idx);
    mcTree->SetBranchAddress("invMass", &mc_M);
    dataTree->SetBranchAddress("isTagMediumEle", &data_e_medium);
    dataTree->SetBranchAddress("electron_matchHLT", &data_e_match);
    dataTree->SetBranchAddress("gamma_matchHLT", &data_g_match);
    dataTree->SetBranchAddress("eleIndex", &data_e_idx);
    dataTree->SetBranchAddress("gammaIndex", &data_g_idx);
    dataTree->SetBranchAddress("invMass", &data_M);

    //---Fill MC histos
    int count=0;
    for(int iEntry=0; iEntry<mcTree->GetEntriesFast(); ++iEntry)
    {
        mcTree->GetEntry(iEntry);
        for(unsigned int iM=0; iM<mc_M->size(); ++iM)
        {
            if(mc_M->at(iM) < 70 || mc_M->at(iM) > 110 ||
               !mc_e_medium->at(mc_e_idx->at(iM)) || !vars_mc[14]->at(mc_g_idx->at(iM)))
                continue;
            if(fabs(vars_mc[1]->at(mc_e_idx->at(iM))) > 1.5 && fabs(vars_mc[4]->at(mc_g_idx->at(iM))) > 1.5)
                continue;
            if(mc_e_match->at(mc_e_idx->at(iM)) && mc_g_match->at(mc_g_idx->at(iM)))
            {
                if(fabs(vars_mc[1]->at(mc_e_idx->at(iM)))<1.5 && fabs(vars_mc[4]->at(mc_g_idx->at(iM)))<1.5)
                    histos[0][NVARIABLES][0]->Fill(mc_M->at(iM));
                else
                    histos[1][NVARIABLES][0]->Fill(mc_M->at(iM));
                FillHistos(histos, vars_mc, 1,
                           fabs(vars_mc[1]->at(mc_e_idx->at(iM)))<1.5,
                           fabs(vars_mc[4]->at(mc_g_idx->at(iM)))<1.5,
                           0, mc_e_idx->at(iM), mc_g_idx->at(iM));
            }
            else
                FillHistos(histos, vars_mc, 0,
                           fabs(vars_mc[1]->at(mc_e_idx->at(iM)))<1.5,
                           fabs(vars_mc[4]->at(mc_g_idx->at(iM)))<1.5,
                           0, mc_e_idx->at(iM), mc_g_idx->at(iM));
            ++count;
        }
    }
    cout << "MC pairs: " << count << endl;
    //---Fill Data histos
    count=0;
    for(int iEntry=0; iEntry<dataTree->GetEntriesFast(); ++iEntry)
    {
        dataTree->GetEntry(iEntry);
        for(unsigned int iM=0; iM<data_M->size(); ++iM)
        {
            if(data_M->at(iM) < 70 || data_M->at(iM) > 110 ||
               !data_e_medium->at(data_e_idx->at(iM)) || !vars_data[14]->at(data_g_idx->at(iM)))
                continue;
            if(fabs(vars_data[1]->at(data_e_idx->at(iM))) > 1.5 && fabs(vars_data[4]->at(data_g_idx->at(iM))) > 1.5)
                continue;
            if(data_e_match->at(data_e_idx->at(iM)) && data_g_match->at(data_g_idx->at(iM)))
            {
                if(fabs(vars_data[1]->at(data_e_idx->at(iM)))<1.5 && fabs(vars_data[4]->at(data_g_idx->at(iM))) < 1.5)
                    histos[0][NVARIABLES][1]->Fill(data_M->at(iM));
                else
                    histos[1][NVARIABLES][1]->Fill(data_M->at(iM));
                FillHistos(histos, vars_data, 1,
                           fabs(vars_data[1]->at(data_e_idx->at(iM)))<1.5,
                           fabs(vars_data[4]->at(data_g_idx->at(iM)))<1.5,
                           1, data_e_idx->at(iM), data_g_idx->at(iM));
            }
            else
                FillHistos(histos, vars_data, 0,
                           fabs(vars_data[1]->at(data_e_idx->at(iM)))<1.5,
                           fabs(vars_data[4]->at(data_g_idx->at(iM)))<1.5,
                           1, data_e_idx->at(iM), data_g_idx->at(iM));
            ++count;
        }
    }
    cout << "Data pairs: " << count << endl;
    
    //---Draw Plots
    for(int iVar=0; iVar<NVARIABLES+1; ++iVar)
    {
        for(int iCut=0; iCut<NCUTS; ++iCut)
        {
            fOut->cd();

            //---Draw and print
            TCanvas* c1 = new TCanvas(Form("tap_%d_%d", iVar, iCut),
                                      Form("tap_%d_%d", iVar, iCut));

            c1->SetLogy(0);
            histos[iCut][iVar][0]->Sumw2();
            histos[iCut][iVar][1]->Sumw2();
            histos[iCut][iVar][0]->Scale(1/histos[iCut][iVar][0]->Integral());
            histos[iCut][iVar][1]->Scale(1/histos[iCut][iVar][1]->Integral());
            //---log y scale for iso variables
            if(variables[iVar].Contains("iso") || variables[iVar].Contains("hoe"))
                c1->SetLogy(1);
            else
            {
                histos[iCut][iVar][0]->SetAxisRange(0, max(histos[iCut][iVar][0]->GetMaximum(),
                                                           histos[iCut][iVar][1]->GetMaximum())*1.1, "Y");            
                histos[iCut][iVar][1]->SetAxisRange(0, max(histos[iCut][iVar][0]->GetMaximum(),
                                                           histos[iCut][iVar][1]->GetMaximum())*1.1, "Y");
            }
            histos[iCut][iVar][0]->SetFillColor(kRed+1);
            histos[iCut][iVar][1]->SetMarkerStyle(20);
            histos[iCut][iVar][1]->SetFillStyle(0);
            histos[iCut][iVar][0]->Draw("hist");
            histos[iCut][iVar][1]->Draw("Psame");
            c1->GetFrame()->DrawClone();            
            c1->SaveAs(plotsDir+variables[iVar]+"_"+TString(icut[iCut])+".pdf");
            c1->SaveAs(plotsDir+variables[iVar]+"_"+TString(icut[iCut])+".png");

            histos[iCut][iVar][0]->Write();
            histos[iCut][iVar][1]->Write();
        }
    }
    fOut->Close();
}
