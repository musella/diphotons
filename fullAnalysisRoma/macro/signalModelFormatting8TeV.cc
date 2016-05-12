#include <TFile.h>
#include <TTree.h>
#include <TBranch.h>
#include <iostream>
#include <TRandom.h>
#include "TLeaf.h"   

using namespace std;

void signalModelFormat8TeV(const char* filename, TString kpl, TString theMass) {

  // initialize the random number 
  TRandom myRandom(12345);

  cout << "Formatting " << filename << " from Otman's to our format for signal model studies" << endl;

  TFile *fileOrig = 0;  
  fileOrig = TFile::Open(filename);

  TTree *treeOrig = 0;
  if( fileOrig ) {
    fileOrig->cd();
    TString theTree = TString::Format("fTree");
    TString theDir  = TString::Format("diphotonSignalMCAnalyzer/"+theTree);
    cout << "Reading " << theDir << endl;
    treeOrig = (TTree*)fileOrig->Get(theDir);
  } else {
    cout << "File " << filename << " not existing !" << endl;
    return;
  }

  fileOrig->cd();
  if (!treeOrig) {
    cout << "Tree not existing !" << endl;
    return;
  }

  treeOrig->SetMakeClass(1);

  // number of entries saved in the first tree   
  int nentriesOrig = treeOrig->GetEntries();

  TString theNewFile = TString::Format("/tmp/crovelli/FormSigMod_kpl"+kpl+"_M"+theMass+".root");
  cout << "OutputFile: " << theNewFile << endl;
  TFile *fileNew = TFile::Open(theNewFile,"RECREATE");
  TTree *treeNew = new TTree("DiPhotonTree","reduced tree for signal model studies");

  std::vector<TTree*> trees;
  trees.push_back(treeNew);

  // Declaration of leaf types
  Double_t Photon1_pt;
  Double_t Photon1_eta;
  Double_t Photon2_pt;
  Double_t Photon2_eta;
  Double_t DiphotonGen_Minv;
  Double_t Diphoton_Minv;
  Double_t MCPUWeight;

  // List of branches
  TBranch *b_Photon1     = (TBranch*)treeOrig->GetBranch("Photon1");
  TBranch *b_Photon2     = (TBranch*)treeOrig->GetBranch("Photon2");
  TBranch *b_DiphotonGen = (TBranch*)treeOrig->GetBranch("DiphotonGen"); 
  TBranch *b_Diphoton    = (TBranch*)treeOrig->GetBranch("Diphoton");
  TBranch *b_MCPUWeight  = (TBranch*)treeOrig->GetBranch("MCPUWeight");

  // New variables  
  float mgg, unsmearedMgg, mggGen;
  int eventClass;
  float puweight;

  for(int i=0; i<(int)trees.size();i++) {
    TTree *theTreeNew = trees[i];

    // New branches  
    theTreeNew->Branch("mgg", &mgg, "mgg/F");
    theTreeNew->Branch("unsmearedMgg", &unsmearedMgg, "unsmearedMgg/F");
    theTreeNew->Branch("mggGen", &mggGen, "mggGen/F");
    theTreeNew->Branch("eventClass", &eventClass, "eventClass/I");
    theTreeNew->Branch("puweight", &puweight, "puweight/F");
  }

  for(int i=0; i<nentriesOrig; i++) {

    b_Photon1->GetEntry(i);
    Photon1_pt  = b_Photon1->GetLeaf("pt")->GetValue();
    Photon1_eta = b_Photon1->GetLeaf("eta")->GetValue();

    b_Photon2->GetEntry(i);
    Photon2_pt  = b_Photon2->GetLeaf("pt")->GetValue();
    Photon2_eta = b_Photon2->GetLeaf("eta")->GetValue();

    b_DiphotonGen->GetEntry(i);
    DiphotonGen_Minv = b_DiphotonGen->GetLeaf("Minv")->GetValue();

    b_Diphoton->GetEntry(i);
    Diphoton_Minv = b_Diphoton->GetLeaf("Minv")->GetValue();
    
    b_MCPUWeight->GetEntry(i);
    MCPUWeight = b_MCPUWeight->GetLeaf("MCPUWeight")->GetValue();

    // 2012 kine selection  
    if (Diphoton_Minv<=300) continue;
    if (Photon1_pt<80)      continue;
    if (Photon2_pt<80)      continue;

    // 2012 was barrel-barrel only
    if (fabs(Photon1_eta)>1.5) continue;
    if (fabs(Photon2_eta)>1.5) continue;
    eventClass = 0;

    puweight = MCPUWeight;

    // gen level
    mggGen = DiphotonGen_Minv;

    // 2012 smearings: 1% on the mass    
    unsmearedMgg = Diphoton_Minv;    
    float theGaussMean  = 1.;
    float theGaussSigma = 0.01;
    float fromGauss = myRandom.Gaus(theGaussMean,theGaussSigma);
    mgg = Diphoton_Minv*fromGauss;

    treeNew->Fill();

  } // loop over tree entries

  // new info       
  fileNew->ls();
  fileNew->cd();
  treeNew->Write();
  fileNew->Close();
  
  fileOrig->cd();
  fileOrig->Close();
};

